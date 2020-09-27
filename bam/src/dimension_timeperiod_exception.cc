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

#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"

#include "com/centreon/broker/database/table_max_size.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_timeperiod_exception::dimension_timeperiod_exception()
    : io::data(dimension_timeperiod_exception::static_type()),
      timeperiod_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_timeperiod_exception::dimension_timeperiod_exception(
    dimension_timeperiod_exception const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_timeperiod_exception::~dimension_timeperiod_exception() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_timeperiod_exception& dimension_timeperiod_exception::operator=(
    dimension_timeperiod_exception const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void dimension_timeperiod_exception::_internal_copy(
    dimension_timeperiod_exception const& other) {
  daterange = other.daterange;
  timerange = other.timerange;
  timeperiod_id = other.timeperiod_id;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_timeperiod_exception::entries[] = {
    mapping::entry(&bam::dimension_timeperiod_exception::timeperiod_id,
                   "timeperiod_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_timeperiod_exception::daterange,
                   "daterange",
                   get_mod_bam_reporting_timeperiods_exceptions_col_size(
                       mod_bam_reporting_timeperiods_exceptions_daterange)),
    mapping::entry(&bam::dimension_timeperiod_exception::timerange,
                   "timerange",
                   get_mod_bam_reporting_timeperiods_exceptions_col_size(
                       mod_bam_reporting_timeperiods_exceptions_timerange)),
    mapping::entry()};

// Operations.
static io::data* new_dimension_timeperiod_exception() {
  return new dimension_timeperiod_exception;
}
io::event_info::event_operations const
    dimension_timeperiod_exception::operations = {
        &new_dimension_timeperiod_exception};
