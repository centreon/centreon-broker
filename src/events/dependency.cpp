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

#include "events/dependency.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] dependency Object to copy from.
 */
void Dependency::InternalCopy(const Dependency& dependency)
{
  this->dependency_period            = dependency.dependency_period;
  this->dependent_host_id            = dependency.dependent_host_id;
  this->execution_failure_options    = dependency.execution_failure_options;
  this->host_id                      = dependency.host_id;
  this->inherits_parent              = dependency.inherits_parent;
  this->notification_failure_options = dependency.notification_failure_options;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
Dependency::Dependency()
  : dependent_host_id(0), host_id(0), inherits_parent(false) {}

/**
 *  Copy constructor.
 *
 *  \param[in] dependency Object to copy from.
 */
Dependency::Dependency(const Dependency& dependency) : Event(dependency)
{
  this->InternalCopy(dependency);
}

/**
 *  Destructor.
 */
Dependency::~Dependency() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] dependency Object to copy from.
 *
 *  \return *this
 */
Dependency& Dependency::operator=(const Dependency& dependency)
{
  this->Event::operator=(dependency);
  this->InternalCopy(dependency);
  return (*this);
}
