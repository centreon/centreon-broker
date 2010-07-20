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

#include "events/group.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal members of the given object to the current instance.
 *
 *  This internal method is used to copy data defined inside the Group class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in HostGroup copy constructor and in the =
 *  operator overload.
 *
 *  \param[in] group Object to copy from.
 *
 *  \see Group(const Group&)
 *  \see operator=(const Group&)
 */
void Group::InternalCopy(const Group& group)
{
  this->action_url  = group.action_url;
  this->alias       = group.alias;
  this->instance_id = group.instance_id;
  this->name        = group.name;
  this->notes       = group.notes;
  this->notes_url   = group.notes_url;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Group default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
Group::Group() : instance_id(0) {}

/**
 *  \brief Group copy constructor.
 *
 *  Copy all members of the Group object to the current instance.
 *
 *  \param[in] group Object to copy from.
 */
Group::Group(const Group& group) : Event(group)
{
  this->InternalCopy(group);
}

/**
 *  Group destructor.
 */
Group::~Group() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the Group object to the current instance.
 *
 *  \param[in] group Object to copy from.
 *
 *  \return *this
 */
Group& Group::operator=(const Group& group)
{
  this->Event::operator=(group);
  this->InternalCopy(group);
  return (*this);
}
