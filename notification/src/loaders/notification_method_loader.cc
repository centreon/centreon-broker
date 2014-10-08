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

#include <sstream>
#include <QVariant>
#include <QSet>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/objects/notification_method.hh"
#include "com/centreon/broker/notification/loaders/notification_method_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_method_loader::notification_method_loader() {}

void notification_method_loader::load(QSqlDatabase *db, notification_method_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT method_id, name, command_id, interval, status, "
                  "types, start, end FROM cfg_configuration_method"))
    throw (exceptions::msg()
      << "Notification: cannot select cfg_configuration_method in loader: "
      << query.lastError().text());

  while (query.next()) {
    notification_method::ptr nm(new notification_method);
    nm->set_name(query.value(1).toString().toStdString());
    nm->set_command_id(query.value(2).toUInt());
    nm->set_interval(query.value(3).toUInt());
    nm->set_status(query.value(4).toString().toStdString());
    nm->set_types(query.value(5).toString().toStdString());
    nm->set_start(query.value(6).toUInt());
    nm->set_end(query.value(7).toUInt());
    output->add_notification_method(query.value(0).toUInt(), nm);
  }

}
