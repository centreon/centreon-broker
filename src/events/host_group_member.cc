/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include "events/host_group_member.hh"

using namespace events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
host_group_member::host_group_member() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hgm Object to copy.
 */
host_group_member::host_group_member(host_group_member const& hgm)
  : group_member(hgm) {}

/**
 *  Destructor.
 */
host_group_member::~host_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] hgm Object to copy.
 *
 *  @return This object.
 */
host_group_member& host_group_member::operator=(host_group_member const& hgm) {
  group_member::operator=(hgm);
  return (*this);
}

/**
 *  Get the type of this event (event::HOSTGROUPMEMBER).
 *
 *  @return event::HOSTGROUPMEMBER
 */
int host_group_member::get_type() const {
  return (HOSTGROUPMEMBER);
}
