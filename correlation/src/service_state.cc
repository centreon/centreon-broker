/*
** Copyright 2009-2013 Merethis
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

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_state::service_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
service_state::service_state(service_state const& ss) : state(ss) {}

/**
 *  Destructor.
 */
service_state::~service_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_state& service_state::operator=(service_state const& ss) {
  state::operator=(ss);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are equal.
 */
bool service_state::operator==(service_state const& ss) const {
  return ((this == &ss)
          || ((ack_time == ss.ack_time)
              && (current_state == ss.current_state)
              && (end_time == ss.end_time)
              && (host_id == ss.host_id)
              && (in_downtime == ss.in_downtime)
              && (service_id == ss.service_id)
              && (start_time == ss.start_time)));
}

/**
 *  Non-equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are not equal.
 */
bool service_state::operator!=(service_state const& ss) const {
  return (!this->operator==(ss));
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int service_state::type() const {
  return (io::events::data_type<io::events::correlation, correlation::de_service_state>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const service_state::entries[] = {
  mapping::entry(
    &service_state::ack_time,
    "ack_time",
    1,
    mapping::entry::NULL_ON_MINUS_ONE),
  mapping::entry(
    &service_state::current_state,
    "state",
    2),
  mapping::entry(
    &service_state::end_time,
    "end_time",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_state::host_id,
    "host_id",
    4),
  mapping::entry(
    &service_state::in_downtime,
    "in_downtime",
    5),
  mapping::entry(
    &service_state::service_id,
    "service_id",
    6),
  mapping::entry(
    &service_state::start_time,
    "start_time",
    7),
  mapping::entry(
    &service_state::instance_id,
    "",
    8),
  mapping::entry()
};

// Operations.
static io::data* new_service_state() {
  return (new service_state);
}
io::event_info::event_operations const service_state::operations = {
  &new_service_state
};
