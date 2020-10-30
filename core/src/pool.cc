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
#include <asio.hpp>
#include "com/centreon/broker/pool.hh"

#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;

size_t pool::_pool_size(0);

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
pool::pool() : _io_context(_pool_size), _worker(_io_context), _closed(true), _timer(_io_context) {
  _start();
  check_latency();
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
    for (uint32_t i = 0; i < count; i++)
      _pool.emplace_back([this] { _io_context.run(); });
  }
}

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
    _io_context.stop();
    for (auto& t : _pool)
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

void pool::check_latency() {
  std::chrono::time_point<std::chrono::system_clock> start =
    std::chrono::system_clock::now();
  asio::post(_io_context, [start, this] {
    std::chrono::time_point<std::chrono::system_clock> end =
      std::chrono::system_clock::now();
    _latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    log_v2::core()->trace("Thread pool latency at {}ms", _latency);
  });
  _timer.expires_after(std::chrono::seconds(30));
  _timer.async_wait(std::bind(&pool::check_latency, this));
}
