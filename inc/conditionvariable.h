/*
** conditionvariable.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/05/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef CONDITIONVARIABLE_H_
# define CONDITIONVARIABLE_H_

# include <pthread.h>
# include "mutex.h"

namespace              CentreonBroker
{
  class                ConditionVariable
  {
   private:
    pthread_cond_t     condvar;
                       ConditionVariable(const ConditionVariable& cv);
    ConditionVariable& operator=(const ConditionVariable& cv);

   public:
                       ConditionVariable() throw (CentreonBroker::Exception);
                       ~ConditionVariable();
    void               Broadcast() throw (CentreonBroker::Exception);
    void               Signal() throw (CentreonBroker::Exception);
    void               Wait(CentreonBroker::Mutex& mutex)
      throw (CentreonBroker::Exception);
  };
}

#endif /* !CONDITIONVARIABLE_H_ */
