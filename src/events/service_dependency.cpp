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

#include "events/service_dependency.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal members from the given object.
 *
 *  @param[in] sd Object to copy from.
 */
void ServiceDependency::InternalCopy(const ServiceDependency& sd)
{
  this->dependent_service_id = sd.dependent_service_id;
  this->service_id           = sd.service_id;
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
ServiceDependency::ServiceDependency()
  : dependent_service_id(0), service_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] service_dependency Object to copy from.
 */
ServiceDependency::ServiceDependency(
  const ServiceDependency& service_dependency)
  : Dependency(service_dependency)
{
  this->InternalCopy(service_dependency);
}

/**
 *  Destructor.
 */
ServiceDependency::~ServiceDependency() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] service_dependency Object to copy from.
 *
 *  \return *this
 */
ServiceDependency& ServiceDependency::operator=(
  const ServiceDependency& service_dependency)
{
  this->ServiceDependency::operator=(service_dependency);
  this->InternalCopy(service_dependency);
  return (*this);
}

/**
 *  Get the type of this object (Event::SERVICEDEPENDENCY).
 *
 *  \return Event::SERVICEDEPENDENCY
 */
int ServiceDependency::GetType() const
{
  return (Event::SERVICEDEPENDENCY);
}
