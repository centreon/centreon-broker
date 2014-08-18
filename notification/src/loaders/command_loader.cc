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
#include "com/centreon/broker/notification/builders/command_builder.hh"
#include "com/centreon/broker/notification/loaders/command_loader.hh"

using namespace com::centreon::broker::notification;

command_loader::command_loader() {}

void command_loader::load(QSqlDatabase* db, command_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  if (!query.exec("SELECT command_id, connector_id, command_name, command_line, command_example, command_type, enable_shell, command_comment, graph_id, cmd_cat_id from command"))
    throw (exceptions::msg()
      << "Notification: cannot select command in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    std::string base_command = query.value(4).toString().toStdString();
    shared_ptr<command> com(new command(base_command));

    output->add_command(id, com);
  }
}
