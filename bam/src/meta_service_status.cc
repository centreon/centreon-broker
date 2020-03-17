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

#include "com/centreon/broker/bam/meta_service_status.hh"

#include <cmath>

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
meta_service_status::meta_service_status()
    : io::data(meta_service_status::static_type()),
      meta_service_id(0),
      state_changed(false),
      value(NAN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
meta_service_status::meta_service_status(meta_service_status const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
meta_service_status::~meta_service_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
meta_service_status& meta_service_status::operator=(
    meta_service_status const& other) {
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
void meta_service_status::_internal_copy(meta_service_status const& other) {
  meta_service_id = other.meta_service_id;
  state_changed = other.state_changed;
  value = other.value;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const meta_service_status::entries[] = {
    mapping::entry(&bam::meta_service_status::meta_service_id,
                   "meta_service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::meta_service_status::value, "value"),
    mapping::entry(&bam::meta_service_status::state_changed, ""),
    mapping::entry()};

// Operations.
static io::data* new_meta_service_status() {
  return new meta_service_status;
}
io::event_info::event_operations const meta_service_status::operations = {
    &new_meta_service_status};
