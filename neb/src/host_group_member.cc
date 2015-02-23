/*
** Copyright 2009-2013,2015 Merethis
**
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
*/

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/host_group_member.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

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
 *  @param[in] other  Object to copy.
 */
host_group_member::host_group_member(host_group_member const& other)
  : group_member(other) {}

/**
 *  Destructor.
 */
host_group_member::~host_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_group_member& host_group_member::operator=(
                                        host_group_member const& other) {
  group_member::operator=(other);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_group_member::type() const {
  return (host_group_member::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int host_group_member::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_host_group_member>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host_group_member::entries[] = {
  mapping::entry(
    &host_group_member::enabled,
    "",
    1),
  mapping::entry(
    &host_group_member::group, // XXX : should be replaced by hostgroup_id
    "group",
    2),
  mapping::entry(
    &host_group_member::instance_id,
    "instance_id",
    3),
  mapping::entry(
    &host_group_member::host_id,
    "host_id",
    4,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry()
};

// Operations.
static io::data* new_hgm() {
  return (new host_group_member);
}
io::event_info::event_operations const host_group_member::operations = {
  &new_hgm
};
