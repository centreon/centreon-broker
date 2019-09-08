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

#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
ba_status::ba_status()
    : ba_id(0),
      in_downtime(false),
      level_acknowledgement(0.0),
      level_downtime(0.0),
      level_nominal(100.0),
      state(0),
      state_changed(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
ba_status::ba_status(ba_status const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba_status::~ba_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
ba_status& ba_status::operator=(ba_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int ba_status::type() const {
  return (ba_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int ba_status::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_ba_status>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void ba_status::_internal_copy(ba_status const& other) {
  ba_id = other.ba_id;
  in_downtime = other.in_downtime;
  last_state_change = other.last_state_change;
  level_acknowledgement = other.level_acknowledgement;
  level_downtime = other.level_downtime;
  level_nominal = other.level_nominal;
  state = other.state;
  state_changed = other.state_changed;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const ba_status::entries[] = {
    mapping::entry(&bam::ba_status::ba_id,
                   "ba_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::ba_status::in_downtime, "in_downtime"),
    mapping::entry(&bam::ba_status::last_state_change, "last_state_change"),
    mapping::entry(&bam::ba_status::level_acknowledgement,
                   "level_acknowledgement"),
    mapping::entry(&bam::ba_status::level_downtime, "level_downtime"),
    mapping::entry(&bam::ba_status::level_nominal, "level_nominal"),
    mapping::entry(&bam::ba_status::state, "state"),
    mapping::entry(&bam::ba_status::state_changed, "state_changed"),
    mapping::entry()};

// Operations.
static io::data* new_ba_status() {
  return (new ba_status);
}
io::event_info::event_operations const ba_status::operations = {&new_ba_status};
