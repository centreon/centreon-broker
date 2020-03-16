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

#include "com/centreon/broker/bam/dimension_ba_event.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_ba_event::dimension_ba_event()
    : io::data(dimension_ba_event::static_type()),
      ba_id(0),
      sla_month_percent_crit(0),
      sla_month_percent_warn(0),
      sla_duration_crit(0),
      sla_duration_warn(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_ba_event::dimension_ba_event(dimension_ba_event const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_ba_event::~dimension_ba_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_ba_event& dimension_ba_event::operator=(
    dimension_ba_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool dimension_ba_event::operator==(dimension_ba_event const& other) const {
  return ba_id == other.ba_id;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_ba_event::_internal_copy(dimension_ba_event const& other) {
  ba_id = other.ba_id;
  ba_name = other.ba_name;
  ba_description = other.ba_description;
  sla_month_percent_crit = other.sla_month_percent_crit;
  sla_month_percent_warn = other.sla_month_percent_warn;
  sla_duration_crit = other.sla_duration_crit;
  sla_duration_warn = other.sla_duration_warn;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_ba_event::entries[] = {
    mapping::entry(&bam::dimension_ba_event::ba_id,
                   "ba_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_ba_event::ba_name, "ba_name"),
    mapping::entry(&bam::dimension_ba_event::ba_description, "ba_description"),
    mapping::entry(&bam::dimension_ba_event::sla_month_percent_crit,
                   "sla_month_percent_crit"),
    mapping::entry(&bam::dimension_ba_event::sla_month_percent_warn,
                   "sla_month_percent_warn"),
    mapping::entry(&bam::dimension_ba_event::sla_duration_crit,
                   "sla_month_duration_crit"),
    mapping::entry(&bam::dimension_ba_event::sla_duration_warn,
                   "sla_month_duration_warn"),
    mapping::entry()};

// Operations.
static io::data* new_dimension_ba_event() {
  return new dimension_ba_event;
}
io::event_info::event_operations const dimension_ba_event::operations = {
    &new_dimension_ba_event};
