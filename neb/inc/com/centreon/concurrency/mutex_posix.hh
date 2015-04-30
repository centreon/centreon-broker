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

#ifndef CC_CONCURRENCY_MUTEX_POSIX_HH
#  define CC_CONCURRENCY_MUTEX_POSIX_HH

#  include <cerrno>
#  include <cstdlib>
#  include <cstring>
#  include <pthread.h>
#  include "com/centreon/exceptions/basic.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace           concurrency {
  /**
   *  @class mutex mutex_posix.hh "com/centreon/concurrency/mutex.hh"
   *  @brief Implements a mutex.
   *
   *  POSIX-based implementation of a mutex.
   */
  class             mutex {
    friend class    condvar;
  public:
    /**
     *  Default constructor.
     */
                    mutex() {
      // Initialize mutex attributes.
      pthread_mutexattr_t mta;
      int ret = pthread_mutexattr_init(&mta);
      if (ret)
        throw (basic_error() << "could not initialize mutex "
               "attributes: " << strerror(ret));

      // Set mutex as recursive.
      ret = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
      if (ret)
        throw (basic_error() << "could not set mutex as recursive: "
               << strerror(ret));

      // Initialize mutex.
      ret = pthread_mutex_init(&_mtx, &mta);
      if (ret)
        throw (basic_error() << "could not initialize mutex: "
               << strerror(ret));
    }

    /**
     *  Destructor.
     */
                    ~mutex() throw () {
      pthread_mutex_destroy(&_mtx);
    }

    /**
     *  Lock the mutex and if another thread has already locked the
     *  mutex then this call will block until the mutex has unlock
     *  by the first thread.
     */
    void            lock() {
      int ret(pthread_mutex_lock(&_mtx));
      if (ret)
        throw (basic_error() << "failed to lock mutex : "
               << strerror(ret));
      return ;
    }

    /**
     *  Lock the mutex if the mutex is unlock and return without any
     *  modification on the mutex if anobther thread has already
     *  locked the mutex.
     *
     *  @return True if the mutex is lock, false if the mutex was
     *          already locked.
     */
    bool            trylock() {
      int ret(pthread_mutex_trylock(&_mtx));
      if (ret && (ret != EBUSY))
        throw (basic_error() << "failed mutex "
               << "lock attempt: " << strerror(ret));
      return (!ret);
    }

    /**
     *  Unlock the mutex.
     */
    void            unlock() {
      int ret(pthread_mutex_unlock(&_mtx));
      if (ret)
        throw (basic_error() << "failed to unlock mutex "
               << strerror(ret));
      return ;
    }


  private:
                    mutex(mutex const& right);
    mutex&          operator=(mutex const& right);

    pthread_mutex_t _mtx;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_MUTEX_POSIX_HH
