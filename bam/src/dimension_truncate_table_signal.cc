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

#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_truncate_table_signal::dimension_truncate_table_signal()
    : io::data(dimension_truncate_table_signal::static_type()),
      update_started(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_truncate_table_signal::dimension_truncate_table_signal(
    dimension_truncate_table_signal const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_truncate_table_signal::~dimension_truncate_table_signal() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_truncate_table_signal& dimension_truncate_table_signal::operator=(
    dimension_truncate_table_signal const& other) {
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
bool dimension_truncate_table_signal::operator==(
    dimension_truncate_table_signal const& other) const {
  return update_started == other.update_started;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_truncate_table_signal::_internal_copy(
    dimension_truncate_table_signal const& other) {
  update_started = other.update_started;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_truncate_table_signal::entries[] = {
    mapping::entry(&bam::dimension_truncate_table_signal::update_started,
                   "update_started"),
    mapping::entry()};

// Operations.
static io::data* new_dimension_truncate_table_signal() {
  return new dimension_truncate_table_signal;
}
io::event_info::event_operations const
    dimension_truncate_table_signal::operations = {
        &new_dimension_truncate_table_signal};
