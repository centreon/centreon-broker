/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "concurrency/condition_variable.hh"
#include "concurrency/mutex.hh"
#include "exceptions/retval.hh"

using namespace concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Condition variables are not copyable and therefore any attempt to
 *  use the copy constructor will result in a call to abort().
 *
 *  @param[in] cv Unused.
 */
condition_variable::condition_variable(condition_variable const& cv) {
  (void)cv;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Condition variables are not copyable and therefore any attempt to
 *  use the assignment operator will result in a call to abort().
 *
 *  @param[in] cv Unused.
 *
 *  @return This object.
 */
condition_variable& condition_variable::operator=(condition_variable const& cv) {
  (void)cv;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
condition_variable::condition_variable() {
  int ret(pthread_cond_init(&_cv, NULL));
  if (ret)
    throw (exceptions::retval(ret) << "cvar ctor: " << strerror(ret));
}

/**
 *  Destructor.
 */
condition_variable::~condition_variable() {
  int ret(pthread_cond_destroy(&_cv));
  if (ret)
    std::cerr << "cvar dtor: " << strerror(ret) << std::endl;
}

/**
 *  @brief Sleep until a specified condition occurs.
 *
 *  The mutex will be unlocked and the current thread will be paused
 *  until some other thread calls wake() or wake_all(). At that time,
 *  the mutex will be locked again prior to running the thread again.
 *
 *  @param[in,out] m Mutex that should be locked when calling sleep().
 */
void condition_variable::sleep(mutex& m) {
  int ret(pthread_cond_wait(&_cv, &m._mutex));
  if (ret)
    throw (exceptions::retval(ret) << "cvar::sleep: " << strerror(ret));
  return ;
}

/**
 *  @brief Sleep until a specified condition occurs or the deadline is
 *         exceeded.
 *
 *  The mutex will be unlocked and the current thread will be paused
 *  until some other thread calls Wake() or WakeAll() or the deadline is
 *  exceeded. At that time, the mutex will be locked again prior to
 *  running the thread again.
 *
 *  @param[in,out] m        Mutex that should be locked when calling
 *                          sleep().
 *  @param[in]     deadline Time that shouldn't be exceeded while
 *                          sleeping.
 *
 *  @return true if the timeout occured, false otherwise.
 */
bool condition_variable::sleep(mutex& m, time_t deadline) {
  // Set deadline.
  struct timespec ts;
  memset(&ts, 0, sizeof(ts));
  ts.tv_sec = deadline;

  // Call pthread function.
  int ret(pthread_cond_timedwait(&_cv, &m._mutex, &ts));
  if (ret && (ret != ETIMEDOUT))
    throw (exceptions::retval(ret) << "cvar::sleep: " << strerror(ret));
  return (ETIMEDOUT == ret);
}

/**
 *  Resume at least one waiting thread.
 */
void condition_variable::wake() {
  int ret(pthread_cond_signal(&_cv));
  if (ret)
    throw (exceptions::retval(ret) << "cvar::wake: " << strerror(ret));
  return ;
}

/**
 *  Resume all waiting threads.
 */
void condition_variable::wake_all() {
  int ret(pthread_cond_broadcast(&_cv));
  if (ret)
    throw (exceptions::retval(ret) << "cvar::wake_all: "
                                   << strerror(ret));
  return ;
}
