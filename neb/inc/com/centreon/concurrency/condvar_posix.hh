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

#ifndef CC_CONCURRENCY_CONDVAR_POSIX_HH
#  define CC_CONCURRENCY_CONDVAR_POSIX_HH

#  include <climits>
#  include <pthread.h>
#  include "com/centreon/namespace.hh"
#  include "com/centreon/concurrency/mutex_posix.hh"

CC_BEGIN()

namespace          concurrency {
  /**
   *  @class condvar condvar_posix.hh "com/centreon/concurrency/condvar.hh"
   *  @brief Allow simple threads synchronization.
   *
   *  Provide condition variable for synchronization between threads.
   */
  class            condvar {
  public:
                   condvar();
                   ~condvar() throw ();
    void           wait(mutex* mtx);
    bool           wait(mutex* mtx, unsigned long timeout);
    void           wake_all();
    void           wake_one();

  private:
                   condvar(condvar const& right);
    condvar&       operator=(condvar const& right);

    pthread_cond_t _cnd;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_CONDVAR_POSIX_HH
