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

#ifndef CONCURRENCY_THREAD_H_
# define CONCURRENCY_THREAD_H_

# include <stddef.h>    // for NULL
# include <sys/types.h> // for pthread_t

namespace           Concurrency
{
  // Forward declaration.
  class             ThreadListener;

  /**
   *  \class Thread thread.h "concurrency/thread.h"
   *  \brief Thread class.
   *
   *  This class represents a thread. It should be subclassed and the
   *  operator()() should be overloaded.
   */
  class             Thread
  {
   private:
    bool            joinable_;
    pthread_t       thread_;
                    Thread(const Thread& thread);
    Thread&         operator=(const Thread& thread);

   protected:
    volatile bool   should_exit;
    ThreadListener* listener;

   public:
                    Thread();
    virtual         ~Thread();
    virtual void    operator()() = 0;
    void            Cancel();
    void            Detach();
    virtual void    Exit();
    void            Join();
    void            Run(ThreadListener* tl = NULL);
  };
}

#endif /* !CONCURRENCY_THREAD_H_ */
