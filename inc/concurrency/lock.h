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

#ifndef CONCURRENCY_LOCK_H_
# define CONCURRENCY_LOCK_H_

namespace  Concurrency
{
  // Forward declaration.
  class    Mutex;

  /**
   *  \class Lock lock.h "concurrency/lock.h"
   *  \brief Handle locking/unlocking of a mutex.
   *
   *  To avoid forgetting about releasing a locked mutex, the Lock will handle
   *  these operations on its creation and its destruction.
   *
   *  \see Mutex
   */
  class    Lock
  {
   private:
    bool   locked_;
    Mutex& mutex_;
           Lock(const Lock& lock);
    Lock&  operator=(const Lock& lock);

   public:
           Lock(Mutex& mutex);
           ~Lock();
    void   Release();
  };
}

#endif /* !CONCURRENCY_LOCK_H_ */
