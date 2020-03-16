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

#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_ba_timeperiod_relation::dimension_ba_timeperiod_relation()
    : io::data(dimension_ba_timeperiod_relation::static_type()),
      ba_id(0),
      timeperiod_id(0),
      is_default(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_ba_timeperiod_relation::dimension_ba_timeperiod_relation(
    dimension_ba_timeperiod_relation const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_ba_timeperiod_relation::~dimension_ba_timeperiod_relation() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_ba_timeperiod_relation& dimension_ba_timeperiod_relation::operator=(
    dimension_ba_timeperiod_relation const& other) {
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
bool dimension_ba_timeperiod_relation::operator==(
    dimension_ba_timeperiod_relation const& other) const {
  return ((ba_id == other.ba_id) && (timeperiod_id == other.timeperiod_id) &&
          (is_default == other.is_default));
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_ba_timeperiod_relation::_internal_copy(
    dimension_ba_timeperiod_relation const& other) {
  ba_id = other.ba_id;
  timeperiod_id = other.timeperiod_id;
  is_default = other.is_default;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_ba_timeperiod_relation::entries[] = {
    mapping::entry(&bam::dimension_ba_timeperiod_relation::ba_id,
                   "ba_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_ba_timeperiod_relation::timeperiod_id,
                   "timeperiod_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_ba_timeperiod_relation::is_default,
                   "is_default"),
    mapping::entry()};

// Operations.
static io::data* new_dimension_ba_timeperiod_relation() {
  return new dimension_ba_timeperiod_relation;
}
io::event_info::event_operations const
    dimension_ba_timeperiod_relation::operations = {
        &new_dimension_ba_timeperiod_relation};
