/*
** mutex.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#include <pthread.h>
#include "exception.h"
#include "mutex.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Mutex copy constructor. It is declared private because it has no sense to
 *  copy a mutex.
 */
Mutex::Mutex(const Mutex& mutex)
{
  (void)mutex;
}

/**
 *  Mutex operator= overload. It is declared private because it has no sense to
 *  copy a mutex.
 */
Mutex& Mutex::operator=(const Mutex& mutex)
{
  (void)mutex;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Mutex constructor.
 */
Mutex::Mutex() throw(Exception)
{
  if (pthread_mutex_init(&this->mutex, NULL))
    throw (Exception("Mutex initialization failed."));
}

/**
 *  Mutex destructor.
 */
Mutex::~Mutex()
{
  pthread_mutex_destroy(&this->mutex);
}

/**
 *  Lock a mutex.
 */
void		Mutex::Lock() throw (Exception)
{
  if (pthread_mutex_lock(&this->mutex))
    throw (Exception("Mutex locking failed."));
  return ;
}

/**
 *  Unlock an already locked mutex.
 */
void		Mutex::Unlock() throw (Exception)
{
  if (pthread_mutex_unlock(&this->mutex))
    throw (Exception("Mutex unlocking failed."));
  return ;
}
