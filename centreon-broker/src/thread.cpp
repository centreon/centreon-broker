/*
** thread.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/13/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstring>
#include <pthread.h>
#include "thread.h"

using namespace CentreonBroker;

/******************************************************************************
*                                                                             *
*                                                                             *
*                             ThreadException                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ThreadException default constructor.
 */
ThreadException::ThreadException()
{
  this->where_ = UNKNOWN;
}

/**
 *  ThreadException copy constructor.
 */
ThreadException::ThreadException(const ThreadException& te) : Exception(te)
{
  this->where_ = te.where_;
}

/**
 *  Build a ThreadException from a string and an optional where argument.
 */
ThreadException::ThreadException(const char* str, ThreadException::Where w)
  : Exception(str)
{
  this->where_ = w;
}

/**
 *  Build a ThreadException from a string and an optional where argument.
 */
ThreadException::ThreadException(const std::string& str,
  ThreadException::Where w)
  : Exception(str)
{
  this->where_ = w;
}

/**
 *  ThreadException destructor.
 */
ThreadException::~ThreadException() throw ()
{
}

/**
 *  ThreadException operator= overload.
 */
ThreadException& ThreadException::operator=(const ThreadException& te)
{
  Exception::operator=(te);
  this->where_ = te.where_;
  return (*this);
}

/**
 *  Returns an enum telling where the exception appeared, if known.
 */
ThreadException::Where ThreadException::GetWhere() const throw ()
{
  return (this->where_);
}

/**
 *  Sets where the exception appeared.
 */
void ThreadException::SetWhere(ThreadException::Where w) throw ()
{
  this->where_ = w;
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                 Thread                                      *
*                                                                             *
*                                                                             *
******************************************************************************/

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
void Thread::Cancel() throw (ThreadException)
{
  int error_code;

  error_code = pthread_cancel(*this->thread);
  if (error_code)
    throw (ThreadException(std::string(__FUNCTION__)
                           + ": "
                           + strerror(error_code),
                           ThreadException::CANCEL));
  return ;
}

/**
 *  Waits for the thread to finish.
 */
void Thread::Join() throw (ThreadException)
{
  int error_code;

  assert(thread);
  error_code = pthread_join(*this->thread, NULL);
  if (error_code)
    throw (ThreadException(std::string(__FUNCTION__)
                           + ": "
                           + strerror(error_code),
                           ThreadException::JOIN));
  delete (this->thread);
  this->thread = NULL;
  return ;
}

/**
 *  This method starts the thread. The Core() method will be executed on the
 *  thread.
 */
void Thread::Run() throw (ThreadException)
{
  int error_code;

  this->thread = new (pthread_t);
  error_code = pthread_create(this->thread,
                              NULL,
                              thread_start_helper,
                              this);
  if (error_code)
    {
      delete (this->thread);
      this->thread = NULL;
      throw (ThreadException(std::string(__FUNCTION__)
                             + ": "
                             + strerror(error_code),
                             ThreadException::RUN));
    }
  return ;
}
