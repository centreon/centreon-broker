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
#include <stdlib.h>        // for abort
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
 *  Build a new source object from its configuration.
 *
 *  \param[in] i Configuration of the new source.
 */
void Manager::Build(const Configuration::Interface& i)
{
}

/**
 *  Delete a source identified by its name.
 *
 *  \param[in] name Name of the source to delete.
 */
void Manager::Delete(const std::string& name)
{
}

/**
 *  Delete a source identified by its Feeder handle (for internal use).
 *
 *  \param[in] feeder Handle of the feeder to delete.
 */
void Manager::Delete(const Feeder* feeder)
{
}

/**
 *  Delete a source identified by its Listener handle (for internal use).
 *
 *  \param[in] listener Handle of the listener to delete.
 */
void Manager::Delete(const Listener* listener)
{
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
 *  \brief Manage an already opened event source.
 *
 *  A new Feeder will be created to handle event retrieving and publishing.
 *  Upon successful return, the source will be owned by the Manager.
 *
 *  \param[in] source Open event source.
 */
void Manager::Manage(Interface::Source* source)
{
}

/**
 *  \brief Update an interface.
 *
 *  The configuration must have the same name for the update to success. If the
 *  configuration changed, it is likely that the source object will be
 *  destroyed and reopened.
 *
 *  \param[in] i Configuration of an event source.
 */
void Manager::Update(const Configuration::Interface& i)
{
}
