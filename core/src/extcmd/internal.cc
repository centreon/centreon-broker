/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/extcmd/factory.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;


/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

/**
 *  @brief Command file initialization routine.
 *
 *  Initialize the command file endpoint.
 */
void extcmd::load() {
  io::events& e(io::events::instance());

  // Register extcmd protocol.
  io::protocols::instance().reg(
                              "extcmd",
                              extcmd::factory(),
                              1,
                              7);

  // Register event.
  e.register_event(
    io::events::internal,
    io::events::de_command_request,
      io::event_info(
            "command_request",
            &command_request::operations,
            command_request::entries));

  return ;
}

/**
 *  @brief Command file cleanup routine.
 *
 *  Delete the command file endpoint.
 */
void extcmd::unload() {
  // Unregister protocol.
  io::protocols::instance().unreg("extcmd");

  return ;
}
