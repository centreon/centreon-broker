/*
** Copyright 2012-2013 Merethis
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

#ifndef CC_CONCURRENCY_WRITE_LOCKER_HH
#  define CC_CONCURRENCY_WRITE_LOCKER_HH

#  include "com/centreon/concurrency/read_write_lock.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace            concurrency {
  /**
   *  @class write_locker write_locker.hh "com/centreon/concurrency/write_locker.hh"
   *  @brief Handle write locking of a readers-writer lock.
   *
   *  Lock RWL upon creation and unlock it upon destruction.
   */
  class              write_locker {
  public:
    /**
     *  Constructor.
     */
                     write_locker(read_write_lock* rwl)
      : _locked(false), _rwl(rwl) {
      relock();
    }

    /**
     *  Destructor.
     */
                     ~write_locker() throw () {
      try {
        if (_locked)
          unlock();
      }
      catch (...) {}
    }

    /**
     *  Relock.
     */
    void             relock() {
      _rwl->write_lock();
      _locked = true;
      return ;
    }

    /**
     *  Unlock.
     */
    void             unlock() {
      _rwl->write_unlock();
      _locked = false;
      return ;
    }

  private:
                     write_locker(write_locker const& right);
    write_locker&    operator=(write_locker const& right);

    bool             _locked;
    read_write_lock* _rwl;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_WRITE_LOCKER_HH
