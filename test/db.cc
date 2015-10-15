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

#include <fstream>
#include <iostream>
#include <map>
#include <QSqlError>
#include <QSqlQuery>
#include <set>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/db.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  Constructor.
 *
 *  @param[in] name     Name of database.
 *  @param[in] include  Tables that should be created. If null, create
 *                      all possible tables.
 *  @param[in] exclude  Exclude tables from being created.
 */
db::db(
      std::string const& name,
      char const* const* include,
      char const* const* exclude) : _remove_db_on_close(false) {
  // Find DB type.
  QString db_type;
  if (!strcmp(DB_TYPE, "mysql"))
    db_type = "QMYSQL";

  // Open DB.
  {
    std::ostringstream oss;
    oss << this;
    _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(
                                               db_type,
                                               oss.str().c_str())));
  }
  _db->setHostName(DB_HOST);
  _db->setPassword(DB_PASSWORD);
  _db->setPort(strtoul(DB_PORT, NULL, 0));
  _db->setUserName(DB_USER);
  if (!_db->open())
    throw (exceptions::msg() << "cannot initiate DB connection to '"
           << name << "': " << _db->lastError().text());

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
    throw (exceptions::msg() << "cannot reopen connection to DB '"
           << name << "': " << _db->lastError().text());

  // Build table creation script list. Order is important.
  std::pair<std::string, std::string> p;
  std::list<std::pair<std::string, std::string> > tables;
  // Instances table.
  p.first = "instances";
  p.second = PROJECT_SOURCE_DIR "/sql/mysql_v2/instances.sql";
  tables.push_back(p);

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
        std::list<std::pair<std::string, std::string> >::iterator
          to_erase(it);
        ++it;
        tables.erase(to_erase);
      }
      else
        ++it;
  }

  // Exclude invalid tables.
  if (exclude) {
    while (*exclude)
      for (std::list<std::pair<std::string, std::string> >::iterator
             it(tables.begin()),
             end(tables.end());
           it != end;
           ++it)
        if (it->first == *exclude) {
          tables.erase(it);
          break ;
        }
  }

  // Run table creation scripts.
  for (std::list<std::pair<std::string, std::string> >::iterator
         it(tables.begin()),
         end(tables.end());
       it != end;
       ++it)
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
 *  @param[in] columns   Number of columns returned in the query.
 *  @param[in] query     SELECT query.
 *  @param[in] expected  Expected content.
 */
template <int columns>
void db::check_content(
           std::string const& query,
           predicate expected[][columns]) {
  // Run query.
  QSqlQuery q(*_db);
  if (!q.exec(query.c_str()))
    throw (exceptions::msg() << "could not run query: "
           << q.lastError().text() << " (query was " << query << ")");

  // Browse data.
  int entry(0);
  while (q.next()) {
    if (!expected[entry][0].is_valid())
      throw (exceptions::msg()
             << "too much data returned by query (query was "
             << query << ")");
  }

  // Check if enough data was provided.
  if (expected[entry][0].is_valid())
    throw (exceptions::msg()
           << "not enough data returned by query (query was "
           << query << ")");

  return ;
}

// Generate templates.
template void db::check_content<1>(std::string const& query, predicate expected[][1]);
template void db::check_content<2>(std::string const& query, predicate expected[][2]);
template void db::check_content<3>(std::string const& query, predicate expected[][3]);
template void db::check_content<4>(std::string const& query, predicate expected[][4]);
template void db::check_content<5>(std::string const& query, predicate expected[][5]);
template void db::check_content<6>(std::string const& query, predicate expected[][6]);
template void db::check_content<7>(std::string const& query, predicate expected[][7]);
template void db::check_content<8>(std::string const& query, predicate expected[][8]);
template void db::check_content<9>(std::string const& query, predicate expected[][9]);
template void db::check_content<10>(std::string const& query, predicate expected[][10]);
template void db::check_content<11>(std::string const& query, predicate expected[][11]);
template void db::check_content<12>(std::string const& query, predicate expected[][12]);
template void db::check_content<13>(std::string const& query, predicate expected[][13]);
template void db::check_content<14>(std::string const& query, predicate expected[][14]);
template void db::check_content<15>(std::string const& query, predicate expected[][15]);
template void db::check_content<16>(std::string const& query, predicate expected[][16]);
template void db::check_content<17>(std::string const& query, predicate expected[][17]);
template void db::check_content<18>(std::string const& query, predicate expected[][18]);
template void db::check_content<19>(std::string const& query, predicate expected[][19]);
template void db::check_content<20>(std::string const& query, predicate expected[][20]);
template void db::check_content<21>(std::string const& query, predicate expected[][21]);
template void db::check_content<22>(std::string const& query, predicate expected[][22]);
template void db::check_content<23>(std::string const& query, predicate expected[][23]);
template void db::check_content<24>(std::string const& query, predicate expected[][24]);
template void db::check_content<25>(std::string const& query, predicate expected[][25]);
template void db::check_content<26>(std::string const& query, predicate expected[][26]);
template void db::check_content<27>(std::string const& query, predicate expected[][27]);
template void db::check_content<28>(std::string const& query, predicate expected[][28]);
template void db::check_content<29>(std::string const& query, predicate expected[][29]);
template void db::check_content<30>(std::string const& query, predicate expected[][30]);

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

  return ;

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
    throw (exceptions::msg()
           << error_msg << ": " << q.lastError().text());
  return ;
}

/**
 *  Set whether or not database should be removed at close.
 *
 *  @param[in] val  True to remove database, false to not remove it.
 */
void db::set_remove_db_on_close(bool val) {
  _remove_db_on_close = val;
  return ;
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
      throw (exceptions::msg()
             << "cannot open SQL table creation script '"
             << script_name << "'");
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
    throw (exceptions::msg()
           << query.lastError().text().toStdString().c_str());

  return ;
}
