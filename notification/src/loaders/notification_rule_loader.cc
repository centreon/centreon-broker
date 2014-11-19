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
#include "com/centreon/broker/notification/loaders/notification_rule_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_rule_loader::notification_rule_loader() {}

void notification_rule_loader::load(
                                 QSqlDatabase *db,
                                 notification_rule_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT rule_id, method_id, timeperiod_id, contact_id, "
                  "       host_id, service_id FROM rt_notification_rules"))
    throw (exceptions::msg()
      << "Notification: cannot select rt_notification_rules in loader: "
      << query.lastError().text());

  while (query.next()) {
    notification_rule::ptr rule(new notification_rule);
    rule->set_id(query.value(0).toUInt());
    rule->set_method_id(query.value(1).toUInt());
    rule->set_timeperiod_id(query.value(2).toUInt());
    rule->set_contact_id(query.value(3).toUInt());
    rule->set_node_id(node_id(query.value(4).toUInt(),
                              query.value(5).toUInt()));
    output->add_rule(query.value(0).toUInt(), rule);
  }
}
