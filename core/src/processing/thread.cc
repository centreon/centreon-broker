/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <QMutexLocker>
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker::processing;

/**
 *  Default constructor.
 */
thread::thread() : _should_exit(false) {}

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
