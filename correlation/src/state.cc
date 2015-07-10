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

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/state.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
state::state()
  : ack_time(-1),
    current_state(3),
    end_time(-1),
    host_id(0),
    in_downtime(false),
    poller_id(0),
    service_id(0),
    start_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
state::state(state const& s) : io::data(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& s) {
  io::data::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  Comparison operator.
 *
 *  @param[in] s  Object to compare with.
 *
 *  @return       True of both objects are equal.
 */
bool state::operator==(state const& s) const {
  return ((this == &s)
          || ((ack_time == s.ack_time)
              && (current_state == s.current_state)
              && (end_time == s.end_time)
              && (host_id == s.host_id)
              && (in_downtime == s.in_downtime)
              && (service_id == s.service_id)
              && (start_time == s.start_time)));
}

/**
 *  Unequal operator.
 *
 *  @param[in] s  Object to compare.
 *
 *  @return       True if both objects are not equal.
 */
bool state::operator!=(state const& s) const {
  return (!(*this == s));
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int state::type() const {
  return (state::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int state::static_type() {
  return (io::events::data_type<io::events::correlation, correlation::de_state>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members from the given object.
 *
 *  @param[in] s Object to copy.
 */
void state::_internal_copy(state const& s) {
  ack_time = s.ack_time;
  current_state = s.current_state;
  end_time = s.end_time;
  host_id = s.host_id;
  in_downtime = s.in_downtime;
  poller_id = s.poller_id;
  service_id = s.service_id;
  start_time = s.start_time;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const state::entries[] = {
  mapping::entry(
    &state::ack_time,
    "ack_time",
    mapping::entry::invalid_on_minus_one),
  mapping::entry(
    &state::current_state,
    "state"),
  mapping::entry(
    &state::end_time,
    "end_time",
    mapping::entry::invalid_on_minus_one),
  mapping::entry(
    &state::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &state::in_downtime,
    "in_downtime"),
  mapping::entry(
    &state::service_id,
    "service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &state::start_time,
    "start_time",
    mapping::entry::invalid_on_minus_one),
  mapping::entry()
};

// Operations.
static io::data* new_state() {
  return (new state);
}
io::event_info::event_operations const state::operations = {
  &new_state
};
