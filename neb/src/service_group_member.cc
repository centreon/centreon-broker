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
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_group_member.hh"

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
service_group_member::service_group_member() : service_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] sgm Object to copy.
 */
service_group_member::service_group_member(service_group_member const& sgm)
  : group_member(sgm), service_id(sgm.service_id) {}

/**
 *  Destructor.
 */
service_group_member::~service_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] sgm Object to copy.
 *
 *  @return This object.
 */
service_group_member& service_group_member::operator=(service_group_member const& sgm) {
  group_member::operator=(sgm);
  service_id = sgm.service_id;
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int service_group_member::type() const {
  return (service_group_member::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int service_group_member::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_service_group_member>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const service_group_member::entries[] = {
  mapping::entry(
    &service_group_member::enabled,
    ""),
  mapping::entry(
    &service_group_member::group, // XXX : should be replaced by servicegroup_id
    "group"),
  mapping::entry(
    &service_group_member::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &service_group_member::source_id,
    "instance_id",
    mapping::entry::invalid_on_zero,
    false),
  mapping::entry(
    &service_group_member::service_id,
    "service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry()
};

// Operations.
static io::data* new_service_group_member() {
  return (new service_group_member);
}
io::event_info::event_operations const service_group_member::operations = {
  &new_service_group_member
};
