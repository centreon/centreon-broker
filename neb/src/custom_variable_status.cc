/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/neb/custom_variable_status.hh"

#include "com/centreon/broker/database/table_max_size.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
custom_variable_status::custom_variable_status(uint32_t type)
    : io::data(type),
      host_id(0),
      modified(true),
      service_id(0),
      update_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
custom_variable_status::custom_variable_status(
    custom_variable_status const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
custom_variable_status::~custom_variable_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
custom_variable_status& custom_variable_status::operator=(
    custom_variable_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void custom_variable_status::_internal_copy(
    custom_variable_status const& other) {
  host_id = other.host_id;
  modified = other.modified;
  name = other.name;
  service_id = other.service_id;
  update_time = other.update_time;
  value = other.value;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const custom_variable_status::entries[] = {
    mapping::entry(&custom_variable_status::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&custom_variable_status::modified, "modified"),
    mapping::entry(&custom_variable_status::name,
                   "name",
                   get_customvariables_col_size(customvariables_name)),
    mapping::entry(&custom_variable_status::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&custom_variable_status::update_time,
                   "update_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&custom_variable_status::value,
                   "value",
                   get_customvariables_col_size(customvariables_value)),
    mapping::entry()};

// Operations.
static io::data* new_custom_var_status() {
  return new custom_variable_status;
}
io::event_info::event_operations const custom_variable_status::operations = {
    &new_custom_var_status};
