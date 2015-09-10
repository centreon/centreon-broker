/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
ba_duration_event::ba_duration_event()
  : ba_id(0),
    duration(0),
    sla_duration(0),
    timeperiod_id(0),
    timeperiod_is_default(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_duration_event::ba_duration_event(ba_duration_event const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba_duration_event::~ba_duration_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba_duration_event& ba_duration_event::operator=(
    ba_duration_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool ba_duration_event::operator==(ba_duration_event const& other) const {
  return ((ba_id == other.ba_id)
          && (real_start_time == other.real_start_time)
          && (end_time == other.end_time)
          && (start_time == other.start_time)
          && (duration == other.duration)
          && (sla_duration == other.sla_duration)
          && (timeperiod_id == other.timeperiod_id)
          && (timeperiod_is_default == other.timeperiod_is_default));
}


/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int ba_duration_event::type() const {
  return (io::events::data_type<io::events::bam,
                                bam::de_ba_duration_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void ba_duration_event::_internal_copy(ba_duration_event const& other) {
  ba_id = other.ba_id;
  real_start_time = other.real_start_time;
  end_time = other.end_time;
  start_time = other.start_time;
  duration = other.duration;
  sla_duration = other.sla_duration;
  timeperiod_id = other.timeperiod_id;
  timeperiod_is_default = other.timeperiod_is_default;
  return ;
}
