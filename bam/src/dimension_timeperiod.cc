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

#include "com/centreon/broker/bam/dimension_timeperiod.hh"

#include "com/centreon/broker/database/table_max_size.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_timeperiod::dimension_timeperiod()
    : io::data(dimension_timeperiod::static_type()) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_timeperiod::dimension_timeperiod(dimension_timeperiod const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_timeperiod::~dimension_timeperiod() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_timeperiod& dimension_timeperiod::operator=(
    dimension_timeperiod const& other) {
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
bool dimension_timeperiod::operator==(dimension_timeperiod const& other) const {
  return ((id == other.id) && (name == other.name) &&
          (monday == other.monday) && (tuesday == other.tuesday) &&
          (wednesday == other.wednesday) && (thursday == other.thursday) &&
          (friday == other.friday) && (saturday == other.saturday) &&
          (sunday == other.sunday));
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_timeperiod::_internal_copy(dimension_timeperiod const& other) {
  id = other.id;
  name = other.name;
  monday = other.monday;
  tuesday = other.tuesday;
  wednesday = other.wednesday;
  thursday = other.thursday;
  friday = other.friday;
  saturday = other.saturday;
  sunday = other.sunday;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_timeperiod::entries[] = {
    mapping::entry(&bam::dimension_timeperiod::id,
                   "tp_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_timeperiod::name,
                   "name",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_name)),
    mapping::entry(&bam::dimension_timeperiod::monday,
                   "monday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_monday)),
    mapping::entry(&bam::dimension_timeperiod::tuesday,
                   "tuesday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_tuesday)),
    mapping::entry(&bam::dimension_timeperiod::wednesday,
                   "wednesday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_wednesday)),
    mapping::entry(&bam::dimension_timeperiod::thursday,
                   "thursday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_thursday)),
    mapping::entry(&bam::dimension_timeperiod::friday,
                   "friday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_friday)),
    mapping::entry(&bam::dimension_timeperiod::saturday,
                   "saturday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_saturday)),
    mapping::entry(&bam::dimension_timeperiod::sunday,
                   "sunday",
                   get_mod_bam_reporting_timeperiods_col_size(
                       mod_bam_reporting_timeperiods_sunday)),
    mapping::entry()};

// Operations.
static io::data* new_dimension_timeperiod() {
  return new dimension_timeperiod;
}
io::event_info::event_operations const dimension_timeperiod::operations = {
    &new_dimension_timeperiod};
