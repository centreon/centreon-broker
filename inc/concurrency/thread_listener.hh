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

#ifndef CONCURRENCY_THREAD_LISTENER_HH_
# define CONCURRENCY_THREAD_LISTENER_HH_

namespace            concurrency {
  // Forward declaration.
  class              thread;

  /**
   *  @class thread_listener thread_listener.hh "concurrency/thread_listener.hh"
   *  @brief Listen thread events.
   *
   *  Upon thread creation, it is possible to specify a listener which
   *  will receive events from the thread.
   *
   *  @see thread
   */
  class              thread_listener {
   protected:
                     thread_listener();
                     thread_listener(thread_listener const& tl);
    thread_listener& operator=(thread_listener const& tl);

   public:
    virtual          ~thread_listener();
    virtual void     on_create(thread* t);
    virtual void     on_exit(thread* t);
    virtual void     on_failure(thread* t);
  };
}

#endif /* !CONCURRENCY_THREAD_LISTENER_HH_ */
