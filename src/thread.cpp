/*
** thread.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
*/

#include <cassert>
#include <pthread.h>
#include "thread.h"

using namespace CentreonBroker;

/**
 *  This function will help us start the thread object.
 */
static void* thread_start_helper(void* arg)
{
  static_cast<Thread*>(arg)->Core();
  return (NULL);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Thread copy constructor. The method is declared private because a copy of a
 *  thread has no sense.
 */
Thread::Thread(const Thread& thread)
{
  (void)thread;
}

/**
 *  Thread operator= overload. The method is declared private because a copy of
 *  a thread has no sense.
 */
Thread& Thread::operator=(const Thread& thread)
{
  (void)thread;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Thread constructor.
 */
Thread::Thread()
{
  this->thread = NULL;
}

/**
 *  Thread destructor.
 */
Thread::~Thread()
{
  if (this->thread)
    {
      try
	{
	  this->Join();
	}
      catch (...)
	{
	}
    }
}

/**
 *  Cancels the thread execution.
 */
void Thread::Cancel() throw (Exception)
{
  if (pthread_cancel(*this->thread))
    throw (Exception("Could not cancel thread execution"));
  return ;
}

/**
 *  Waits for the thread to finish.
 */
void Thread::Join() throw (Exception)
{
  assert(thread);
  if (pthread_join(*this->thread, NULL))
    throw (Exception("Could not join thread."));
  delete (this->thread);
  this->thread = NULL;
  return ;
}

/**
 *  This method starts the thread. The Core() method will be executed on the
 *  thread.
 */
void Thread::Run() throw (Exception)
{
  this->thread = new (pthread_t);
  if (pthread_create(this->thread,
                     NULL,
                     thread_start_helper,
                     this))
    {
      delete (this->thread);
      this->thread = NULL;
      throw (Exception("Could not create thread."));
    }
  return ;
}
