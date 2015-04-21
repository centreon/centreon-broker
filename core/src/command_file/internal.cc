/*
** Copyright 2013,2015 Merethis
**
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

#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/command_file/internal.hh"
#include "com/centreon/broker/command_file/factory.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::command_file;


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
void command_file::load() {
  io::events& e(io::events::instance());

  // Register command_file protocol.
  io::protocols::instance().reg(
                              "command_file",
                              command_file::factory(),
                              1,
                              7);

  e.register_category("internal", io::events::internal);

  // Register event.
  e.register_event(
    io::events::internal,
    io::events::de_command,
      io::event_info(
            "command",
            &external_command::operations,
            external_command::entries));

  return ;
}

/**
 *  @brief Command file cleanup routine.
 *
 *  Delete the command file endpoint.
 */
void command_file::unload() {
  // Unregister protocol.
  io::protocols::instance().unreg("command_file");

  return ;
}
