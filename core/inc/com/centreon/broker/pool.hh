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
#ifndef CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_
#define CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_

#include <asio.hpp>
#include <cstdlib>

#include "broker.pb.h"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 * @brief The Broker's thread pool.
 *
 * At the origin, this thread pool is configured to be used by ASIO. Each thread
 * in this pool runs an io_context that allows any part in Broker to post
 * a work to be done.
 *
 * This pool may look a little complicated. We can see inside it several
 * attributes, let's make a quick tour of them. A thread pool is an array of
 * threads. This array, here, is a std::vector named _pool.
 *
 * This pool is instanciated through a unique instance. So its constructor is
 * private and we have a static method named instance() to get it.
 *
 * Initially, the pool is not started. To start it, there is a static method
 * named start() that takes one argument: the number of threads.
 *
 * If the start() method is not called before using the pool, broker does not
 * work, at least not as expected. If the given size is 0, its size will be
 * initialized with the number of cpus on the host computer.
 *
 * To post tasks to the pool, we use the ASIO api, for that we need an
 * asio::io_context and an asio::io_service::work which are defined when then
 * pool is constructed.
 *
 * There is a _closed boolean variable used internally to know if the pool is
 * running (and not closed) or stopped (and closed). To work with it, we also
 * use a mutex _closed_m.
 *
 * And to have statistics on that pool, every 10s, we execute a task
 * _check_latency() whose goal is to measure the duration between the time point
 * when we ask to execute a task and the time point when we get its execution.
 * This duration is stored in _latency.
 *
 * We can see a steady_timer in the class, its goal is to cadence this check.
 */
class pool {
  static pool* _instance;
  static std::mutex _init_m;

  double _last_check_latency;
  ThreadPool* _stats;
  const size_t _pool_size;

  asio::io_context _io_context;
  std::unique_ptr<asio::io_context::work> _worker;
  std::vector<std::thread> _pool;
  bool _closed;
  mutable std::mutex _closed_m;

  asio::steady_timer _timer;
  std::atomic_bool _stats_running;

  pool(size_t size);
  ~pool() noexcept;
  void _stop();
  void _check_latency(asio::error_code ec);

 public:
  pool(const pool&) = delete;
  pool& operator=(const pool&) = delete;

  static void load(size_t size);
  static void unload();
  static pool& instance();
  static asio::io_context& io_context();

  void start_stats(ThreadPool* stats);
  void stop_stats();

  /**
   * @brief Returns the number of threads used in the pool.
   *
   * @return a size.
   */
  size_t get_pool_size() const { return _pool_size; }

  // used for tests
  double get_last_check_latency();
};

CCB_END()

#endif  // CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_
