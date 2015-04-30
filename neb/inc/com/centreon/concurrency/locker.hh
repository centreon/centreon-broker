/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_CONCURRENCY_LOCKER_HH
#  define CC_CONCURRENCY_LOCKER_HH

#  include "com/centreon/namespace.hh"
#  include "com/centreon/concurrency/mutex.hh"

CC_BEGIN()

namespace   concurrency {
  /**
   *  @class locker locker.hh "com/centreon/concurrency/locker.hh"
   *  @brief Provide a simple way to lock ans un lock mutex.
   *
   *  Allow simple method to lock and unlock mutex.
   */
  class     locker {
  public:
    /**
     *  Default constructor.
     *
     *  @param[in] m  The mutex to lock.
     */
            locker(mutex* m = NULL)
      : _is_lock(false),
        _m(m) {
      if (_m)
        relock();
    }

    /**
     *  Default destructor.
     */
            ~locker() throw () {
      if (_is_lock)
        unlock();
    }

    /**
     *  Get the mutex.
     *
     *  @return The internal mutex.
     */
    mutex*  get_mutex() const throw() {
      return (_m);
    }

    /**
     *  Lock the internal mutex.
     */
    void    relock() {
      if (_m) {
        _is_lock = true;
        _m->lock();
      }
    }

    /**
     *  Unlock the internal mutex.
     */
    void    unlock() {
      if (_m) {
        _m->unlock();
        _is_lock = false;
      }
    }

  private:
            locker(locker const& right);
    locker& operator=(locker const& right);

    bool   _is_lock;
    mutex* _m;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_LOCKER_HH
