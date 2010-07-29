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

#include "events/group_member.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy data members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] gm Object to copy from.
 */
void GroupMember::InternalCopy(const GroupMember& gm)
{
  this->group       = gm.group;
  this->host_id     = gm.host_id;
  this->instance_id = gm.instance_id;
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
GroupMember::GroupMember() : host_id(0), instance_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] gm Object to copy from.
 */
GroupMember::GroupMember(const GroupMember& gm) : Event(gm)
{
  this->InternalCopy(gm);
}

/**
 *  Destructor.
 */
GroupMember::~GroupMember() {}

/**
 *  Assignement operator overload.
 *
 *  \param[in] gm Object to copy from.
 *
 *  \return *this
 */
GroupMember& GroupMember::operator=(const GroupMember& gm)
{
  this->Event::operator=(gm);
  this->InternalCopy(gm);
  return (*this);
}
