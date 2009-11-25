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

#ifndef CONCURRENCY_MUTEX_H_
# define CONCURRENCY_MUTEX_H_

# include <sys/types.h> // for pthread_mutex_t

namespace           Concurrency
{
  /**
   *  \class Mutex mutex.h "concurrency/mutex.h"
   *  \brief Mutex class.
   *
   *  A mutex (mutual exclusion) is a primitive for thread synchronisation.
   *  Only one thread can lock a mutex at a time. This is useful when two
   *  threads access the same object, which can cause race conditions for
   *  example.
   *
   *  \see Lock
   *  \see Thread
   */
  class             Mutex
  {
   private:
    pthread_mutex_t mutex_;
    void            InitMutex();

   public:
                    Mutex();
                    Mutex(const Mutex& mutex);
                    ~Mutex();
    Mutex&          operator=(const Mutex& mutex);
    void            Lock();
    bool            TryLock();
    void            Unlock();
  };
}

#endif /* !CONCURRENCY_MUTEX_H_ */
