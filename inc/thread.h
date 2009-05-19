/*
** thread.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef THREAD_H_
# define THREAD_H_

# include <sys/types.h>
# include "exception.h"

namespace       CentreonBroker
{
  /**
   *  The Thread class is the root of all threads that can be created in
   *  CentreonBroker. It is really to use. A class that want to be thread just
   *  have to subclass Thread and redefine Core(). Then just call Run(), Join()
   *  or Cancel() to manager the Thread.
   */
  class         Thread
  {
   private:
    pthread_t   *thread;
                Thread(const Thread& thread);
    Thread&     operator=(const Thread& thread);

   public:
                Thread();
    virtual     ~Thread();
    void        Cancel() throw (Exception);
    virtual int Core() = 0;
    void        Join() throw (Exception);
    void        Run() throw (Exception);
  };
}

#endif /* !THREAD_H_ */
