/*
** Copyright 2015 Centreon
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

#ifndef CCB_PROCESSING_THREAD_HH
#define CCB_PROCESSING_THREAD_HH

#include <atomic>
#include <climits>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include "com/centreon/broker/processing/stat_visitable.hh"

namespace com {
namespace centreon {
namespace broker {
// Forward declaration.
namespace io {
class properties;
}

namespace processing {
/**
 *  @class bthread thread.hh "com/centreon/broker/processing/thread.hh"
 *  @brief Processing thread interface.
 *
 *  All processing threads respect this interface.
 *
 *  @see acceptor
 *  @see failover
 *  @see feeder
 */
class bthread : public stat_visitable {
 public:
  bthread(std::string const& name = std::string());
  bthread(bthread const& other) = delete;
  bthread& operator=(bthread const& other) = delete;
  virtual ~bthread();
  virtual void exit();
  bool should_exit() const;
  void start();
  virtual void update();
  virtual void run() {};
  bool is_running() const;

 private:
  std::atomic_bool _should_exit;

  std::thread _thread;
  bool _started;
  mutable std::mutex _started_m;
  std::condition_variable _started_cv;
  bool _stopped;
  mutable std::mutex _stopped_m;
  std::condition_variable _stopped_cv;

  void _callback();
};
}  // namespace processing
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_PROCESSING_THREAD_HH
