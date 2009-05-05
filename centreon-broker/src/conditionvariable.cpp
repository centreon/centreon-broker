/*
** conditionvariable.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/05/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#include "conditionvariable.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ConditionVariable copy constructor.
 */
ConditionVariable::ConditionVariable(const ConditionVariable& cv)
{
  (void)cv;
}

/**
 *  ConditionVariable operator= overload.
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
  if (pthread_cond_init(&this->condvar, NULL))
    throw (Exception("ConditionVariable initialization failed."));
}

/**
 *  ConditionVariable destructor.
 */
ConditionVariable::~ConditionVariable()
{
  pthread_cond_destroy(&this->condvar);
}

/**
 *  The Broadcast method shall unblock all threads waiting on the
 *  ConditionVariable.
 */
void ConditionVariable::Broadcast() throw (Exception)
{
  if (pthread_cond_broadcast(&this->condvar))
    throw (Exception("Could not broadcast on ConditionVariable."));
  return ;
}

/**
 *  The Signal method shall unblock at least one thread that is waiting on the
 *  ConditionVariable.
 */
void ConditionVariable::Signal() throw (Exception)
{
  if (pthread_cond_signal(&this->condvar))
    throw (Exception("Could not signal a condition variable."));
  return ;
}

/**
 *  This method causes the calling thread, having ownership of the mutex, to
 *  sleep until it is unblocked by a Broadcast() or Signal() call.
 */
void ConditionVariable::Wait(Mutex& mutex) throw (Exception)
{
  if (pthread_cond_wait(&this->condvar, &mutex.mutex))
    throw (Exception("Wait on a condition variable failed."));
  return ;
}
