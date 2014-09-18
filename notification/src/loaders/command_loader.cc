/*
** Copyright 2011-2013 Merethis
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

#include <QVariant>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
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

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT command_id, connector_id, command_name, command_line,"
                  "command_example, command_type, enable_shell,"
                  "command_comment, graph_id, cmd_cat_id"
                  " FROM command"))
    throw (exceptions::msg()
      << "Notification: cannot select command in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    std::string base_command = query.value(3).toString().toStdString();
    shared_ptr<command> com(new command(base_command));
    com->set_name(query.value(2).toString().toStdString());

    output->add_command(id, com);
  }
}
