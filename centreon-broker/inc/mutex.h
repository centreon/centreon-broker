/*
** mutex.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#ifndef MUTEX_H_
# define MUTEX_H_

# include <sys/types.h>
# include "exception.h"

namespace           CentreonBroker
{
  class             Mutex
  {
   private:
    pthread_mutex_t mutex;
                    Mutex(const Mutex& mutex);
    Mutex&          operator=(const Mutex& mutex);

   public:
                    Mutex() throw (CentreonBroker::Exception);
                    ~Mutex();
    void            Lock() throw (CentreonBroker::Exception);
    void            Unlock() throw (CentreonBroker::Exception);
  };
}

#endif /* !MUTEX_H_ */
