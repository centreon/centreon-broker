/*
** Copyright 2012-2015 Centreon
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

#include "test/db.hh"
#include <stdlib.h>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/predicate.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  Add a database script to the list of scripts.
 *
 *  @param[out] list      Script list.
 *  @param[in]  table     Table name.
 *  @param[in]  base_dir  Scripts base directory.
 */
static void add_db_script(std::list<std::pair<std::string, std::string> >& list,
                          std::string const& table,
                          std::string const& base_dir) {
  std::string path(base_dir);
  path.append("/");
  path.append(table);
  path.append(".sql");
  list.push_back(std::make_pair(table, path));
  return;
}

/**
 *  Constructor.
 *
 *  @param[in] name     Name of database.
 *  @param[in] include  Tables that should be created. If null, create
 *                      all possible tables.
 *  @param[in] exclude  Exclude tables from being created.
 */
db::db(std::string const& name,
       char const* const* include,
       char const* const* exclude)
    : _remove_db_on_close(false) {
  // Find DB type.
  QString db_type;
  std::string db_subdir;
  if (!strcmp(DB_TYPE, "mysql")) {
    db_type = "QMYSQL";
    db_subdir = "mysql";
  } else if (!strcmp(DB_TYPE, "psql") || !strcmp(DB_TYPE, "postgres") ||
             !strcmp(DB_TYPE, "postgresql")) {
    db_type = "QPSQL";
    db_subdir = "postgresql";
  } else if (!strcmp(DB_TYPE, "oci") || !strcmp(DB_TYPE, "oracle")) {
    db_type = "QOCI";
    db_subdir = "oracle";
  } else
    throw(exceptions::msg() << "unsupported database type: " << DB_TYPE);

  // Open DB.
  {
    std::ostringstream oss;
    oss << this;
    _db.reset(new QSqlDatabase(
        QSqlDatabase::addDatabase(db_type, oss.str().c_str())));
  }
  _db->setHostName(DB_HOST);
  _db->setPassword(DB_PASSWORD);
  _db->setPort(strtoul(DB_PORT, NULL, 0));
  _db->setUserName(DB_USER);
  if (!_db->open())
    throw(exceptions::msg() << "cannot initiate DB connection to '" << name
                            << "': " << _db->lastError().text());

  // Drop DB.
  {
    std::ostringstream query;
    query << "DROP DATABASE " << name;
    QSqlQuery q(*_db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  // Create DB.
  {
    std::ostringstream query;
    query << "CREATE DATABASE " << name;
    if ("QMYSQL" == _db->driverName())
      query << " DEFAULT CHARACTER SET utf8";
    QSqlQuery q(*_db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  // Close and reopen.
  _db->close();
  _db->setDatabaseName(name.c_str());
  if (!_db->open())
    throw(exceptions::msg() << "cannot reopen connection to DB '" << name
                            << "': " << _db->lastError().text());

  // Build table creation script list. Order is important.
  std::list<std::pair<std::string, std::string> > tables;
  std::string db_dir;

  //
  // Schema version 2.
  //

  // RT-monitoring.
  db_dir = PROJECT_SOURCE_DIR "/sql/";
  db_dir.append(db_subdir);
  db_dir.append("_v2");
  add_db_script(tables, "instances", db_dir);
  add_db_script(tables, "hosts", db_dir);
  add_db_script(tables, "hostgroups", db_dir);
  add_db_script(tables, "hosts_hostgroups", db_dir);
  add_db_script(tables, "hosts_hosts_dependencies", db_dir);
  add_db_script(tables, "hosts_hosts_parents", db_dir);
  add_db_script(tables, "services", db_dir);
  add_db_script(tables, "servicegroups", db_dir);
  add_db_script(tables, "services_servicegroups", db_dir);
  add_db_script(tables, "services_services_dependencies", db_dir);
  add_db_script(tables, "acknowledgements", db_dir);
  add_db_script(tables, "comments", db_dir);
  add_db_script(tables, "customvariables", db_dir);
  add_db_script(tables, "downtimes", db_dir);
  add_db_script(tables, "eventhandlers", db_dir);
  add_db_script(tables, "flappingstatuses", db_dir);
  add_db_script(tables, "logs", db_dir);
  add_db_script(tables, "modules", db_dir);
  add_db_script(tables, "notifications", db_dir);
  // Correlation.
  add_db_script(tables, "issues", db_dir);
  add_db_script(tables, "issues_issues_parents", db_dir);
  add_db_script(tables, "hoststateevents", db_dir);
  add_db_script(tables, "servicestateevents", db_dir);
  // Performance data.
  add_db_script(tables, "index_data", db_dir);
  add_db_script(tables, "metrics", db_dir);
  add_db_script(tables, "data_bin", db_dir);
  // Centreon.
  db_dir = PROJECT_SOURCE_DIR "/test/";
  db_dir.append(db_subdir);
  db_dir.append("_v2");
  add_db_script(tables, "timeperiod", db_dir);
  add_db_script(tables, "timeperiod_exceptions", db_dir);
  add_db_script(tables, "timeperiod_exclude_relations", db_dir);
  add_db_script(tables, "connector", db_dir);
  add_db_script(tables, "command", db_dir);
  add_db_script(tables, "hostgroup", db_dir);
  add_db_script(tables, "servicegroup", db_dir);
  add_db_script(tables, "host", db_dir);
  add_db_script(tables, "service", db_dir);
  add_db_script(tables, "host_service_relation", db_dir);
  // BAM monitoring.
  db_dir = PROJECT_SOURCE_DIR "/bam/";
  db_dir.append(db_subdir);
  db_dir.append("_v2");
  add_db_script(tables, "mod_bam", db_dir);
  add_db_script(tables, "mod_bam_poller_relations", db_dir);
  add_db_script(tables, "mod_bam_impacts", db_dir);
  add_db_script(tables, "mod_bam_boolean", db_dir);
  add_db_script(tables, "mod_bam_kpi", db_dir);
  add_db_script(tables, "mod_bam_relations_ba_timeperiods", db_dir);
  add_db_script(tables, "mod_bam_ba_groups", db_dir);
  add_db_script(tables, "mod_bam_bagroup_ba_relation", db_dir);
  add_db_script(tables, "meta_service", db_dir);
  add_db_script(tables, "meta_service_relation", db_dir);
  // BAM reporting.
  add_db_script(tables, "mod_bam_reporting_bv", db_dir);
  add_db_script(tables, "mod_bam_reporting_ba", db_dir);
  add_db_script(tables, "mod_bam_reporting_kpi", db_dir);
  add_db_script(tables, "mod_bam_reporting_relations_ba_bv", db_dir);
  add_db_script(tables, "mod_bam_reporting_ba_events", db_dir);
  add_db_script(tables, "mod_bam_reporting_kpi_events", db_dir);
  add_db_script(tables, "mod_bam_reporting_relations_ba_kpi_events", db_dir);
  add_db_script(tables, "mod_bam_reporting_timeperiods", db_dir);
  add_db_script(tables, "mod_bam_reporting_timeperiods_exceptions", db_dir);
  add_db_script(tables, "mod_bam_reporting_timeperiods_exclusions", db_dir);
  add_db_script(tables, "mod_bam_reporting_relations_ba_timeperiods", db_dir);
  add_db_script(tables, "mod_bam_reporting_ba_events_durations", db_dir);
  add_db_script(tables, "mod_bam_reporting_ba_availabilities", db_dir);

  //
  // Schema version 3.
  //

  // RT-monitoring.
  db_dir = PROJECT_SOURCE_DIR "/sql/";
  db_dir.append(db_subdir);
  db_dir.append("_v3");
  add_db_script(tables, "rt_instances", db_dir);
  add_db_script(tables, "rt_hosts", db_dir);
  add_db_script(tables, "rt_hosts_hosts_dependencies", db_dir);
  add_db_script(tables, "rt_hosts_hosts_parents", db_dir);
  add_db_script(tables, "rt_services", db_dir);
  add_db_script(tables, "rt_services_services_dependencies", db_dir);
  add_db_script(tables, "rt_acknowledgements", db_dir);
  add_db_script(tables, "rt_customvariables", db_dir);
  add_db_script(tables, "log_logs", db_dir);
  add_db_script(tables, "rt_downtimes", db_dir);
  add_db_script(tables, "rt_eventhandlers", db_dir);
  add_db_script(tables, "rt_flappingstatuses", db_dir);
  add_db_script(tables, "rt_modules", db_dir);
  add_db_script(tables, "rt_notifications", db_dir);
  // Correlation.
  add_db_script(tables, "rt_issues", db_dir);
  add_db_script(tables, "rt_issues_issues_parents", db_dir);
  add_db_script(tables, "rt_hoststateevents", db_dir);
  add_db_script(tables, "rt_servicestateevents", db_dir);
  // Performance data.
  add_db_script(tables, "rt_index_data", db_dir);
  add_db_script(tables, "rt_metrics", db_dir);
  add_db_script(tables, "log_data_bin", db_dir);
  // BAM monitoring.
  db_dir = PROJECT_SOURCE_DIR "/bam/";
  db_dir.append(db_subdir);
  db_dir.append("_v3");
  add_db_script(tables, "cfg_bam_ba_types", db_dir);
  add_db_script(tables, "cfg_bam", db_dir);
  add_db_script(tables, "cfg_bam_poller_relations", db_dir);
  add_db_script(tables, "cfg_bam_impacts", db_dir);
  add_db_script(tables, "cfg_bam_boolean", db_dir);
  add_db_script(tables, "cfg_bam_kpi", db_dir);
  add_db_script(tables, "cfg_bam_relations_ba_timeperiods", db_dir);
  add_db_script(tables, "cfg_bam_ba_groups", db_dir);
  add_db_script(tables, "cfg_bam_bagroup_ba_relation", db_dir);
  add_db_script(tables, "cfg_meta_services", db_dir);
  add_db_script(tables, "cfg_meta_services_relations", db_dir);
  // BAM reporting.
  // add_db_script(tables, "mod_bam_reporting_bv", db_dir);
  // add_db_script(tables, "mod_bam_reporting_ba", db_dir);
  // add_db_script(tables, "mod_bam_reporting_kpi", db_dir);
  // add_db_script(tables, "mod_bam_reporting_relations_ba_bv", db_dir);
  // add_db_script(tables, "mod_bam_reporting_ba_events", db_dir);
  // add_db_script(tables, "mod_bam_reporting_kpi_events", db_dir);
  // add_db_script(tables, "mod_bam_reporting_relations_ba_kpi_events", db_dir);
  // add_db_script(tables, "mod_bam_reporting_timeperiods", db_dir);
  // add_db_script(tables, "mod_bam_reporting_timeperiods_exceptions", db_dir);
  // add_db_script(tables, "mod_bam_reporting_timeperiods_exclusions", db_dir);
  // add_db_script(tables, "mod_bam_reporting_relations_ba_timeperiods",
  // db_dir); add_db_script(tables, "mod_bam_reporting_ba_events_durations",
  // db_dir); add_db_script(tables, "mod_bam_reporting_ba_availabilities",
  // db_dir);

  // Only include valid tables.
  if (include) {
    std::set<std::string> included;
    while (*include) {
      included.insert(*include);
      ++include;
    }
    for (std::list<std::pair<std::string, std::string> >::iterator
             it(tables.begin()),
         end(tables.end());
         it != end;)
      if (included.find(it->first) == included.end()) {
        std::list<std::pair<std::string, std::string> >::iterator to_erase(it);
        ++it;
        tables.erase(to_erase);
      } else
        ++it;
  }

  // Exclude invalid tables.
  if (exclude) {
    while (*exclude)
      for (std::list<std::pair<std::string, std::string> >::iterator
               it(tables.begin()),
           end(tables.end());
           it != end; ++it)
        if (it->first == *exclude) {
          tables.erase(it);
          break;
        }
  }

  // Run table creation scripts.
  for (std::list<std::pair<std::string, std::string> >::iterator
           it(tables.begin()),
       end(tables.end());
       it != end; ++it)
    _run_script(it->second.c_str());
}

/**
 *  Destructor.
 */
db::~db() {
  close();
}

/**
 *  Check database content.
 *
 *  @param[in] query     SELECT query.
 *  @param[in] expected  Expected content.
 */
void db::check_content(std::string const& query,
                       table_content const& expected) {
  // Run query.
  QSqlQuery q(*_db);
  if (!q.exec(query.c_str()))
    throw(exceptions::msg() << "could not run query: " << q.lastError().text()
                            << " (query was " << query << ")");

  // Browse data.
  int columns(expected.get_columns());
  int rows(expected.get_rows());
  int entry(0);
  while (q.next()) {
    if (entry >= rows) {
      exceptions::msg e;
      e << "too much data returned by query: next entry is ";
      QSqlRecord rec(q.record());
      for (int i(0); i < columns; ++i)
        e << rec.fieldName(i) << " "
          << (q.value(i).isNull() ? "NULL" : q.value(i).toString())
          << ((i == columns - 1) ? "" : ", ");
      e << " (query was " << query << ")";
      throw(e);
    }
    std::list<int> mismatching;
    for (int i(0); i < columns; ++i)
      if (expected[entry][i] != q.value(i))
        mismatching.push_back(i);
    if (!mismatching.empty()) {
      exceptions::msg e;
      e << "row " << entry << " does not match expected values: ";
      QSqlRecord rec(q.record());
      for (std::list<int>::const_iterator it(mismatching.begin()),
           end(mismatching.end());
           it != end; ++it) {
        e << ((it == mismatching.begin()) ? "" : ", ") << rec.fieldName(*it)
          << " " << (q.value(*it).isNull() ? "NULL" : q.value(*it).toString())
          << " / " << expected[entry][*it];
      }
      throw(e);
    }
    ++entry;
  }

  // Check if enough data was provided.
  if (entry != rows)
    throw(exceptions::msg()
          << "not enough data returned by query (query was " << query << ")");

  return;
}

/**
 *  Close database.
 */
void db::close() {
  QString connection_name(_db->connectionName());

  if (_remove_db_on_close) {
    std::ostringstream query;
    query << "DROP DATABASE " << _db->databaseName().toStdString();
    QSqlQuery q(*_db);
    if (!q.exec(query.str().c_str()))
      std::cerr << q.lastError().text().toStdString() << std::endl;
  }

  _db->close();
  _db.reset();
  QSqlDatabase::removeDatabase(connection_name);

  return;
}

/**
 *  Get database object.
 *
 *  @return Database object.
 */
QSqlDatabase* db::get_db() {
  return (_db.get());
}

/**
 *  Run a query.
 *
 *  @param[in] query      Query to execute.
 *  @param[in] error_msg  Error message in case of execution failure.
 */
void db::run(QString const& query, QString const& error_msg) {
  QSqlQuery q(*_db);
  if (!q.exec(query))
    throw(exceptions::msg() << error_msg << ": " << q.lastError().text());
  return;
}

/**
 *  Set whether or not database should be removed at close.
 *
 *  @param[in] val  True to remove database, false to not remove it.
 */
void db::set_remove_db_on_close(bool val) {
  _remove_db_on_close = val;
  return;
}

/**
 *  Run a script on the database.
 *
 *  @param[in] script_name  Path to the script.
 */
void db::_run_script(char const* script_name) {
  // Read table creation script.
  QByteArray table_creation_script;
  {
    std::ifstream ifs;
    ifs.open(script_name);
    if (ifs.fail())
      throw(exceptions::msg()
            << "cannot open SQL table creation script '" << script_name << "'");
    char buffer[1024];
    std::streamsize rb;
    ifs.read(buffer, sizeof(buffer));
    rb = ifs.gcount();
    while (rb > 0) {
      table_creation_script.append(buffer, rb);
      ifs.read(buffer, sizeof(buffer));
      rb = ifs.gcount();
    }
    ifs.close();
  }

  // Execute table creation script.
  QSqlQuery query(*_db);
  if (!query.exec(QString(table_creation_script)))
    throw(exceptions::msg()
          << "cannot run table creation script '" << script_name
          << "': " << query.lastError().text().toStdString().c_str());

  return;
}
