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

#ifndef CC_CONCURRENCY_MUTEX_WIN32_HH
#  define CC_CONCURRENCY_MUTEX_WIN32_HH

#  include <windows.h>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace            concurrency {
  /**
   *  @class mutex mutex_win32.hh "com/centreon/concurrency/mutex.hh"
   *  @brief Implements a mutex.
   *
   *  Win32-base implementation of a mutex (intra-process
   *  synchronisation). On Windows the mutex class is a critical
   *  section and should not be confused with a Win32 mutex which is
   *  used for inter-process synchronisation.
   */
  class              mutex {
    friend class     condvar;

  public:
                     mutex();
                     ~mutex() throw ();
    void             lock();
    bool             trylock();
    void             unlock();

  private:
                     mutex(mutex const& right);
    mutex&           operator=(mutex const& right);

    CRITICAL_SECTION _csection;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_MUTEX_WIN32_HH
