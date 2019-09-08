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

#include "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_timeperiod_exclusion::dimension_timeperiod_exclusion()
    : excluded_timeperiod_id(0), timeperiod_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_timeperiod_exclusion::dimension_timeperiod_exclusion(
    dimension_timeperiod_exclusion const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_timeperiod_exclusion::~dimension_timeperiod_exclusion() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_timeperiod_exclusion& dimension_timeperiod_exclusion::operator=(
    dimension_timeperiod_exclusion const& other) {
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
unsigned int dimension_timeperiod_exclusion::type() const {
  return (dimension_timeperiod_exclusion::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_timeperiod_exclusion::static_type() {
  return (io::events::data_type<io::events::bam,
                                bam::de_dimension_timeperiod_exclusion>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void dimension_timeperiod_exclusion::_internal_copy(
    dimension_timeperiod_exclusion const& other) {
  excluded_timeperiod_id = other.excluded_timeperiod_id;
  timeperiod_id = other.timeperiod_id;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_timeperiod_exclusion::entries[] = {
    mapping::entry(&bam::dimension_timeperiod_exclusion::timeperiod_id,
                   "timeperiod_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_timeperiod_exclusion::excluded_timeperiod_id,
                   "excluded_timeperiod_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_dimension_timeperiod_exclusion() {
  return (new dimension_timeperiod_exclusion);
}
io::event_info::event_operations const
    dimension_timeperiod_exclusion::operations = {
        &new_dimension_timeperiod_exclusion};
