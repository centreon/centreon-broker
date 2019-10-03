/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
ba_event::ba_event()
    : ba_id(0), first_level(0), in_downtime(false), status(3) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_event::ba_event(ba_event const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba_event::~ba_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba_event& ba_event::operator=(ba_event const& other) {
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
bool ba_event::operator==(ba_event const& other) const {
  return ((ba_id == other.ba_id) && (first_level == other.first_level) &&
          (end_time == other.end_time) && (in_downtime == other.in_downtime) &&
          (start_time == other.start_time) && (status == other.status));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
uint32_t ba_event::type() const {
  return (ba_event::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
uint32_t ba_event::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_ba_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void ba_event::_internal_copy(ba_event const& other) {
  ba_id = other.ba_id;
  first_level = other.first_level;
  end_time = other.end_time;
  in_downtime = other.in_downtime;
  start_time = other.start_time;
  status = other.status;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const ba_event::entries[] = {
    mapping::entry(&bam::ba_event::ba_id,
                   "ba_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::ba_event::first_level, "first_level"),
    mapping::entry(&bam::ba_event::end_time, "end_time"),
    mapping::entry(&bam::ba_event::in_downtime, "in_downtime"),
    mapping::entry(&bam::ba_event::start_time, "start_time"),
    mapping::entry(&bam::ba_event::status, "status"),
    mapping::entry()};

// Operations.
static io::data* new_ba_event() {
  return (new ba_event);
}
io::event_info::event_operations const ba_event::operations = {&new_ba_event};
