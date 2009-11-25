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
#include <stdlib.h>             // for abort
#include "concurrency/thread.h"

using namespace Concurrency;

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
Thread::Thread() : init_(false) {}

/**
 *  \brief Thread destructor.
 *
 *  If the thread is still running, it won't be Cancel()'d but instead
 *  Detach()'d.
 *  \par Safety No exception guarantee.
 */
Thread::~Thread()
{
  // If thread is running and is not detached.
  if (this->init_)
    try
      {
        // Try to detach it first.
        this->Detach();
      }
    catch (...)
      {
        try
          {
            // If we couldn't detach the thread, try to cancel its execution.
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
  if (this->init_)
    {
      int ret;

      ret = pthread_detach(this->thread_);
      if (ret)
        throw (Exception(ret, strerror(ret)));
      this->init_ = false;
    }
  else
    throw (Exception(0, "Thread has already been detached."));
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
  if (this->init_)
    {
      int ret;
      void* ptr;

      ret = pthread_join(this->thread_, &ptr);
      if (ret)
        throw (Exception(ret, strerror(ret)));
    }
  else
    throw (Exception(0, "Trying to join invalid or detached thread."));
  return ;
}
