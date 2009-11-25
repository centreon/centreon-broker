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
#include <stdlib.h>            // for abort
#include "concurrency/lock.h"
#include "concurrency/mutex.h"

using namespace Concurrency;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Lock copy constructor.
 *
 *  Lock is not copyable. Any attempt to use this constructor will result in a
 *  call to abort().
 *
 *  \param[in] lock Unused.
 */
Lock::Lock(const Lock& lock) : mutex_(lock.mutex_)
{
  (void)lock;
  assert(false);
  abort();
}

/**
 *  \brief Assignement operator overload.
 *
 *  Lock is not copyable. Any attempt to use this operator will result in a
 *  call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] lock Unused.
 *
 *  \return *this
 */
Lock& Lock::operator=(const Lock& lock)
{
  (void)lock;
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
 *  \brief Lock constructor.
 *
 *  Bind the lock with a mutex. During the construction, the Mutex object will
 *  be locked.
 *
 *  \param[in] mutex Mutex to bind with the lock.
 */
Lock::Lock(Mutex& mutex) : locked_(false), mutex_(mutex)
{
  this->mutex_.Lock();
  this->locked_ = true;
}

/**
 *  \brief Lock destructor.
 *
 *  If the Release() method has not already been called, the Mutex object will
 *  be unlocked.
 */
Lock::~Lock()
{
  this->Release();
}

/**
 *  \brief Release the associated mutex.
 *
 *  If the mutex should be released before the lock destruction, this method
 *  can be called directly.
 */
void Lock::Release()
{
  if (this->locked_)
    {
      this->mutex_.Unlock();
      this->locked_ = false;
    }
  return ;
}
