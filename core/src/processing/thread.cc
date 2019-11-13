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

#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker::processing;

/**
 *  Default constructor.
 */
bthread::bthread(std::string const& name)
    : stat_visitable{name}, _should_exit{false}, _started{false} {}

/**
 *  Destructor.
 */
bthread::~bthread() {}

/**
 *  Notify bthread to exit.
 */
void bthread::exit() {
  if (is_running()) {
    bool to_join = false;
    {
      std::lock_guard<std::mutex> lock(_should_exitm);
      _should_exit = true;
      if (_started) {
        _started = false;
        to_join = true;
      }
    }
    if (to_join) {
      _cv.notify_all();
      _thread.join();
    }
  }
}

/**
 *  Check if bthread should exit.
 *
 *  @return True if bthread should exit.
 */
bool bthread::should_exit() const {
  std::lock_guard<std::mutex> lock(_should_exitm);
  return _should_exit;
}

/**
 *  Start bthread.
 */
void bthread::start() {
  {
    std::lock_guard<std::mutex> lock(_should_exitm);
    if (_started)
      return;
    _should_exit = false;
    _started = true;
  }
  _thread = std::thread(&bthread::_callback, this);
}

/**
 *  Notify bthread of a configuration update request.
 */
void bthread::update() {
  // Do nothing.
}

/**
 *  Wait for bthread termination. The idea is to add a timeout to the join
 *  function. If the main loop is over, then join() is called, otherwise,
 *  if the timeout is reached, the main loop continues to run and no join()
 *  is called.
 *
 *  @param[in] timeout_ms  Maximum wait time in ms.
 *
 *  @return True if bthread exited in less than timeout_ms.
 */
bool bthread::wait(unsigned long timeout_ms) {
  if (timeout_ms == ULONG_MAX && _thread.joinable()) {
    _thread.join();
    return true;
  }

  if (!_thread.joinable())
    return true;

  struct timespec tt {
    .tv_sec = static_cast<long int>(timeout_ms / 1000),
    .tv_nsec = static_cast<long int>(timeout_ms % 1000) * 1000000
  };
  int res = pthread_timedjoin_np(_thread.native_handle(), nullptr, &tt);
  if (res == ETIMEDOUT)
    return false;

  return true;
}

void bthread::_callback() {
  run();

  // Let's wait for the exit() method.
  std::unique_lock<std::mutex> lock(_cv_m);
  _cv.wait(lock, [this] { return !is_running(); });
}

bool bthread::is_running() const {
  std::lock_guard<std::mutex> lk(_should_exitm);
  return _started;
}
