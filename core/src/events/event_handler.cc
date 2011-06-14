/*
** Copyright 2009-2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "events/event_handler.hh"

using namespace com::centreon::broker::events;

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

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
event_handler::event_handler()
  : early_timeout(0),
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
event_handler::event_handler(event_handler const& eh) : event(eh) {
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
  event::operator=(eh);
  _internal_copy(eh);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return EVENTHANDLER.
 */
unsigned int event_handler::type() const {
  return (EVENTHANDLER);
}
