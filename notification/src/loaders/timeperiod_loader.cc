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
#include "com/centreon/broker/notification/objects/timeperiod.hh"
#include "com/centreon/broker/notification/builders/timeperiod_builder.hh"
#include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

timeperiod_loader::timeperiod_loader() {

}

/**
 *  Load the timeperiods from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   A timeperiod builder object to register the timeperiods.
 */
void timeperiod_loader::load(QSqlDatabase* db, timeperiod_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday,"
                  "tp_tuesday, tp_wednesday, tp_thursday, tp_friday,"
                  "tp_saturday sunday"
                  " FROM timeperiod"))
    throw (exceptions::msg()
      << "Notification: cannot select timeperiod in loader: "
      << query.lastError().text());

  while(query.next()) {
    timeperiod::ptr tperiod(new timeperiod);
    unsigned int timeperiod_id = query.value(0).toUInt();
    tperiod->set_name(query.value(1).toString().toStdString());
    tperiod->set_alias(query.value(2).toString().toStdString());
    tperiod->set_timerange(query.value(3).toString().toStdString(), 0);
    tperiod->set_timerange(query.value(4).toString().toStdString(), 1);
    tperiod->set_timerange(query.value(5).toString().toStdString(), 2);
    tperiod->set_timerange(query.value(6).toString().toStdString(), 3);
    tperiod->set_timerange(query.value(7).toString().toStdString(), 4);
    tperiod->set_timerange(query.value(8).toString().toStdString(), 5);
    tperiod->set_timerange(query.value(9).toString().toStdString(), 6);

    output->add_timeperiod(timeperiod_id,
                           tperiod);
  }

  if (!query.exec("SELECT exception_id, timeperiod_id, days, timerange"
                  " FROM timeperiod_exceptions"))
    throw (exceptions::msg()
      << "Notification: cannot select timeperiod_exceptions in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int timeperiod_id = query.value(1).toUInt();
    std::string days = query.value(2).toString().toStdString();
    std::string timerange = query.value(3).toString().toStdString();

    output->add_timeperiod_exception(timeperiod_id, days, timerange);
  }

  if (!query.exec("SELECT exclude_id, timeperiod_id, timeperiod_exclude_id"
                  " FROM timeperiod_exclude_relations"))
    throw (exceptions::msg()
      << "Notification: cannot select timeperiod_exclude_relations in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int timeperiod_id = query.value(0).toUInt();
    unsigned int timeperiod_exclude_id = query.value(0).toUInt();

    output->add_timeperiod_exclude_relation(timeperiod_id, timeperiod_exclude_id);
  }

  if (!query.exec("SELECT include_id, timeperiod_id, timeperiod_include_id"
                  " FROM timeperiod_include_relations"))
    throw (exceptions::msg()
      << "Notification: cannot select timeperiod_include_relations in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int timeperiod_id = query.value(0).toUInt();
    unsigned int timeperiod_include_id = query.value(0).toUInt();

    output->add_timeperiod_include_relation(timeperiod_id, timeperiod_include_id);
  }
}
