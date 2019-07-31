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

#include <sstream>
#include <QSet>
#include <QSqlError>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
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

  logging::debug(logging::medium)
    << "notification: loading notification rules from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT rule_id, method_id, timeperiod_id, contact_id, "
                  "       host_id, service_id"
                  "  FROM rt_notification_rules"))
    throw (exceptions::msg()
           << "notification: cannot load notification rules from database: "
           << query.lastError().text());

  while (query.next()) {
    notification_rule::ptr rule(new notification_rule);
    rule->set_id(query.value(0).toUInt());
    rule->set_method_id(query.value(1).toUInt());
    rule->set_timeperiod_id(query.value(2).toUInt());
    rule->set_contact_id(query.value(3).toUInt());
    rule->set_node_id(node_id(query.value(4).toUInt(),
                              query.value(5).toUInt()));
    logging::debug(logging::low)
      << "notification: new rule " << rule->get_id()
      << " affecting node (" << rule->get_node_id().get_host_id()
      << ", " << rule->get_node_id().get_service_id()
      << ") using method " << rule->get_method_id();
    output->add_rule(query.value(0).toUInt(), rule);
  }
}
