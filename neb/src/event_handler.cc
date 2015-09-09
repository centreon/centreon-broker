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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/event_handler.hh"
#include "com/centreon/broker/neb/internal.hh"

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
 *  @param[in] eh Object to copy.
 */
event_handler::event_handler(event_handler const& eh) : io::data(eh) {
  _internal_copy(eh);
}

/**
 *  Destructor.
 */
event_handler::~event_handler() {}

/**
 *  Assignment operator.
 *
 *  @param[in] eh Object to copy.
 *
 *  @return This object.
 */
event_handler& event_handler::operator=(event_handler const& eh) {
  io::data::operator=(eh);
  _internal_copy(eh);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int event_handler::type() const {
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
 *  @param[in] eh Object to copy.
 */
void event_handler::_internal_copy(event_handler const& eh) {
  command_args = eh.command_args;
  command_line = eh.command_line;
  early_timeout = eh.early_timeout;
  end_time = eh.end_time;
  execution_time = eh.execution_time;
  handler_type = eh.handler_type;
  host_id = eh.host_id;
    output = eh.output;
  return_code = eh.return_code;
  service_id = eh.service_id;
  start_time = eh.start_time;
  state = eh.state;
  state_type = eh.state_type;
  timeout = eh.timeout;
  return ;
}
