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
#include <pthread.h>
#include <stdlib.h>                         // for abort
#include <string.h>                         // for strerror
#include "concurrency/condition_variable.h"
#include "concurrency/mutex.h"
#include "exception.h"

using namespace Concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief ConditionVariable copy constructor.
 *
 *  Condition variables are not copyable and therefore any attempt to use the
 *  copy constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] cv Unused.
 */
ConditionVariable::ConditionVariable(const ConditionVariable& cv)
{
  (void)cv;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Condition variables are not copyable and therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] cv Unused.
 *
 *  \return *this
 */
ConditionVariable& ConditionVariable::operator=(const ConditionVariable& cv)
{
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
 *  ConditionVariable default constructor.
 *
 *  \throw Exception Condition variable initialization failed.
 */
ConditionVariable::ConditionVariable()
{
  int ret;

  ret = pthread_cond_init(&this->cv_, NULL);
  if (ret)
    throw (Exception(ret, strerror(ret)));
}

/**
 *  ConditionVariable destructor.
 */
ConditionVariable::~ConditionVariable()
{
  pthread_cond_destroy(&this->cv_);
}

/**
 *  \brief Sleep until a specified condition occurs.
 *
 *  The mutex will be unlocked and the current thread will be paused until some
 *  other thread calls Wake() or WakeAll(). At that time, the mutex will be
 *  locked again prior to running the thread again.
 *
 *  \param[in,out] mutex Mutex that should be locked when calling Sleep().
 */
void ConditionVariable::Sleep(Mutex& mutex)
{
  int ret;

  ret = pthread_cond_wait(&this->cv_, &mutex.mutex_);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}

/**
 *  Resume at least one waiting thread.
 */
void ConditionVariable::Wake()
{
  int ret;

  ret = pthread_cond_signal(&this->cv_);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}

/**
 *  Resume all waiting threads.
 */
void ConditionVariable::WakeAll()
{
  int ret;

  ret = pthread_cond_broadcast(&this->cv_);
  if (ret)
    throw (Exception(ret, strerror(ret)));
  return ;
}
