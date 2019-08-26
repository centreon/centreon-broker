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

#include <QSqlError>
#include <QVariant>
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
 *  @param[in] db       An open connection to the database.
 * @param[out] output   A command builder object to register the commands.
 */
void command_loader::load(QSqlDatabase* db, command_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading commands from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load the commands
  if (!query.exec("SELECT command_id, connector_id, command_name, command_line,"
                  "       command_type, enable_shell"
                  "  FROM cfg_commands"))
    throw (exceptions::msg()
           << "notification: cannot load commands from database: "
           << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    std::string base_command = query.value(3).toString().toStdString();
    command::ptr com(new command(base_command));
    com->set_name(query.value(2).toString().toStdString());
    com->set_enable_shell(query.value(5).toBool());

    output->add_command(id, com);
  }
}
