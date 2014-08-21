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
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/loaders/escalation_loader.hh"

using namespace com::centreon::broker::notification;

escalation_loader::escalation_loader() {}

void escalation_loader::load(QSqlDatabase* db, escalation_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  if (!query.exec("SELECT esc_id, esc_name, esc_alias, first_notification, last_notification, notification_interval, escalation_period, escalation_options1, escalation_options2 from escalation"))
    throw (exceptions::msg()
      << "Notification: cannot select escalation in loader: "
      << query.lastError().text());

  while (query.next()) {
    shared_ptr<escalation> esc(new escalation);
    unsigned int id = query.value(0).toUInt();

    esc->set_first_notification(query.value(3).toUInt());
    esc->set_last_notification(query.value(4).toUInt());
    esc->set_notification_interval(query.value(5).toUInt());
    esc->set_escalation_period(query.value(6).toString().toStdString());

    output->add_escalation(id, esc);
  }
}
