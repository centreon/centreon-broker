/*
** Copyright 2020 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/
#include "com/centreon/broker/pool.hh"

#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;

size_t pool::_pool_size(0);

/**
 * @brief The way to access to the pool.
 *
 * @return a reference to the pool.
 */
pool& pool::instance() {
  static pool instance;
  return instance;
}

/**
 * @brief A static method to access the IO context.
 *
 * @return the IO context.
 */
asio::io_context& pool::io_context() {
  return instance()._io_context;
}

/**
 * @brief Default constructor. Hidden, is called throw the static instance()
 * method.
 */
pool::pool()
    : _io_context(_pool_size),
      _worker(new asio::io_service::work(_io_context)),
      _closed(true),
      _timer(_io_context),
      _stats_running{false} {
  _start();
  _start_stats();
}

/**
 * @brief Start the stats of the pool. This method is called by the stats engine
 * when it is ready.
 *
 * @param stats The pointer used by the pool to set its data in the stats
 * engine.
 */
void pool::_start_stats() {
  _stats_running = true;
  _timer.expires_after(std::chrono::seconds(10));
  _timer.async_wait(
      std::bind(&pool::_check_latency, this, std::placeholders::_1));
}

/**
 * @brief Start the thread pool used for the tcp connections.
 *
 */
void pool::_start() {
  std::lock_guard<std::mutex> lock(_closed_m);
  if (_closed) {
    _closed = false;
    /* We fix the thread pool used by asio to hardware concurrency and at
     * least, we want 2 threads. So in case of two sockets, one in and one out,
     * they should be managed by those two threads. This is empirical, and maybe
     * will be changed later. */
    size_t count = _pool_size == 0
                       ? std::max(std::thread::hardware_concurrency(), 2u)
                       : _pool_size;

    log_v2::core()->info("Starting the TCP thread pool of {} threads", count);
    for (uint32_t i = 0; i < count; i++) {
      _pool.emplace_back([this] { _io_context.run(); });
      pthread_setname_np(_pool[i].native_handle(), "pool_thread");
    }
  }
}

/**
 * @brief Destructor
 */
pool::~pool() noexcept {
  _stop();
}

/**
 * @brief Stop the thread pool.
 */
void pool::_stop() {
  if (_stats_running) {
    std::promise<bool> p;
    std::future<bool> f(p.get_future());
    _stats_running = false;
    asio::post(_timer.get_executor(), [this, &p] {
        _timer.cancel();
        p.set_value(true);
        });
    f.get();
  }

  log_v2::core()->trace("Stopping the TCP thread pool");
  std::lock_guard<std::mutex> lock(_closed_m);
  if (!_closed) {
    _closed = true;
    _worker.reset();
    for (auto& t : _pool)
      if (t.joinable())
        t.join();
  }
  log_v2::core()->trace("No remaining thread in the pool");
}

/**
 * @brief Static method to set the thread pool size. A positive integer or
 * 0 to leave broker choosing the size with the formula max(2, number of CPUs /
 * 2).
 *
 * @param size The size.
 */
void pool::set_size(size_t size) noexcept {
  _pool_size = size;
}

/**
 * @brief Returns the number of threads used in the pool.
 *
 * @return a size.
 */
size_t pool::get_current_size() const {
  std::lock_guard<std::mutex> lock(_closed_m);
  return _pool.size();
}

/**
 * @brief The function whose role is to compute the latency. It makes the
 * computation every 10s.
 *
 */
void pool::_check_latency(asio::error_code ec) {
  if (ec)
    log_v2::core()->info("pool: the latency check encountered an error: {}",
                         ec.message());
  else {
    auto start = std::chrono::system_clock::now();
    asio::post(_io_context, [start, this] {
      auto end = std::chrono::system_clock::now();
      auto duration = std::chrono::duration<double, std::milli>(end - start);
      _latency = duration.count();
      log_v2::core()->trace("Thread pool latency {:.3f}ms", _latency);
    });
    if (_stats_running) {
      _timer.expires_after(std::chrono::seconds(10));
      _timer.async_wait(
          std::bind(&pool::_check_latency, this, std::placeholders::_1));
    }
  }
}

/**
 * @brief Get the pool latency in ms. This value is computed
 * every 10s and represents the duration between the time point we tell the
 * thread pool to execute a task and the time point when it really executes this
 * task. A latency of 0ms means the pool has enough free threads to execute
 * tasks immediatly.
 *
 * @return A duration in ms.
 */
double pool::get_latency() const {
  return _latency;
}
