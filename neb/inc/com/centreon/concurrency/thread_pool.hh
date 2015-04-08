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

#ifndef CC_CONCURRENCY_THREAD_POOL_HH
#  define CC_CONCURRENCY_THREAD_POOL_HH

#  include <list>
#  ifndef _WIN32
#    include <sys/types.h>
#  endif // POSIX
#  include "com/centreon/namespace.hh"
#  include "com/centreon/concurrency/condvar.hh"
#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/concurrency/runnable.hh"
#  include "com/centreon/concurrency/thread.hh"

CC_BEGIN()

namespace                concurrency {
  /**
   *  @class thread_pool thread_pool.hh "com/centreon/concurrency/thread_pool.hh"
   *  @brief Allow to manage a collection of threads.
   *
   *  Allow to manage a collection of thread and reduce cost of lunching
   *  thread by recycle threads.
   */
  class                  thread_pool {
  public:
                         thread_pool(unsigned int max_thread_count = 0);
                         ~thread_pool() throw ();
    unsigned int         get_current_task_running() const throw ();
    unsigned int         get_max_thread_count() const throw ();
    void                 set_max_thread_count(unsigned int max);
    void                 start(runnable* r);
    void                 wait_for_done();

  private:
    class                internal_thread : public thread {
    public:
                         internal_thread(thread_pool* th_pool);
                         ~internal_thread() throw ();
      void               quit();

    private:
                         internal_thread(internal_thread const& right);
      internal_thread&   operator=(internal_thread const& right);
      internal_thread&   _internal_copy(internal_thread const& right);
      void               _run();

      bool               _quit;
      thread_pool*       _th_pool;
    };

                         thread_pool(thread_pool const& right);
    thread_pool&         operator=(thread_pool const& right);

    condvar              _cnd_pool;
    condvar              _cnd_thread;
    unsigned int         _current_task_running;
    unsigned int         _max_thread_count;
    mutable mutex        _mtx_pool;
    mutable mutex        _mtx_thread;
#ifndef _WIN32
    pid_t                _pid;
#endif // POSIX
    std::list<internal_thread*>
                         _pool;
    bool                 _quit;
    std::list<runnable*> _tasks;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_THREAD_POOL_HH
