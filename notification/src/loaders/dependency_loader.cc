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
#include "com/centreon/broker/notification/objects/dependency.hh"
#include "com/centreon/broker/notification/loaders/dependency_loader.hh"

using namespace com::centreon::broker::notification;

dependency_loader::dependency_loader() {}

void dependency_loader::load(QSqlDatabase* db, dependency_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  if (!query.exec("SELECT dep_id, dep_name, dep_description, inherits_parent, execution_failure_criteria, notification_failure_criteria from dependency"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency in loader: "
      << query.lastError().text());

  while (query.next()) {
    shared_ptr<dependency> dep(new dependency);
    unsigned int id = query.value(0).toUInt();
    dep->set_inherits_parent(query.value(3).toBool());


    output->add_dependency(id, dep);
  }

}
