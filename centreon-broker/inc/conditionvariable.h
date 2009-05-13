/*
** conditionvariable.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/05/09 Matthieu Kermagoret
** Last update 05/13/09 Matthieu Kermagoret
*/

#ifndef CONDITIONVARIABLE_H_
# define CONDITIONVARIABLE_H_

# include <sys/types.h>
# include <time.h>
# include "exception.h"

namespace                       CentreonBroker
{
  class                         Mutex;

  /**
   *  This class of exception will be raised by ConditionVariable when
   *  needed.
   */
  class                         ConditionVariableException : public Exception
  {
   public:
    enum                        Where
    {
      UNKNOWN,
      BROADCAST,
      INIT,
      SIGNAL,
      TIMEDWAIT,
      WAIT
    };

   private:
    Where                       where_;

   public:
                                ConditionVariableException();
                                ConditionVariableException(const
                                  ConditionVariableException& cve);
                                ConditionVariableException(const char* str,
                                                           Where w = UNKNOWN);
                                ConditionVariableException(const
							   std::string& str,
                                                           Where w = UNKNOWN);
                                ~ConditionVariableException() throw ();
    ConditionVariableException& operator=(const ConditionVariableException& c);
    Where                       GetWhere() const throw ();
    void                        SetWhere(Where w) throw ();
  };

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
                       ConditionVariable() throw (ConditionVariableException);
                       ~ConditionVariable() throw();
    void               Broadcast() throw (ConditionVariableException);
    void               Signal() throw (ConditionVariableException);
    bool               TimedWait(Mutex& mutex, const struct timespec* abstime)
                         throw (ConditionVariableException);
    void               Wait(Mutex& mutex) throw (ConditionVariableException);
  };
}

#endif /* !CONDITIONVARIABLE_H_ */
