/*
** mutex.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#include <cstring>
#include <pthread.h>
#include "exception.h"
#include "mutex.h"

using namespace CentreonBroker;

/******************************************************************************
*                                                                             *
*                                                                             *
*                              MutexException                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MutexException default constructor.
 */
MutexException::MutexException()
{
  this->where_ = UNKNOWN;
}

/**
 *  MutexException copy constructor.
 */
MutexException::MutexException(const MutexException& me) : Exception(me)
{
  this->where_ = me.where_;
}

/**
 *  Build a MutexException from a string and an optional where argument.
 */
MutexException::MutexException(const char* str, MutexException::Where w)
  : Exception(str)
{
  this->where_ = w;
}

/**
 *  Build a MutexException from a string and an optional where argument.
 */
MutexException::MutexException(const std::string& str, MutexException::Where w)
  : Exception(str)
{
  this->where_ = w;
}

/**
 *  MutexException destructor.
 */
MutexException::~MutexException() throw()
{
}

/**
 *  MutexException operator= overload.
 */
MutexException& MutexException::operator=(const MutexException& me)
{
  Exception::operator=(me);
  this->where_ = me.where_;
  return (*this);
}

/**
 *  Returns an enum defining where the exception happened.
 */
MutexException::Where MutexException::GetWhere() const throw()
{
  return (this->where_);
}

/**
 *  Sets where the exception happened.
 */
void MutexException::SetWhere(MutexException::Where w) throw()
{
  this->where_ = w;
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                 Mutex                                       *
*                                                                             *
*                                                                             *
******************************************************************************/

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
Mutex::Mutex() throw (MutexException)
{
  int error_code;

  error_code = pthread_mutex_init(&this->mutex_, NULL);
  if (error_code)
    throw (MutexException(std::string(__FUNCTION__)
                          + ": "
                          + strerror(error_code),
                          MutexException::INIT));
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
void Mutex::Lock() throw (MutexException)
{
  int error_code;

  error_code = pthread_mutex_lock(&this->mutex_);
  if (error_code)
    throw (MutexException(std::string(__FUNCTION__)
                          + ": "
                          + strerror(error_code),
                          MutexException::LOCK));
  return ;
}

/**
 *  Unlocks an already locked mutex.
 */
void Mutex::Unlock() throw (MutexException)
{
  int error_code;

  error_code = pthread_mutex_unlock(&this->mutex_);
  if (error_code)
    throw (MutexException(std::string(__FUNCTION__)
                          + ": "
                          + strerror(error_code),
                          MutexException::UNLOCK));
  return ;
}
