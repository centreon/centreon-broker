/*
** Copyright 2014 Merethis
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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/loaders/macro_loader.hh"
#include "com/centreon/broker/notification/utilities/get_datetime_string.hh"

using namespace com::centreon::broker::notification;

/**
 *  Default constructor.
 */
macro_loader::macro_loader() {}

/**
 *  Load the macros from the database.
 *
 *  @param[in] db       An open connection to the database.
 *  @param[out] output  A macro builder object to register the macros.
 */
void macro_loader::load(QSqlDatabase *db, macro_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading macros from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load global, constant macro.
  if (!query.exec(
               "SELECT admin_email, admin_pager, cfg_file, status_file,"
               "       state_retention_file, object_cache_file, temp_file,"
               "       log_file, command_file, host_perfdata_file, "
               "       service_perfdata_file, temp_path, date_format"
               " FROM cfg_engine"))
    throw (exceptions::msg()
           << "notification: cannot load global monitoring options from database: "
           << query.lastError().text());
  if (!query.next()) {
    logging::config(logging::medium)
      << "notification: could not find default monitoring options, "
      << "some global macros will be empty";
  }
  else {
    output->add_global_macro(
              "ADMINEMAIL",
              query.value(0).toString().toStdString());
    output->add_global_macro(
              "ADMINPAGER",
              query.value(1).toString().toStdString());
    output->add_global_macro(
              "MAINCONFIGFILE",
              query.value(2).toString().toStdString());
    output->add_global_macro(
              "STATUSDATAFILE",
              query.value(3).toString().toStdString());
    output->add_global_macro(
              "RETENTIONDATAFILE",
              query.value(4).toString().toStdString());
    output->add_global_macro(
              "OBJECTCACHEFILE",
              query.value(5).toString().toStdString());
    output->add_global_macro(
              "TEMPFILE",
              query.value(6).toString().toStdString());
    output->add_global_macro(
              "LOGFILE",
              query.value(7).toString().toStdString());
    output->add_global_macro("RESOURCEFILE", "resource.cfg");
    output->add_global_macro(
              "COMMANDFILE",
              query.value(8).toString().toStdString());
    output->add_global_macro(
              "HOSTPERFDATAFILE",
              query.value(9).toString().toStdString());
    output->add_global_macro(
              "SERVICEPERFDATAFILE",
              query.value(10).toString().toStdString());
    output->add_global_macro(
              "TEMPPATH",
              query.value(11).toString().toStdString());
    QString date_format = query.value(12).toString();
    if (date_format == "euro")
      output->add_date_format(utilities::date_format_euro);
    else if (date_format == "iso8601")
      output->add_date_format(utilities::date_format_iso8601);
    else if (date_format == "strict-iso8601")
      output->add_date_format(utilities::date_format_strict_iso8601);
    else
      output->add_date_format(utilities::date_format_us);
  }

  // Load global resource macros.
  if (!query.exec(
               "SELECT resource_name, resource_line"
                "  FROM cfg_resource"
                "  WHERE resource_activate = 1"));
    throw (exceptions::msg()
           << "notification: cannot load resource macros from database: "
           << query.lastError().text());
  while (query.next()) {
    // Remove leading and trailing $$
    QString macro_name = query.value(0).toString();
    macro_name.remove(0, 1).remove(macro_name.size() - 1, 1);
    output->add_resource_macro(
              // Remove leading and trailing $$
              macro_name.toStdString(),
              query.value(1).toString().toStdString());
  }
  return ;
}
