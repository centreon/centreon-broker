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
using namespace com::centreon::broker::notification::objects;

downtime_loader::downtime_loader() {}

void downtime_loader::load(QSqlDatabase* db, downtime_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT downtime_id, entry_time, host_id, service_id, author,"
                  "cancelled, deletion_time, duration, end_time, fixed,"
                  "start_time, actual_start_time, actual_end_time, started,"
                  "triggered_by, type"
                  " FROM downtimes"))
    throw (exceptions::msg()
      << "Notification: cannot select downtimes in loader: "
      << query.lastError().text());

  while (query.next()) {
    shared_ptr<downtime> down(new downtime);
    unsigned int downtime_id = query.value(0).toUInt();
    down->set_entry_time(query.value(1).toUInt());
    down->set_host_id(query.value(2).toUInt());
    down->set_service_id(query.value(3).toUInt());
    down->set_author(query.value(4).toString().toStdString());
    down->set_cancelled(query.value(5).toBool());
    down->set_deletion_time(query.value(6).toUInt());
    down->set_duration(query.value(7).toUInt());
    down->set_end_time(query.value(8).toUInt());
    down->set_fixed(query.value(9).toBool());
    down->set_start_time(query.value(10).toUInt());
    down->set_actual_start_time(query.value(11).toUInt());
    down->set_actual_end_time(query.value(12).toUInt());
    down->set_started(query.value(13).toBool());
    down->set_triggered_by(query.value(14).toUInt());
    down->set_type(query.value(15).toInt());

    output->add_downtime(downtime_id, down);
  }
}

void downtime_loader::new_downtime(neb::downtime& new_downtime,
                                   downtime_builder& output) {
  shared_ptr<downtime> down(new downtime);
  down->set_actual_end_time(new_downtime.actual_end_time);
  down->set_actual_start_time(new_downtime.actual_start_time);
  down->set_author(new_downtime.author.toStdString());
  down->set_deletion_time(new_downtime.deletion_time);
  down->set_duration(new_downtime.duration);
  down->set_end_time(new_downtime.end_time);
  down->set_entry_time(new_downtime.entry_time);
  down->set_fixed(new_downtime.fixed);
  down->set_host_id(new_downtime.host_id);
  down->set_service_id(new_downtime.service_id);
  down->set_start_time(new_downtime.start_time);
  down->set_triggered_by(new_downtime.triggered_by);

  output.add_downtime(new_downtime.internal_id, down);
}
