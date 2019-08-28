/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/objects/command.hh"
#include "com/centreon/broker/notification/loaders/command_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

command_loader::command_loader() {}

/**
 *  Load the commands from the database.
 *
 *  @param[in] ms       An open connection to the database.
 * @param[out] output   A command builder object to register the commands.
 */
void command_loader::load(mysql* ms, command_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading commands from the database";

  // Performance improvement, as we never go back.
  //query.setForwardOnly(true);

  // Load the commands
  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result(
        "SELECT command_id, connector_id, command_name, command_line,"
        "       command_type, enable_shell"
        "  FROM cfg_commands",
        &promise);
  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      unsigned int id = res.value_as_u32(0);
      std::string base_command = res.value_as_str(3);
      command::ptr com(new command(base_command));
      com->set_name(res.value_as_str(2));
      com->set_enable_shell(res.value_as_bool(5));

      output->add_command(id, com);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load commands from database: "
      << e.what();
  }
}
