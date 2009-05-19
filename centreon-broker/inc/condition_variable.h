/*
** condition_variable.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/05/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef CONDITION_VARIABLE_H_
# define CONDITION_VARIABLE_H_

# include <sys/types.h>
# include <time.h>
# include "exception.h"

namespace                       CentreonBroker
{
  class                         Mutex;

  /**
   *  A condition variable is an object with which a thread can temporarily
   *  give up exclusive access, in order to wait for some condition to be met.
   */
  class                ConditionVariable
  {
   private:
    pthread_cond_t     condvar_;
                       ConditionVariable(const ConditionVariable& cv);
    ConditionVariable& operator=(const ConditionVariable& cv);

   public:
                       ConditionVariable() throw (Exception);
                       ~ConditionVariable() throw ();
    void               Broadcast() throw (Exception);
    void               Signal() throw (Exception);
    bool               TimedWait(Mutex& mutex, const struct timespec* abstime)
                         throw (Exception);
    void               Wait(Mutex& mutex) throw (Exception);
  };
}

#endif /* !CONDITION_VARIABLE_H_ */
