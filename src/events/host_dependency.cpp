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

#include "events/host_dependency.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
HostDependency::HostDependency() {}

/**
 *  Copy constructor.
 *
 *  \param[in] host_dependency Object to copy from.
 */
HostDependency::HostDependency(const HostDependency& host_dependency)
  : Dependency(host_dependency) {}

/**
 *  Destructor.
 */
HostDependency::~HostDependency() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] host_dependency Object to copy from.
 *
 *  \return *this
 */
HostDependency& HostDependency::operator=(
  const HostDependency& host_dependency)
{
  this->HostDependency::operator=(host_dependency);
  return (*this);
}

/**
 *  Get the type of this object (Event::HOSTDEPENDENCY).
 *
 *  \return Event::HOSTDEPENDENCY
 */
int HostDependency::GetType() const
{
  return (Event::HOSTDEPENDENCY);
}
