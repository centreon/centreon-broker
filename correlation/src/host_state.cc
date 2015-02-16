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
#include "com/centreon/broker/correlation/host_state.hh"
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
host_state::host_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hs Object to copy.
 */
host_state::host_state(host_state const& hs) : state(hs) {
  service_id = 0;
}

/**
 *  Destructor.
 */
host_state::~host_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This instance.
 */
host_state& host_state::operator=(host_state const& hs) {
  state::operator=(hs);
  service_id = 0;
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] hs Object to compare to.
 *
 *  @return true if both objects are equal.
 */
bool host_state::operator==(host_state const& hs) const {
  return ((this == &hs)
          || ((ack_time == hs.ack_time)
              && (current_state == hs.current_state)
              && (end_time == hs.end_time)
              && (host_id == hs.host_id)
              && (in_downtime == hs.in_downtime)
              && (start_time == hs.start_time)));
}

/**
 *  Non-equality operator.
 *
 *  @param[in] hs Object to compare to.
 *
 *  @return true if both objects are not equal.
 */
bool host_state::operator!=(host_state const& hs) const {
  return (!this->operator==(hs));
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_state::type() const {
  return (io::events::data_type<io::events::correlation, correlation::de_host_state>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host_state::entries[] = {
  mapping::entry(
    &host_state::ack_time,
    "ack_time",
    1,
    mapping::entry::NULL_ON_MINUS_ONE),
  mapping::entry(
    &host_state::current_state,
    "state",
    2),
  mapping::entry(
    &host_state::end_time,
    "end_time",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_state::host_id,
    "host_id",
    4),
  mapping::entry(
    &host_state::in_downtime,
    "in_downtime",
    5),
  mapping::entry(
    &host_state::start_time,
    "start_time",
    6),
  mapping::entry(
    &host_state::instance_id,
    "",
    7),
  mapping::entry()
};

// Operations.
static io::data* new_host_state() {
  return (new host_state);
}
io::event_info::event_operations const host_state::operations = {
  &new_host_state
};
