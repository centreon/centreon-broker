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

#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_event::kpi_event()
    : kpi_id(0),
      impact_level(0),
      in_downtime(false),
      status(kpi_event::state::state_unknown), ba_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
kpi_event::kpi_event(kpi_event const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi_event::~kpi_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
kpi_event& kpi_event::operator=(kpi_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test.
 *
 *  @return  True if the two objects are equal.
 */
bool kpi_event::operator==(kpi_event const& other) const {
  return ((end_time == other.end_time) && (kpi_id == other.kpi_id) &&
          (impact_level == other.impact_level) &&
          (in_downtime == other.in_downtime) && (output == other.output) &&
          (perfdata == other.perfdata) && (start_time == other.start_time) &&
          (status == other.status) && (ba_id == other.ba_id));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
uint32_t kpi_event::type() const {
  return (kpi_event::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
uint32_t kpi_event::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_kpi_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void kpi_event::_internal_copy(kpi_event const& other) {
  end_time = other.end_time;
  kpi_id = other.kpi_id;
  impact_level = other.impact_level;
  in_downtime = other.in_downtime;
  output = other.output;
  perfdata = other.perfdata;
  start_time = other.start_time;
  status = other.status;
  ba_id = other.ba_id;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const kpi_event::entries[] = {
    mapping::entry(&bam::kpi_event::kpi_id,
                   "kpi_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::kpi_event::end_time, "end_time"),
    mapping::entry(&bam::kpi_event::impact_level, "impact_level"),
    mapping::entry(&bam::kpi_event::in_downtime, "in_downtime"),
    mapping::entry(&bam::kpi_event::output, "first_output"),
    mapping::entry(&bam::kpi_event::perfdata, "first_perfdata"),
    mapping::entry(&bam::kpi_event::start_time, "start_time"),
    mapping::entry(&bam::kpi_event::status, "status"),
    mapping::entry()};

// Operations.
static io::data* new_kpi_event() {
  return (new kpi_event);
}
io::event_info::event_operations const kpi_event::operations = {&new_kpi_event};
