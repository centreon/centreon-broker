/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <errno.h>             // for EBUSY
#include <pthread.h>
#include <stdlib.h>            // for abort
#include <string.h>            // for strerror
#include "concurrency/mutex.h"
#include "exception.h"

using namespace Concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Mutex copy constructor.
 *
 *  Copying a mutex makes no sense. Therefore any attempt to use this copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] mutex Unused.
 */
Mutex::Mutex(const Mutex& mutex)
{
  (void)mutex;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Copying a mutex makes no sense. Therefore any attempt to use this
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] mutex Unused.
 *
 *  \return *this
 */
Mutex& Mutex::operator=(const Mutex& mutex)
{
  (void)mutex;
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
 *  Mutex default constructor.
 *
 *  \throw Exception Mutex initialization failed.
 */
Mutex::Mutex()
{
  int ret;

  ret = pthread_mutex_init(&this->mutex_, NULL);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}

/**
 *  Mutex destructor.
 *  \par Safety No throw guarantee.
 */
Mutex::~Mutex()
{
  pthread_mutex_destroy(&this->mutex_);
}

/**
 *  \brief Lock a mutex.
 *
 *  Only one thread can hold the lock at a time. Also the thread that locked
 *  the mutex is the only one allowed to release it. The mutex should be
 *  unlocked as soon as possible to avoid as much lock contention as possible.
 *  \par Safety Basic exception safety.
 *
 *  \throw Exception Mutex locking failed.
 */
void Mutex::Lock()
{
  int ret;

  ret = pthread_mutex_lock(&this->mutex_);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}

/**
 *  \brief Try to lock a mutex.
 *
 *  This method has the same effect as Lock() except if the mutex is already
 *  in which case it will return immediately.
 *  \par Safety Basic exception safety.
 *
 *  \return true if the mutex's ownership has been acquired, false otherwise.
 *
 *  \throw Exception An unexpected error occurred.
 */
bool Mutex::TryLock()
{
  int ret;

  ret = pthread_mutex_trylock(&this->mutex_);
  if (ret && ret != EBUSY) // EBUSY indicates that the mutex is already locked.
    throw (Exception(ret, strerror(ret)));
  return (ret != EBUSY);
}

/**
 *  \brief Unlock a mutex.
 *
 *  When a thread is done manipulating a shared variable, it shall unlock the
 *  associated mutex. Only the thread that owns the mutex (the one which locked
 *  it) can unlock the mutex.
 *  \par Safety Basic exception safety.
 */
void Mutex::Unlock()
{
  int ret;

  ret = pthread_mutex_unlock(&this->mutex_);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}
