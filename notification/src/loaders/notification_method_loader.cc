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
#include "com/centreon/broker/notification/objects/notification_method.hh"
#include "com/centreon/broker/notification/loaders/notification_method_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_method_loader::notification_method_loader() {}

void notification_method_loader::load(
                                   QSqlDatabase *db,
                                   notification_method_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading notification methods from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT method_id, name, command_id, `interval`, status, "
                  "       types, start, end "
                  "  FROM cfg_notification_methods"))
    throw (exceptions::msg()
           << "notification: cannot load notification methods from database: "
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
    logging::debug(logging::low)
      << "notification: new method " << query.value(0).toUInt()
      << " ('" << nm->get_name() << "')";
    output->add_notification_method(query.value(0).toUInt(), nm);
  }
}
