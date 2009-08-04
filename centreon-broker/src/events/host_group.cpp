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

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal members of the given object to the current instance.
 *
 *  This internal method is used to copy data defined inside the HostGroup
 *  class from an object to the current instance. This means that no superclass
 *  data are copied. This method is used in HostGroup copy constructor and in
 *  the = operator overload.
 *
 *  \param[in] host_group Object to copy from.
 *
 *  \see HostGroup(const HostGroup&)
 *  \see operator=
 */
void HostGroup::InternalCopy(const HostGroup& host_group)
{
  this->action_url = host_group.action_url;
  this->alias      = host_group.alias;
  this->name       = host_group.name;
  this->notes      = host_group.notes;
  this->notes_url  = host_group.notes_url;
  return ;
}

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
 *  Copy all members of the HostGroup object to the current instance.
 *
 *  \param[in] host_group Object to copy from.
 */
HostGroup::HostGroup(const HostGroup& host_group) : Event(host_group)
{
  this->InternalCopy(host_group);
}

/**
 *  HostGroup destructor.
 */
HostGroup::~HostGroup() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the HostGroup object to the current instance.
 *
 *  \param[in] host_group Object to copy from.
 *
 *  \return *this
 */
HostGroup& HostGroup::operator=(const HostGroup& host_group)
{
  this->Event::operator=(host_group);
  this->InternalCopy(host_group);
  return (*this);
}

/**
 *  \brief Returns the type of this events (CentreonBroker::Event::HOSTGROUP).
 *
 *  This method can be used for runtime event type identification.
 *
 *  \return CentreonBroker::Event::HOSTGROUP
 *
 *  \see CentreonBroker::Event
 */
int HostGroup::GetType() const throw ()
{
  return (Event::HOSTGROUP);
}
