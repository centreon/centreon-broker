/*
** mutex.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#ifndef MUTEX_H_
# define MUTEX_H_

# include <sys/types.h>
# include "exception.h"

namespace           CentreonBroker
{
  /**
   *  This is the base exception that will be thrown by Mutex methods whenever
   *  needed.
   */
  class             MutexException : public Exception
  {
   public:
    enum            Where
    {
      UNKNOWN,
      INIT,
      LOCK,
      UNLOCK
    };

   private:
    Where           where_;

   public:
                    MutexException();
                    MutexException(const MutexException& me);
                    MutexException(const char* str, Where w = UNKNOWN);
                    MutexException(const std::string& str, Where w = UNKNOWN);
                    ~MutexException() throw();
    MutexException& operator=(const MutexException& me);
    Where           GetWhere() const throw();
    void            SetWhere(Where w) throw();
  };

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
                    Mutex() throw (MutexException);
                    ~Mutex() throw();
    void            Lock() throw (MutexException);
    void            Unlock() throw (MutexException);
  };
}

#endif /* !MUTEX_H_ */
