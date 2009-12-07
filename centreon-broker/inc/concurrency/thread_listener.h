/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CONCURRENCY_THREAD_LISTENER_H_
# define CONCURRENCY_THREAD_LISTENER_H_

namespace           Concurrency
{
  // Forward declaration.
  class             Thread;

  /**
   *  \class ThreadListener thread_listener.h "concurrency/thread_listener.h"
   *  \brief Listen thread events.
   *
   *  Upon thread creation, it is possible to specify a listener which will
   *  receive events from the thread.
   */
  class             ThreadListener
  {
   protected:
                    ThreadListener();
                    ThreadListener(const ThreadListener& tl);
    ThreadListener& operator=(const ThreadListener& tl);

   public:
    virtual         ~ThreadListener();
    virtual void    OnCreate(Thread* thread);
    virtual void    OnExit(Thread* thread);
  };
}

#endif /* !CONCURRENCY_THREAD_LISTENER_H_ */
