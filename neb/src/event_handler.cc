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

#include "com/centreon/broker/neb/event_handler.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"

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
event_handler::event_handler()
    : early_timeout(false),
      end_time(0),
      execution_time(0),
      handler_type(0),
      host_id(0),
      return_code(0),
      service_id(0),
      start_time(0),
      state(0),
      state_type(0),
      timeout(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
event_handler::event_handler(event_handler const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
event_handler::~event_handler() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
event_handler& event_handler::operator=(event_handler const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int event_handler::type() const {
  return (event_handler::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int event_handler::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_event_handler>::value);
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
void event_handler::_internal_copy(event_handler const& other) {
  command_args = other.command_args;
  command_line = other.command_line;
  early_timeout = other.early_timeout;
  end_time = other.end_time;
  execution_time = other.execution_time;
  handler_type = other.handler_type;
  host_id = other.host_id;
  output = other.output;
  return_code = other.return_code;
  service_id = other.service_id;
  start_time = other.start_time;
  state = other.state;
  state_type = other.state_type;
  timeout = other.timeout;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const event_handler::entries[] = {
    mapping::entry(&event_handler::early_timeout, "early_timeout"),
    mapping::entry(&event_handler::end_time,
                   "end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&event_handler::execution_time, "execution_time"),
    mapping::entry(&event_handler::handler_type, "type"),
    mapping::entry(&event_handler::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&event_handler::return_code, "return_code"),
    mapping::entry(&event_handler::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&event_handler::start_time,
                   "start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&event_handler::state, "state"),
    mapping::entry(&event_handler::state_type, "state_type"),
    mapping::entry(&event_handler::timeout, "timeout"),
    mapping::entry(&event_handler::command_args, "command_args"),
    mapping::entry(&event_handler::command_line, "command_line"),
    mapping::entry(&event_handler::output, "output"),
    mapping::entry()};

// Operations.
static io::data* new_event_handler() {
  return (new event_handler);
}
io::event_info::event_operations const event_handler::operations = {
    &new_event_handler};
