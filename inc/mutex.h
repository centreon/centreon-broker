/*
** mutex.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef MUTEX_H_
# define MUTEX_H_

# include <sys/types.h>
# include "exception.h"

namespace           CentreonBroker
{
  /**
   *  A Mutex is a synchronization primitive that shall be used when two
   *  Threads can access the same ressource simultaneously. Only the thread
   *  that has successfully locked the Mutex can then access the ressource.
   */
  class             Mutex
  {
    // With pthreads, the pthread_cond_wait call has to access the Mutex's
    // pthread_mutex_t internal structure.
    friend class    ConditionVariable;

   private:
    pthread_mutex_t mutex_;
                    Mutex(const Mutex& mutex);
    Mutex&          operator=(const Mutex& mutex);

   public:
                    Mutex() throw (Exception);
                    ~Mutex() throw();
    void            Lock() throw (Exception);
    void            Unlock() throw (Exception);
  };
}

#endif /* !MUTEX_H_ */
