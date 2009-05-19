/*
** condition_variable.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/05/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#include <cerrno>
#include <cstring>
#include <pthread.h>
#include <string>
#include "condition_variable.h"
#include "mutex.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ConditionVariable copy constructor. Declared private because copying a
 *  condition variable makes no sense.
 */
ConditionVariable::ConditionVariable(const ConditionVariable& cv)
{
  (void)cv;
}

/**
 *  ConditionVariable operator= overload. Declared private because copying a
 *  condition variable makes no sense.
 */
ConditionVariable& ConditionVariable::operator=(const ConditionVariable &cv)
{
  (void)cv;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  ConditionVariable constructor.
 */
ConditionVariable::ConditionVariable() throw (Exception)
{
  int error_code;

  error_code = pthread_cond_init(&this->condvar_, NULL);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
}

/**
 *  ConditionVariable destructor.
 */
ConditionVariable::~ConditionVariable() throw()
{
  pthread_cond_destroy(&this->condvar_);
}

/**
 *  The Broadcast() method shall unblock all threads waiting on the
 *  condition variable.
 */
void ConditionVariable::Broadcast() throw (Exception)
{
  int error_code;

  error_code = pthread_cond_broadcast(&this->condvar_);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
  return ;
}

/**
 *  The Signal() method shall unblock at least one thread that is waiting on
 *  the condition variable.
 */
void ConditionVariable::Signal() throw (Exception)
{
  int error_code;

  error_code = pthread_cond_signal(&this->condvar_);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
  return ;
}

/**
 *  This method causes the calling thread, having ownership of the mutex, to
 *  sleep until it is unblocked by a Broadcast() call, a Signal() call or until
 *  system time exceeds abstime. In the latter case, the function will return
 *  true.
 */
bool ConditionVariable::TimedWait(Mutex& mutex, const struct timespec* abstime)
  throw (Exception)
{
  int error_code;
  bool return_value;

  error_code = pthread_cond_timedwait(&this->condvar_, &mutex.mutex_, abstime);
  if (error_code)
    {
      if (ETIMEDOUT == error_code)
	return_value = true;
      else
	throw (Exception(error_code, strerror(error_code)));
    }
  else
    error_code = false;
  return (error_code);
}

/**
 *  This method causes the calling thread, having ownership of the mutex, to
 *  sleep until it is unblocked by a Broadcast() or Signal() call.
 */
void ConditionVariable::Wait(Mutex& mutex) throw (Exception)
{
  int error_code;

  error_code = pthread_cond_wait(&this->condvar_, &mutex.mutex_);
  if (error_code)
    throw (Exception(error_code, strerror(error_code)));
  return ;
}
