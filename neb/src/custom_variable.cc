/*
** Copyright 2009-2015 Centreon
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

#include "com/centreon/broker/neb/custom_variable.hh"

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
custom_variable::custom_variable()
    : custom_variable_status(custom_variable::static_type()),
      enabled(true),
      var_type(0) {
  modified = false;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
custom_variable::custom_variable(custom_variable const& other)
    : custom_variable_status(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
custom_variable::~custom_variable() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
custom_variable& custom_variable::operator=(custom_variable const& other) {
  if (this != &other) {
    custom_variable_status::operator=(other);
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
void custom_variable::_internal_copy(custom_variable const& other) {
  default_value = other.default_value;
  enabled = other.enabled;
  var_type = other.var_type;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const custom_variable::entries[] = {
    mapping::entry(&custom_variable::enabled, nullptr),
    mapping::entry(&custom_variable::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&custom_variable::modified, "modified"),
    mapping::entry(&custom_variable::name, "name"),
    mapping::entry(&custom_variable::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&custom_variable::update_time,
                   "update_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&custom_variable::var_type, "type"),
    mapping::entry(&custom_variable::value, "value"),
    mapping::entry(&custom_variable::default_value, "default_value"),
    mapping::entry()};

// Operations.
static io::data* new_custom_var() {
  return new custom_variable;
}
io::event_info::event_operations const custom_variable::operations = {
    &new_custom_var};
