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
#include "com/centreon/broker/notification/loaders/node_loader.hh"

using namespace com::centreon::broker::notification;

node_loader::node_loader() {

}

void node_loader::load(QSqlDatabase* db, node_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Instead of doing crosses select from host_id and service_id,
  // we only do three selects and internally do the connexions. It's faster.

  if (!query.exec("SELECT host_id from host"))
    throw (exceptions::msg()
      << "Notification: cannot select host in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();

    output->add_host(id);
  }

  if (!query.exec("SELECT service_id from service"))
    throw (exceptions::msg()
      << "Notification: cannot select host in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();

    output->add_service(id);
  }

  if (!query.exec("SELECT host_host_id, service_service_id from host_service_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select host_service_relation in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int host_id = query.value(0).toUInt();
    unsigned int service_id = query.value(0).toUInt();

    output->connect_service_host(host_id, service_id);
  }

}
