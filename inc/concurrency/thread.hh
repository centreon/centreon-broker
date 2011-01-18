/*
** Copyright 2009-2011 MERETHIS
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

#ifndef CONCURRENCY_THREAD_HH_
# define CONCURRENCY_THREAD_HH_

# include <stddef.h>
# include <sys/types.h>

namespace            concurrency {
  // Forward declaration.
  class              thread_listener;

  /**
   *  @class thread thread.hh "concurrency/thread.hh"
   *  @brief Thread class.
   *
   *  This class represents a thread. It should be subclassed and the
   *  operator()() should be overloaded.
   */
  class              thread {
   private:
    bool             _joinable;
    pthread_t        _thread;
                     thread(thread const& t);
    thread&          operator=(thread const& t);

   protected:
    thread_listener* _listener;
    volatile bool    _should_exit;

   public:
                     thread();
    virtual          ~thread();
    virtual void     operator()() = 0;
    void             cancel();
    void             detach();
    virtual void     exit();
    void             join();
    void             run(thread_listener* tl = NULL);
  };
}

#endif /* !CONCURRENCY_THREAD_HH_ */
