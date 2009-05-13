/*
** thread.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/13/09 Matthieu Kermagoret
*/

#ifndef THREAD_H_
# define THREAD_H_

# include <sys/types.h>
# include "exception.h"

namespace       CentreonBroker
{
  /**
   *  Standard exception thrown by the Thread class.
   */
  class              ThreadException : public Exception
  {
   public:
    enum             Where
    {
      UNKNOWN,
      CANCEL,
      JOIN,
      RUN
    };

   private:
    Where            where_;

   public:
                     ThreadException();
                     ThreadException(const ThreadException& te);
                     ThreadException(const char* str, Where w = UNKNOWN);
                     ThreadException(const std::string& str,
                                     Where w = UNKNOWN);
                     ~ThreadException() throw();
    ThreadException& operator=(const ThreadException& te);
    Where            GetWhere() const throw ();
    void             SetWhere(Where w) throw ();
  };

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
    void        Cancel() throw (ThreadException);
    virtual int Core() = 0;
    void        Join() throw (ThreadException);
    void        Run() throw (ThreadException);
  };
}

#endif /* !THREAD_H_ */
