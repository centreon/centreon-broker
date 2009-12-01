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

#include "events/host_group.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief HostGroup default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
HostGroup::HostGroup() {}

/**
 *  \brief HostGroup copy constructor.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  \param[in] host_group Object to copy from.
 */
HostGroup::HostGroup(const HostGroup& host_group) : Group(host_group) {}

/**
 *  HostGroup destructor.
 */
HostGroup::~HostGroup() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  \param[in] host_group Object to copy from.
 *
 *  \return *this
 */
HostGroup& HostGroup::operator=(const HostGroup& host_group)
{
  this->Group::operator=(host_group);
  return (*this);
}

/**
 *  \brief Get the event's type.
 *
 *  Returns the type of this event (Event::HOSTGROUP). This can be useful for
 *  runtime event type identification.
 *
 *  \return Event::HOSTGROUP
 */
int HostGroup::GetType() const
{
  return (Event::HOSTGROUP);
}
