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

#include <QMutexLocker>
#include "com/centreon/broker/io/properties.hh"
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker::processing;

/**
 *  Default constructor.
 */
thread::thread(std::string const& name)
  : stat_visitable(name), _should_exit(false) {}

/**
 *  Destructor.
 */
thread::~thread() {}

/**
 *  Notify thread to exit.
 */
void thread::exit() {
  QMutexLocker lock(&_should_exitm);
  _should_exit = true;
  return ;
}

/**
 *  Check if thread should exit.
 *
 *  @return True if thread should exit.
 */
bool thread::should_exit() const {
  QMutexLocker lock(&_should_exitm);
  return (_should_exit);
}

/**
 *  Start thread.
 */
void thread::start() {
  {
    QMutexLocker lock(&_should_exitm);
    _should_exit = false;
  }
  QThread::start();
  return ;
}

/**
 *  Notify thread of a configuration update request.
 */
void thread::update() {
  // Do nothing.
  return ;
}

/**
 *  Wait for thread termination.
 *
 *  @param[in] timeout_ms  Maximum wait time in ms.
 *
 *  @return True if thread exited in less than timeout_ms.
 */
bool thread::wait(unsigned long timeout_ms) {
  bool retval(QThread::wait(timeout_ms));
  if (retval) {
    QMutexLocker lock(&_should_exitm);
    _should_exit = false;
  }
  return (retval);
}

