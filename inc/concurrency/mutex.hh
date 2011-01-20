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

#ifndef CONCURRENCY_MUTEX_HH_
# define CONCURRENCY_MUTEX_HH_

# include <sys/types.h>

namespace           concurrency {
  /**
   *  @class mutex mutex.hh "concurrency/mutex.hh"
   *  @brief mutex class.
   *
   *  A mutex (mutual exclusion) is a primitive for thread
   *  synchronisation. Only one thread can lock a mutex at a time. This
   *  is useful when two threads access the same object, which can cause
   *  race conditions for example.
   *
   *  @see lock
   *  @see thread
   */
  class             mutex {
    // condition_variable needs to access the internal mutex object.
    friend class    condition_variable;

   private:
    pthread_mutex_t _mutex;
                    mutex(mutex const& m);
    mutex&          operator=(mutex const& m);

   public:
                    mutex();
                    ~mutex();
    void            lock();
    bool            try_lock();
    void            unlock();
  };
}

#endif /* !CONCURRENCY_MUTEX_HH_ */
