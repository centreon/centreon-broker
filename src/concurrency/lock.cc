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
#include <stdlib.h>
#include "concurrency/lock.hh"
#include "concurrency/mutex.hh"

using namespace concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  lock is not copyable. Any attempt to use this constructor will
 *  result in a call to abort().
 *
 *  @param[in] l Unused.
 */
lock::lock(lock const& l) : _mutex(l._mutex) {
  (void)l;
  assert(false);
  abort();
}

/**
 *  @brief Assignement operator.
 *
 *  lock is not copyable. Any attempt to use this operator will result
 *  in a call to abort().
 *
 *  @param[in] l Unused.
 *
 *  @return This object.
 */
lock& lock::operator=(lock const& l) {
  (void)l;
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
 *  @brief Constructor.
 *
 *  Bind the lock with a mutex. During the construction, the mutex
 *  object will be locked.
 *
 *  @param[in] m Mutex to bind with the lock.
 */
lock::lock(mutex& m) : _locked(false), _mutex(m) {
  _mutex.lock();
  _locked = true;
}

/**
 *  @brief Destructor.
 *
 *  If the release() method has not already been called, the mutex
 *  object will be unlocked.
 */
lock::~lock() {
  release();
}

/**
 *  @brief Release the associated mutex.
 *
 *  If the mutex should be released before the lock destruction, this
 *  method can be called directly.
 */
void lock::release() {
  if (_locked) {
    _mutex.unlock();
    _locked = false;
  }
  return ;
}
