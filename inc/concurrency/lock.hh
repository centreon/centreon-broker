/*
** Copyright 2009-2010 MERETHIS
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

#ifndef CONCURRENCY_LOCK_HH_
# define CONCURRENCY_LOCK_HH_

namespace  concurrency {
  // Forward declaration.
  class    mutex;

  /**
   *  @class lock lock.hh "concurrency/lock.hh"
   *  @brief Handle locking/unlocking of a mutex.
   *
   *  To avoid forgetting about releasing a locked mutex, the lock will
   *  handle these operations on its creation and its destruction.
   *
   *  @see mutex
   */
  class    lock {
   private:
    bool   _locked;
    mutex& _mutex;
           lock(lock const& l);
    lock&  operator=(lock const& l);

   public:
           lock(mutex& m);
           ~lock();
    void   release();
  };
}

#endif /* !CONCURRENCY_LOCK_HH_ */
