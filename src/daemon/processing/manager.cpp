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

#include <algorithm>            // for find
#include <assert.h>
#include <memory>               // for auto_ptr
#include <stdlib.h>             // for abort
#include "concurrency/lock.h"
#include "concurrency/thread.h"
#include "exception.h"
#include "processing/manager.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Manager default constructor.
 */
Manager::Manager() {}

/**
 *  \brief Manager copy constructor.
 *
 *  Manager is not copyable, therefore any attempt to use this copy constructor
 *  will result in a call to abort().
 *
 *  \param[in] manager Unused.
 */
Manager::Manager(const Manager& manager) : Concurrency::ThreadListener(manager)
{
  (void)manager;
  assert(false);
  abort();
}

/**
 *  Manager destructor.
 */
Manager::~Manager()
{
  try
    {
      // Delete all threads.
      while (1)
        {
	  Concurrency::Thread* thread;
          {
            std::list<Concurrency::Thread*>::iterator it;
            Concurrency::Lock lock(this->threadsm_);

            it = this->threads_.begin();
            if (this->threads_.end() == it)
              break ;
            thread = *it;
            this->threads_.pop_front();
          }
          try { delete (thread); }
          catch (...) {}
        }
    }
  catch (...) {}
}

/**
 *  \brief Assignment operator overload.
 *
 *  Manager is not copyable, therefore any attempt to use this assignment
 *  operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] manager Unused.
 *
 *  \return *this
 */
Manager& Manager::operator=(const Manager& manager)
{
  (void)manager;
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
 *  Delete a thread identified by its handle.
 *
 *  \param[in] thread Handle of the thread to delete.
 */
void Manager::Delete(const Concurrency::Thread* thread)
{
  std::list<Concurrency::Thread*>::iterator it;
  Concurrency::Lock lock(this->threadsm_);

  it = std::find(this->threads_.begin(), this->threads_.end(), thread);
  if (it != this->threads_.end())
    {
      std::auto_ptr<Concurrency::Thread> t(*it);

      this->threads_.erase(it);
    }
  return ;
}

/**
 *  \brief Get the only instance of the Manager singleton.
 *
 *  Returns a statically allocated Manager object.
 *  \par Safety No throw guarantee.
 *
 *  \return The Manager instance.
 */
Manager& Manager::Instance()
{
  static Manager manager;

  return (manager);
}

/**
 *  \brief Manage a thread.
 *
 *  The Feeder should be managed before it is started.
 *
 *  \param[in] thread Thread to manage.
 */
void Manager::Manage(Concurrency::Thread* thread)
{
  Concurrency::Lock lock(this->threadsm_);

  this->threads_.push_back(thread);
  return ;
}

/**
 *  Callback method when a new thread is created.
 *
 *  \param[in] thread New thread to manage.
 */
void Manager::OnCreate(Concurrency::Thread* thread)
{
  this->Manage(thread);
  return ;
}

/**
 *  Callback method called when a thread is about to exit.
 */
void Manager::OnExit(Concurrency::Thread* thread)
{
  this->Delete(thread);
  return ;
}
