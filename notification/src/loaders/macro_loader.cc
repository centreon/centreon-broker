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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/loaders/macro_loader.hh"

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
  if (!query.exec("SELECT admin_email, admin_pager, cfg_file, status_file,"
                  "       state_retention_file, object_cache_file, temp_file,"
                  "       log_file, command_file, host_perfdata_file, "
                  "       service_perfdata_file, temp_path"
                  " FROM cfg_nagios)") || !query.next())
    throw (exceptions::msg()
      << "notification: cannot select cfg_nagios in loader: "
      << query.lastError().text());
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
  query.next();
}
