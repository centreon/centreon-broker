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

#include "events/service_group_member.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
ServiceGroupMember::ServiceGroupMember() : service_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] sgm Object to copy from.
 */
ServiceGroupMember::ServiceGroupMember(const ServiceGroupMember& sgm)
  : GroupMember(sgm), service_id(sgm.service_id) {}

/**
 *  Destructor.
 */
ServiceGroupMember::~ServiceGroupMember() {}

/**
 *  Assignement operator overload.
 *
 *  \param[in] sgm Object to copy from.
 *
 *  \return *this
 */
ServiceGroupMember& ServiceGroupMember::operator=(const ServiceGroupMember& sgm)
{
  this->GroupMember::operator=(sgm);
  this->service_id = sgm.service_id;
  return (*this);
}

/**
 *  Get the type of this event (Event::SERVICEGROUPMEMBER).
 *
 *  \return Event::SERVICEGROUPMEMBER
 */
int ServiceGroupMember::GetType() const
{
  return (Event::SERVICEGROUPMEMBER);
}
