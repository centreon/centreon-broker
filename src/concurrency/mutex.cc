/*
** Copyright 2009-2010 MERETHIS
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
#include "concurrency/mutex.hh"
#include "exceptions/retval.hh"

using namespace concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Mutex copy constructor.
 *
 *  Copying a mutex makes no sense. Therefore any attempt to use this copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] m Unused.
 */
mutex::mutex(mutex const& m) {
  (void)m;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator overload.
 *
 *  Copying a mutex makes no sense. Therefore any attempt to use this
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] m Unused.
 *
 *  @return This instance.
 */
mutex& mutex::operator=(mutex const& m) {
  (void)m;
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
mutex::mutex() {
  int ret(pthread_mutex_init(&_mutex, NULL));
  if (ret)
    throw (exceptions::retval(ret) << "mutex ctor: " << strerror(ret));
  return ;
}

/**
 *  Destructor.
 */
mutex::~mutex() {
  int ret(pthread_mutex_destroy(&_mutex));
  if (ret)
    std::cerr << "mutex dtor: " << strerror(ret) << std::endl;
}

/**
 *  @brief Lock a mutex.
 *
 *  Only one thread can hold the lock at a time. Also the thread that
 *  locked the mutex is the only one allowed to release it. The mutex
 *  should be unlocked as soon as possible to avoid as much lock
 *  contention as possible.
 */
void mutex::lock() {
  int ret(pthread_mutex_lock(&_mutex));
  if (ret)
    throw (exceptions::retval(ret) << "mutex::lock: " << strerror(ret));
  return ;
}

/**
 *  @brief Try to lock a mutex.
 *
 *  This method has the same effect as lock() except if the mutex is
 *  already in which case it will return immediately.
 *
 *  @return true if the mutex's ownership has been acquired, false
 *          otherwise.
 */
bool mutex::try_lock() {
  int ret(pthread_mutex_trylock(&_mutex));
  if (ret && ret != EBUSY) // EBUSY if mutex is already locked.
    throw (exceptions::retval(ret) << "mutex::try_lock: "
                                   << strerror(ret));
  return (ret != EBUSY);
}

/**
 *  @brief Unlock a mutex.
 *
 *  When a thread is done manipulating a shared variable, it shall
 *  unlock the associated mutex. Only the thread that owns the mutex
 *  (the one which locked it) can unlock the mutex.
 */
void mutex::unlock() {
  int ret(pthread_mutex_unlock(&_mutex));
  if (ret)
    throw (exceptions::retval(ret) << "mutex::unlock: "
                                   << strerror(ret));
  return ;
}
