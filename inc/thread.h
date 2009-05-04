/*
** thread.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#ifndef THREAD_H_
# define THREAD_H_

# include <sys/types.h>
# include "exception.h"

namespace       CentreonBroker
{
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
    void        Run() throw (Exception);
  };
}

#endif /* !THREAD_H_ */
