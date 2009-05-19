/*
** mutex.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#include <cstring>
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
Mutex::Mutex() throw (Exception)
{
  int error_code;

  error_code = pthread_mutex_init(&this->mutex_, NULL);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
}

/**
 *  Mutex destructor.
 */
Mutex::~Mutex() throw()
{
  pthread_mutex_destroy(&this->mutex_);
}

/**
 *  Locks a mutex.
 */
void Mutex::Lock() throw (Exception)
{
  int error_code;

  error_code = pthread_mutex_lock(&this->mutex_);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
  return ;
}

/**
 *  Unlocks an already locked mutex.
 */
void Mutex::Unlock() throw (Exception)
{
  int error_code;

  error_code = pthread_mutex_unlock(&this->mutex_);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
  return ;
}
