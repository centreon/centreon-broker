/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <memory>                        // for auto_ptr
#include <pthread.h>
#include <stdlib.h>                      // for abort
#include <string.h>                      // for strerror
#include "concurrency/thread.h"
#include "concurrency/thread_listener.h"
#include "exception.h"

using namespace Concurrency;

/**************************************
*                                     *
*             Local Type              *
*                                     *
**************************************/

struct ThreadHelperArg
{
  Thread*         t;
  ThreadListener* tl;
};

/**************************************
*                                     *
*           Static Function           *
*                                     *
**************************************/

/**
 *  Entry point of all threads.
 */
void* ThreadHelper(void* arg)
{
  std::auto_ptr<ThreadHelperArg> harg(static_cast<ThreadHelperArg*>(arg));

  // Thread is starting.
  try
    {
      if (harg->tl)
        harg->tl->OnCreate(harg->t);
      harg->t->operator()();
    }
  catch (...)
    {
      // Exception caught. Thread execution failed.
      if (harg->tl)
        try
          {
            harg->tl->OnFailure(harg->t);
          }
        catch (...) {}
    }
  // Thread will exit soon.
  if (harg->tl)
    try
      {
        harg->tl->OnExit(harg->t);
      }
    catch (...) {}

  return (NULL);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Thread copy constructor.
 *
 *  As Thread is not copyable, any attempt to use the copy constructor will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] thread Unused.
 */
Thread::Thread(const Thread& thread)
{
  (void)thread;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Thread is not copyable, any attempt to use this assignment operator will
 *  result in a call to abort().
 *  \par Safety No Exception safety.
 *
 *  \param[in] thread Unused.
 *
 *  \return *this
 */
Thread& Thread::operator=(const Thread& thread)
{
  (void)thread;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Thread default constructor.
 */
Thread::Thread() : joinable_(false), should_exit(true) {}

/**
 *  \brief Thread destructor.
 *
 *  If the thread is still running, it's completion will be awaited (Join()'d)
 *  if it has not been detached.
 *  \par Safety No exception guarantee.
 */
Thread::~Thread()
{
  // If thread is running and is not detached.
  if (this->joinable_)
    try
      {
        // Wait for thread execution completion.
        this->Join();
      }
    catch (...)
      {
        try
          {
            // If we couldn't reach the thread, try to cancel its execution.
            this->Cancel();
            this->Join();
          }
        catch (...) {}
      }
}

/**
 *  \brief Cancel thread execution.
 *
 *  This method should only be used on critical situations. No thread should be
 *  terminated this way. It should instead be detached or joined to ensure its
 *  proper termination. No exception will be thrown if either the thread is
 *  running or not.
 *  \par Safety No exception guarantee.
 */
void Thread::Cancel()
{
  pthread_cancel(this->thread_);
  return ;
}

/**
 *  \brief Detach the current thread.
 *
 *  Detaching a thread means that it can automatically be cleaned up by the
 *  operating system upon its termination. However, it won't be possible to
 *  Join() it anymore. Detach() shall only be called once per thread creation.
 *  \par Safety Minimal exception safety.
 *
 *  \throw Exception Detach operation failed in some way.
 */
void Thread::Detach()
{
  if (this->joinable_)
    {
      int ret;

      ret = pthread_detach(this->thread_);
      if (ret)
        throw (Exception(ret, strerror(ret)));
      this->joinable_ = false;
    }
  else
    throw (Exception(0, "Thread has already been detached."));
  return ;
}

/**
 *  Request thread to exit ASAP.
 */
void Thread::Exit()
{
  this->should_exit = true;
  return ;
}

/**
 *  \brief Waits for thread completion.
 *
 *  Waits for the current thread to terminate. The thread shall not have been
 *  detached.
 *  \par Safety Minimal exception safety.
 *
 *  \throw Exception Could not join thread.
 */
void Thread::Join()
{
  if (this->joinable_)
    {
      int ret;
      void* ptr;

      ret = pthread_join(this->thread_, &ptr);
      if (ret)
        throw (Exception(ret, strerror(ret)));
      this->joinable_ = false;
    }
  else
    throw (Exception(0, "Trying to join invalid or detached thread."));
  return ;
}

/**
 *  \brief Run the thread.
 *
 *  The thread will be run using the operator()() method as overload.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] tl Optionnal thread listener.
 */
void Thread::Run(ThreadListener* tl)
{
  // Thread has not already been run.
  if (!this->joinable_)
    {
      std::auto_ptr<ThreadHelperArg> arg(new ThreadHelperArg);
      int ret;

      // Set the thread helper argument.
      arg->t = this;
      arg->tl = tl;
      this->listener = tl;

      // Run the thread using an helper static method.
      this->joinable_ = true;
      this->should_exit = false;
      ret = pthread_create(&this->thread_, NULL, &ThreadHelper, arg.get());
      if (ret)
        {
          this->joinable_ = false;
          this->should_exit = true;
          throw (Exception(ret, strerror(ret)));
        }
      else
        arg.release();
    }
  else
    throw (Exception(0, "Thread is already running " \
                        "and has not been detached."));

  return ;
}
