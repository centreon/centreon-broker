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

#include <utility>
#include <vector>
#include <sstream>
#include <QVariant>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/objects/downtime.hh"
#include "com/centreon/broker/notification/loaders/downtime_loader.hh"

using namespace com::centreon::broker::notification;

downtime_loader::downtime_loader() {}

void downtime_loader::load(QSqlDatabase* db, downtime_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT downtime_id, entry_time, host_id, service_id, author, cancelled, deletion_time, duration, end_time, fixed, start_time, actual_start_time, actual_end_time, started, triggered_by, type FROM downtimes"))
    throw (exceptions::msg()
      << "Notification: cannot select downtimes in loader: "
      << query.lastError().text());

  while (query.next()) {
    shared_ptr<downtime> down(new downtime);
    unsigned int downtime_id = query.value(0).toUInt();

    output->add_downtime(downtime_id, down);
  }
}
