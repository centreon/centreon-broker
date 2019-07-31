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

#include <QSqlError>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/notification/builders/timeperiod_builder.hh"
#include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::time;

timeperiod_loader::timeperiod_loader() {

}

/**
 *  Load the timeperiods from the database.
 *
 *  @param[in] db       An open connection to the database.
 *  @param[out] output   A timeperiod builder object to register the timeperiods.
 */
void timeperiod_loader::load(QSqlDatabase* db, timeperiod_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading timeperiods from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load the timeperiods.
  if (!query.exec("SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday,"
                  "       tp_tuesday, tp_wednesday, tp_thursday, tp_friday,"
                  "       tp_saturday sunday"
                  " FROM cfg_timeperiods"))
    throw (exceptions::msg()
           << "notification: cannot load timeperiods from database: "
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

    output->add_timeperiod(
              timeperiod_id,
              tperiod);
  }

  // Load the timeperiod exceptions.
  if (!query.exec("SELECT exception_id, timeperiod_id, days, timerange"
                  " FROM cfg_timeperiods_exceptions"))
    throw (exceptions::msg()
           << "notification: cannot load timeperiods exceptions from database: "
           << query.lastError().text());
  while (query.next()) {
    unsigned int timeperiod_id = query.value(1).toUInt();
    std::string days = query.value(2).toString().toStdString();
    std::string timerange = query.value(3).toString().toStdString();
    output->add_timeperiod_exception(timeperiod_id, days, timerange);
  }

  // Load the timeperiod exclude relations.
  if (!query.exec("SELECT exclude_id, timeperiod_id, timeperiod_exclude_id"
                  " FROM cfg_timeperiods_exclude_relations"))
    throw (exceptions::msg()
           << "notification: cannot load timeperiods exclusions from database: "
           << query.lastError().text());
  while (query.next())
    output->add_timeperiod_exclude_relation(
              query.value(1).toUInt(),
              query.value(0).toUInt());

  // Load the timeperiod include relations.
  if (!query.exec("SELECT include_id, timeperiod_id, timeperiod_include_id"
                  " FROM cfg_timeperiods_include_relations"))
    throw (exceptions::msg()
           << "notification: cannot load timeperiods inclusions from database: "
           << query.lastError().text());
  while (query.next())
    output->add_timeperiod_include_relation(
              query.value(1).toUInt(),
              query.value(0).toUInt());
}
