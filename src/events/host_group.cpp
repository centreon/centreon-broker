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

#include <cstring>
#include "events/host_group.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  This internal method is used to copy data defined inside the HostGroup
 *  class from an object to the current instance. This means that no superclass
 *  data are copied. This method is used in HostGroup copy constructor and in
 *  the = operator overload.
 *
 *  \see HostGroup(const HostGroup&)
 *  \see operator=
 *
 *  \param[in] host_group Object to copy from.
 */
void HostGroup::InternalCopy(const HostGroup& host_group)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = host_group.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostGroup default constructor. Set all members to their default value (0,
 *  NULL or equivalent).
 */
HostGroup::HostGroup()
{
}

/**
 *  HostGroup copy constructor.
 *
 *  \param[in] host_group Object to copy from.
 */
HostGroup::HostGroup(const HostGroup& host_group) : Event(host_group)
{
  this->InternalCopy(host_group);
}

/**
 *  HostGroup destructor, release all acquired ressources.
 */
HostGroup::~HostGroup()
{
}

/**
 *  Overload of the = operator.
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
 *  XXX : need fix
 */
const std::string& HostGroup::GetActionUrl() const throw ()
{
  return (this->strings_[ACTION_URL]);
}

/**
 *  Get the alias of the host group.
 *
 *  \return The alias of the host group.
 */
const std::string& HostGroup::GetAlias() const throw ()
{
  return (this->strings_[ALIAS]);
}

/**
 *  Get the real name of the host group.
 *
 *  \return The real name of the host group.
 */
const std::string& HostGroup::GetHostGroupName() const throw ()
{
  return (this->strings_[HOST_GROUP_NAME]);
}

/**
 *  XXX : need fix
 */
const std::string& HostGroup::GetNotes() const throw ()
{
  return (this->strings_[NOTES]);
}

/**
 *  XXX : need fix
 */
const std::string& HostGroup::GetNotesUrl() const throw ()
{
  return (this->strings_[NOTES_URL]);
}

/**
 *  Returns the type of this events (CentreonBroker::Event::HOSTGROUP).
 *
 *  \see CentreonBroker::Event
 *
 *  \return CentreonBroker::Event::HOSTGROUP
 */
int HostGroup::GetType() const throw ()
{
  return (Event::HOSTGROUP);
}

/**
 *  XXX : need fix
 *
 *  \see GetActionUrl
 */
void HostGroup::SetActionUrl(const std::string& au)
{
  this->strings_[ACTION_URL] = au;
  return ;
}

/**
 *  Set the alias of the host group.
 *
 *  \see GetAlias
 *
 *  \param[in] a The alias of the host group.
 */
void HostGroup::SetAlias(const std::string& a)
{
  this->strings_[ALIAS] = a;
  return ;
}

/**
 *  Set the real name of the host group.
 *
 *  \see GetHostGroupName
 *
 *  \param[in] hgn The real name of the host group.
 */
void HostGroup::SetHostGroupName(const std::string& hgn)
{
  this->strings_[HOST_GROUP_NAME] = hgn;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetNotes
 */
void HostGroup::SetNotes(const std::string& n)
{
  this->strings_[NOTES] = n;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetNotesUrl
 */
void HostGroup::SetNotesUrl(const std::string& nu)
{
  this->strings_[NOTES_URL] = nu;
  return ;
}
