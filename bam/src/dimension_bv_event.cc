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

#include "com/centreon/broker/bam/dimension_bv_event.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_bv_event::dimension_bv_event()
    : io::data(dimension_bv_event::static_type()), bv_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_bv_event::dimension_bv_event(dimension_bv_event const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_bv_event::~dimension_bv_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_bv_event& dimension_bv_event::operator=(
    dimension_bv_event const& other) {
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
bool dimension_bv_event::operator==(dimension_bv_event const& other) const {
  return ((bv_id == other.bv_id) && (bv_name == other.bv_name) &&
          (bv_description == other.bv_description));
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_bv_event::_internal_copy(dimension_bv_event const& other) {
  bv_id = other.bv_id;
  bv_name = other.bv_name;
  bv_description = other.bv_description;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const dimension_bv_event::entries[] = {
    mapping::entry(&bam::dimension_bv_event::bv_id,
                   "bv_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::dimension_bv_event::bv_name, "bv_name"),
    mapping::entry(&bam::dimension_bv_event::bv_description, "bv_description"),
    mapping::entry()};

// Operations.
static io::data* new_dimension_bv_event() {
  return new dimension_bv_event;
}
io::event_info::event_operations const dimension_bv_event::operations = {
    &new_dimension_bv_event};
