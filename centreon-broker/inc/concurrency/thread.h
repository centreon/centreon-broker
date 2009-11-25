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
# include <pthread.h>   // for pthread_create
# include <string.h>    // for strerror
# include "exception.h"

namespace     Concurrency
{
  /**
   *  \class Thread thread.h "concurrency/thread.h"
   *  \brief Thread class.
   *
   *  This class represents a thread and its associated methods.
   */
  class       Thread
  {
   private:
    bool      init_;
    pthread_t thread_;
              Thread(const Thread& thread);
    Thread&   operator=(const Thread& thread);
    // Helper function for thread creation. See Run().
    template  <typename T>
    static void*     ThreadHelper(void* ptr)
    {
      T* obj;

      obj = static_cast<T*>(ptr);
      obj->operator()();
      return (NULL);
    }

   public:
              Thread();
              ~Thread();
    void      Cancel();
    void      Detach();
    void      Join();

    /**
     *  \brief Run a thread.
     *
     *  The thread will be run using as entry point the operator()() method of
     *  the t object.
     *  \par Safety Minimal exception safety.
     *
     *  \param[in] t     Object.
     */
    template  <typename T>
    void      Run(T* t)
    {
      // Thread has not already been run.
      if (!this->init_)
        {
          int ret;

          // Run the thread using an helper static method.
          ret = pthread_create(&this->thread_, NULL, &ThreadHelper<T>, t);
          if (ret)
            throw (Exception(ret, strerror(ret)));
          this->init_ = true;
        }
      else
        throw (Exception(0, "Thread is already running and" \
                            " has not been detached."));

      return ;
    }
  };
}

#endif /* !CONCURRENCY_THREAD_H_ */
