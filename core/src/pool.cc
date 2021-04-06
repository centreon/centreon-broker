/*
** Copyright 2020-2021 Centreon
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
#include "com/centreon/broker/stats/center.hh"

using namespace com::centreon::broker;

pool* pool::_instance{nullptr};

std::mutex pool::_init_m;

/**
 * @brief The way to access to the pool.
 *
 * @return a reference to the pool.
 */
pool& pool::instance() {
  assert(pool::_instance);
  return *_instance;
}

void pool::load(size_t size) {
  std::lock_guard<std::mutex> lck(_init_m);
  if (_instance == nullptr)
    _instance = new pool(size);
  else
    log_v2::core()->error("pool already started.");
}

void pool::unload() {
  std::lock_guard<std::mutex> lck(_init_m);
  if (_instance) {
    delete _instance;
    _instance = nullptr;
  }
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
 * @brief Constructor. Private, it is called through the static
 * instance() method. While this object gathers statistics for the statistics
 * engine, it is not initialized as others. This is because, the stats engine
 * is heavily dependent on the pool. So the stats engine needs the pool and the
 * pool needs the stats engine.
 *
 * The idea here, is that when the pool is started, no stats are done. And when
 * the stats::center is well started, it asks the pool to start its stats.
 */
pool::pool(size_t size)
    : _stats(nullptr),
      _pool_size{size == 0 ? std::max(std::thread::hardware_concurrency(), 3u)
                           : size},
      _io_context(_pool_size),
      _worker(new asio::io_context::work(_io_context)),
      _closed(true),
      _timer(_io_context),
      _stats_running{false} {
  std::lock_guard<std::mutex> lock(_closed_m);
  if (_closed) {
    log_v2::core()->info("Starting the TCP thread pool of {} threads",
                         _pool_size);
    for (uint32_t i = 0; i < _pool_size; ++i)
      _pool.emplace_back([this] { _io_context.run(); });
    _closed = false;
  }
}

/**
 * @brief Start the stats of the pool. This method is called by the stats engine
 * when it is ready.
 *
 * @param stats The pointer used by the pool to set its data in the stats
 * engine.
 */
void pool::start_stats(ThreadPool* stats) {
  _stats = stats;
  /* The only time, we set a data directly to stats, this is because this method
   * is called by the stats engine and the _check_latency has not started yet */
  _stats->set_size(_pool_size);
  _stats_running = true;
  _timer.expires_after(std::chrono::seconds(10));
  _timer.async_wait(
      std::bind(&pool::_check_latency, this, std::placeholders::_1));
}

void pool::stop_stats() {
  if (_stats_running) {
    std::promise<bool> p;
    std::future<bool> f(p.get_future());
    asio::post(_timer.get_executor(), [this, &p] {
      _stats_running = false;
      _timer.cancel();
      p.set_value(true);
    });
    f.get();
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
      stats::center::instance().update(
          _stats->mutable_latency(), fmt::format("{:.3f}ms", duration.count()));
      log_v2::core()->trace("Thread pool latency {:.3f}ms", duration.count());
    });
    if (_stats_running) {
      _timer.expires_after(std::chrono::seconds(10));
      _timer.async_wait(
          std::bind(&pool::_check_latency, this, std::placeholders::_1));
    }
  }
}
