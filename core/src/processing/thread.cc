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

#include <cassert>
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker::processing;

/**
 *  Default constructor.
 */
bthread::bthread(std::string const& name)
    : stat_visitable{name},
      _should_exit{false},
      _started{false},
      _stopped{false} {}

/**
 *  Destructor.
 */
bthread::~bthread() {
  // We must call exit before this destructor
  assert(!_started);
}

/**
 *  Notify bthread to exit.
 */
void bthread::exit() {
  std::unique_lock<std::mutex> lock(_stopped_m);
  if (_started) {
    if (!_should_exit) {
      _should_exit = true;
      _stopped_cv.wait(lock, [this] { return _stopped; });
      _thread.join();
      _started = false;
    }
  }
}

/**
 *  Check if bthread should exit.
 *
 *  @return True if bthread should exit.
 */
bool bthread::should_exit() const {
  return _should_exit;
}

/**
 *  Start bthread.
 */
void bthread::start() {
  std::unique_lock<std::mutex> lock(_started_m);
  _stopped = false;
  if (!_started) {
    _should_exit = false;
    _thread = std::thread(&bthread::_callback, this);
    _started_cv.wait(lock, [this] { return _started; });
  }
}

/**
 *  Notify bthread of a configuration update request.
 */
void bthread::update() {
  // Do nothing.
}

void bthread::_callback() {
  std::unique_lock<std::mutex> lock_start(_started_m);
  _started = true;
  _started_cv.notify_all();
  lock_start.unlock();

  run();

  std::unique_lock<std::mutex> lock_stop(_stopped_m);
  _stopped = true;
  _stopped_cv.notify_all();
}

bool bthread::is_running() const {
  std::lock_guard<std::mutex> lock(_started_m);
  return _started;
}
