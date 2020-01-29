/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#ifndef CCB_STATS_WORKER_HH
#define CCB_STATS_WORKER_HH

#include <atomic>
#include <string>
#include <thread>

namespace com {
namespace centreon {
namespace broker {
namespace stats {
/**
 *  @class worker worker.hh "com/centreon/broker/stats/worker.hh"
 *  @brief Statistics worker thread.
 *
 *  The worker thread will wait for readability on a FIFO file
 *  and write to it statistics when available.
 */
class worker {
  std::string _buffer;
  int _fd;
  std::string _fifo;

  std::thread _thread;
  std::atomic_bool _exit;

  void _close();
  bool _open();
  void _run();

 public:
  worker();
  ~worker() noexcept;
  worker(worker const& right) = delete;
  worker& operator=(worker const& right) = delete;

  void run(std::string const& fifo_file);
};
}  // namespace stats
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_STATS_WORKER_HH
