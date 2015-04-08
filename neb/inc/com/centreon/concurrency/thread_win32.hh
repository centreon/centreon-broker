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

#ifndef CC_CONCURRENCY_THREAD_WIN32_HH
#  define CC_CONCURRENCY_THREAD_WIN32_HH

#  include <windows.h>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace            concurrency {
  typedef HANDLE thread_id;

  /**
   *  @class thread thread_win32.hh "com/centreon/concurrency/thread.hh"
   *  @brief Win32 thread wrapper.
   *
   *  Wrap Win32 threads in a nice and easy to use class.
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
                     thread(thread const& t);
    thread&          operator=(thread const& t);
    void             _close() throw ();
    static DWORD     _helper(void* data);

    HANDLE           _th;
  };
}

CC_END()

#endif // !CC_CONCURRENCY_THREAD_WIN32_HH
