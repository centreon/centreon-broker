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

#ifndef CC_CONCURRENCY_READ_LOCKER_HH
#  define CC_CONCURRENCY_READ_LOCKER_HH

#  include "com/centreon/concurrency/read_write_lock.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace            concurrency {
  /**
   *  @class read_locker read_locker.hh "com/centreon/concurrency/read_locker.hh"
   *  @brief Handle read locking of a readers-writer lock.
   *
   *  Lock RWL upon creation and unlock it upon destruction.
   */
  class              read_locker {
  public:
    /**
     *  Constructor.
     *
     *  @param[in] rwl Target RWL.
     */
                     read_locker(read_write_lock* rwl)
      : _locked(false), _rwl(rwl) {
      relock();
    }

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
                     read_locker(read_locker const& right)
      : _locked(false), _rwl(right._rwl) {
      relock();
    }

    /**
     *  Destructor.
     */
                     ~read_locker() throw () {
      try {
        if (_locked)
          unlock();
      }
      catch (...) {}
    }

    /**
     *  Assignment operator.
     *
     *  @param[in] right Object to copy.
     *
     *  @return This object.
     */
    read_locker&     operator=(read_locker const& right) {
      if (this != &right) {
        if (_locked)
          unlock();
        _rwl = right._rwl;
        relock();
      }
      return (*this);
    }

    /**
     *  Relock.
     */
    void             relock() {
      _rwl->read_lock();
      _locked = true;
      return ;
    }

    /**
     *  Unlock.
     */
    void             unlock() {
      _rwl->read_unlock();
      _locked = false;
      return ;
    }

  private:
    bool             _locked;
    read_write_lock* _rwl;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_READ_LOCKER_HH
