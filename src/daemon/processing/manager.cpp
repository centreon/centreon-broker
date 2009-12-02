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
#include "concurrency/lock.h"
#include "exception.h"
#include "processing/feeder.h"
#include "processing/listener.h"
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
Manager::Manager(const Manager& manager)
{
  (void)manager;
  assert(false);
  abort();
}

/**
 *  Manager destructor.
 */
Manager::~Manager() {}

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
 *  Delete a source identified by its Feeder handle.
 *
 *  \param[in] feeder Handle of the feeder to delete.
 */
void Manager::Delete(const Feeder* feeder)
{
  std::list<Feeder*>::iterator end;
  std::list<Feeder*>::iterator it;
  Concurrency::Lock lock(this->mutex_);

  end = this->feeders_.end();
  for (it = this->feeders_.begin(); it != end; ++it)
    if (*it == feeder)
      {
        delete (*it);
        this->feeders_.erase(it);
        break ;
      }
  return ;
}

/**
 *  Delete a source identified by its Listener handle.
 *
 *  \param[in] listener Handle of the listener to delete.
 */
void Manager::Delete(const Listener* listener)
{
  std::list<Listener*>::iterator end;
  std::list<Listener*>::iterator it;
  Concurrency::Lock lock(this->mutex_);

  end = this->listeners_.end();
  for (it = this->listeners_.begin(); it != end; ++it)
    if (*it == listener)
      {
        delete (*it);
        this->listeners_.erase(it);
        break ;
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
 *  \brief Manage an event feeder (internal use).
 *
 *  The Feeder should be managed before it is started.
 *
 *  \param[in] feeder Feeder to manager.
 */
void Manager::Manage(Feeder* feeder)
{
  Concurrency::Lock lock(this->mutex_);

  this->feeders_.push_back(feeder);
  return ;
}

/**
 *  \brief Manage a listener.
 *
 *  The Listener should be managed before it is started.
 *
 *  \param[in] listener Listener to manage.
 */
void Manager::Manage(Listener* listener)
{
  Concurrency::Lock lock(this->mutex_);

  this->listeners_.push_back(listener);
  return ;
}
