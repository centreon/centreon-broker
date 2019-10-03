/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/correlation/state.hh"
#include "com/centreon/broker/correlation/internal.hh"
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
  return ((this == &s) ||
          ((ack_time == s.ack_time) && (current_state == s.current_state) &&
           (end_time == s.end_time) && (host_id == s.host_id) &&
           (in_downtime == s.in_downtime) && (service_id == s.service_id) &&
           (start_time == s.start_time)));
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
uint32_t state::type() const {
  return (state::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
uint32_t state::static_type() {
  return (io::events::data_type<io::events::correlation,
                                correlation::de_state>::value);
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
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const state::entries[] = {
    mapping::entry(&state::ack_time,
                   "ack_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&state::current_state, "state"),
    mapping::entry(&state::end_time,
                   "end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&state::host_id, "host_id", mapping::entry::invalid_on_zero),
    mapping::entry(&state::in_downtime, "in_downtime"),
    mapping::entry(&state::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&state::start_time,
                   "start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry()};

// Operations.
static io::data* new_state() {
  return (new state);
}
io::event_info::event_operations const state::operations = {&new_state};
