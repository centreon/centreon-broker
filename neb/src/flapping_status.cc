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

#include "com/centreon/broker/neb/flapping_status.hh"
#include "com/centreon/broker/io/events.hh"
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
flapping_status::flapping_status()
    : event_time(0),
      event_type(0),
      flapping_type(0),
      high_threshold(0),
      host_id(0),
      low_threshold(0),
      percent_state_change(0),
      reason_type(0),
      service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
flapping_status::flapping_status(flapping_status const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
flapping_status::~flapping_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
flapping_status& flapping_status::operator=(flapping_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int flapping_status::type() const {
  return (flapping_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int flapping_status::static_type() {
  return (
      io::events::data_type<io::events::neb, neb::de_flapping_status>::value);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void flapping_status::_internal_copy(flapping_status const& other) {
  event_time = other.event_time;
  event_type = other.event_type;
  flapping_type = other.flapping_type;
  high_threshold = other.high_threshold;
  host_id = other.host_id;
  low_threshold = other.low_threshold;
  percent_state_change = other.percent_state_change;
  reason_type = other.reason_type;
  service_id = other.service_id;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const flapping_status::entries[] = {
    mapping::entry(&flapping_status::event_time, "event_time"),
    mapping::entry(&flapping_status::event_type, "event_type"),
    mapping::entry(&flapping_status::flapping_type, "type"),
    mapping::entry(&flapping_status::high_threshold, "high_threshold"),
    mapping::entry(&flapping_status::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&flapping_status::low_threshold, "low_threshold"),
    mapping::entry(&flapping_status::percent_state_change,
                   "percent_state_change"),
    mapping::entry(&flapping_status::reason_type, "reason_type"),
    mapping::entry(&flapping_status::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_flapping() {
  return (new flapping_status);
}
io::event_info::event_operations const flapping_status::operations = {
    &new_flapping};
