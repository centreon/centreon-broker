/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
command_result::command_result() : code(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
command_result::command_result(command_result const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
command_result::~command_result() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
command_result& command_result::operator=(command_result const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
uint32_t command_result::type() const {
  return (command_result::static_type());
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
uint32_t command_result::static_type() {
  return (io::events::data_type<io::events::extcmd,
                                io::events::de_command_result>::value);
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
void command_result::_internal_copy(command_result const& other) {
  code = other.code;
  uuid = other.uuid;
  msg = other.msg;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const command_result::entries[] = {
    mapping::entry(&command_result::code, "code"),
    mapping::entry(&command_result::uuid,
                   "uuid",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&command_result::msg, "msg"), mapping::entry()};

// Operations.
static io::data* new_command_result() {
  return (new command_result);
}
io::event_info::event_operations const command_result::operations = {
    &new_command_result};
