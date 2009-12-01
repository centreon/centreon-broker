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
 *  Delete a source identified by its name.
 *
 *  \param[in] name Name of the source to delete.
 */
void Manager::Delete(const std::string& name)
{
  std::list<std::pair<std::string, Feeder*> >::iterator end1;
  std::list<std::pair<std::string, Listener*> >::iterator end2;
  std::list<std::pair<std::string, Feeder*> >::iterator it1;
  std::list<std::pair<std::string, Listener*> >::iterator it2;
  Concurrency::Lock lock(this->mutex_);

  // Try to find name among feeders.
  end1 = this->feeders_.end();
  for (it1 = this->feeders_.begin(); it1 != end1; ++it1)
    if (it1->first == name)
      {
	delete (it1->second);
	this->feeders_.erase(it1);
	return ;
      }

  // Try to find name among listeners.
  end2 = this->listeners_.end();
  for (it2 = this->listeners_.begin(); it2 != end2; ++it2)
    if (it2->first == name)
      {
	delete (it2->second);
	this->listeners_.erase(it2);
	break ;
      }

  return ;
}

/**
 *  Delete a source identified by its Feeder handle (for internal use).
 *
 *  \param[in] feeder Handle of the feeder to delete.
 */
void Manager::Delete(const Feeder* feeder)
{
  std::list<std::pair<std::string, Feeder*> >::iterator end;
  std::list<std::pair<std::string, Feeder*> >::iterator it;
  Concurrency::Lock lock(this->mutex_);

  end = this->feeders_.end();
  for (it = this->feeders_.begin(); it != end; ++it)
    if (it->second == feeder)
      {
	delete (it->second);
	this->feeders_.erase(it);
	break ;
      }
  return ;
}

/**
 *  Delete a source identified by its Listener handle (for internal use).
 *
 *  \param[in] listener Handle of the listener to delete.
 */
void Manager::Delete(const Listener* listener)
{
  std::list<std::pair<std::string, Listener*> >::iterator end;
  std::list<std::pair<std::string, Listener*> >::iterator it;
  Concurrency::Lock lock(this->mutex_);

  end = this->listeners_.end();
  for (it = this->listeners_.begin(); it != end; ++it)
    if (it->second == listener)
      {
	delete (it->second);
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
 *  \brief Manage an event feeder.
 *
 *  The Feeder should be managed before it is started.
 *
 *  \param[in] feeder Feeder to manager.
 *  \param[in] name   Name of the new feeder.
 */
void Manager::Manage(Feeder* feeder, const std::string& name)
{
  Concurrency::Lock lock(this->mutex_);

  this->feeders_.push_back(std::pair<std::string, Feeder*>(name, feeder));
  return ;
}

/**
 *  \brief Manage a listener.
 *
 *  The Listener should be managed before it is started.
 *
 *  \param[in] listener Listener to manage.
 *  \param[in] name     Name of the new listener.
 *
 *  \throw Exception Name already exists.
 */
void Manager::Manage(Listener* listener, const std::string& name)
{
  Concurrency::Lock lock(this->mutex_);

  this->listeners_.push_back(
    std::pair<std::string, Listener*>(name, listener));
  return ;
}
