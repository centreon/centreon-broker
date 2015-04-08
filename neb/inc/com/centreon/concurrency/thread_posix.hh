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

#ifndef CC_CONCURRENCY_THREAD_POSIX_HH
#  define CC_CONCURRENCY_THREAD_POSIX_HH

#  include <pthread.h>
#  include "com/centreon/namespace.hh"
#  include "com/centreon/concurrency/mutex_posix.hh"

CC_BEGIN()

namespace            concurrency {
  typedef pthread_t thread_id;

  /**
   *  @class thread thread_posix.hh "com/centreon/concurrency/thread.hh"
   *  @brief POSIX thread wrapper.
   *
   *  Wrap POSIX thread library (pthreads) in a nice and easy to use
   *  class.
   */
  class              thread {
  public:
                     thread();
    virtual          ~thread() throw ();
    void             exec();
    static thread_id get_current_id() throw ();
    static void      msleep(unsigned long msecs);
    static void      nsleep(unsigned long nsecs);
    static void      sleep(unsigned long secs);
    static void      usleep(unsigned long usecs);
    void             wait();
    bool             wait(unsigned long timeout);
    static void      yield() throw ();

  protected:
    virtual void     _run() = 0;

  private:
                     thread(thread const& right);
    thread&          operator=(thread const& right);
    static void*     _execute(void* data);

    bool             _initialized;
    mutable mutex    _mtx;
    pthread_t        _th;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_THREAD_POSIX_HH
