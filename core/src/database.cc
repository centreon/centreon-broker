/*
** Copyright 2014-2015,2017 Centreon
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

#include <cstdlib>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
database::database(database_config const& db_cfg)
  : _db_cfg(db_cfg),
    _error(false),
    _pending_queries(0),
    _committed(db_cfg.get_queries_per_transaction() > 1 ? false : true),
    _version(v3) {
  // Qt type.
  QString qt_type(qt_db_type(_db_cfg.get_type()));

  // Compute connection ID.
  _connection_id.setNum((qulonglong)this, 16);

  // Create database connection.
  _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(
                                             qt_type,
                                             _connection_id)));
  try {
    if ("QMYSQL" == qt_type)
      _db->setConnectOptions("CLIENT_FOUND_ROWS");

    // Open database.
    _db->setHostName(_db_cfg.get_host().c_str());
    _db->setPort(_db_cfg.get_port());
    _db->setUserName(_db_cfg.get_user().c_str());
    _db->setPassword(_db_cfg.get_password().c_str());
    _db->setDatabaseName(_db_cfg.get_name().c_str());
    {
      QMutexLocker lock(&misc::global_lock);
      if (!_db->open())
        throw (exceptions::msg() << "could not open database: "
               << _db->lastError().text());
    }

    // Check that replication is OK.
    if (_db_cfg.get_check_replication()) {
      logging::debug(logging::medium)
        << "core: checking replication status of database '"
        << _db_cfg.get_name() << "' on host '" << _db_cfg.get_host()
        << "'";
      QSqlQuery q(*_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "core: could not check replication status of database '"
          << _db_cfg.get_name() << "' on host '" << _db_cfg.get_host()
          << "': " << q.lastError().text();
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "core: database '" << _db_cfg.get_name() << "' on host '"
            << _db_cfg.get_host() << "' is not under replication";
        else {
          QSqlRecord record(q.record());
          unsigned int i(0);
          for (QString field(record.fieldName(i));
               !field.isEmpty();
               field = record.fieldName(++i))
            if (((field == "Slave_IO_Running")
                 && (q.value(i).toString() != "Yes"))
                || ((field == "Slave_SQL_Running")
                    && (q.value(i).toString() != "Yes"))
                || ((field == "Seconds_Behind_Master")
                    && (q.value(i).toInt() != 0)))
              throw (exceptions::msg() << "replication of database '"
                     << _db_cfg.get_name() << "' on host '"
                     << _db_cfg.get_host() << "' is not complete: "
                     << field << "=" << q.value(i).toString());
          logging::info(logging::medium)
            << "core: replication of database '" << _db_cfg.get_name()
            << "' on host '" << _db_cfg.get_host()
            << "' is complete, connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "core: NOT checking replication status of database '"
        << _db_cfg.get_name() << "' on host '" << _db_cfg.get_host()
        << "'";

    // Check database structure.
    {
      QSqlQuery q(*_db);
      if (q.exec("SELECT instance_id FROM instances LIMIT 1")) {
        _version = v2;
        logging::info(logging::low)
          << "core: database is using version 2 of Centreon schema";
      }
      else {
        _version = v3;
        logging::info(logging::low)
          << "core: database is using version 3 of Centreon schema";
      }
    }

    // Initialize transaction.
    _new_transaction();
  }
  catch (...) {
    _db.reset();
    QSqlDatabase::removeDatabase(_connection_id);
    throw ;
  }
}

/**
 *  Destructor.
 */
database::~database() {
  if (!_error) {
    try {
      _commit();
    }
    catch (std::exception const& e) {
      (void)e;
    }
  }
  _db.reset();
  QSqlDatabase::removeDatabase(_connection_id);
}

/**
 *  Commit a transaction and restart one.
 */
void database::commit() {
  _commit();
  _new_transaction();
  return ;
}

/**
 *  Get the QDatabase object.
 *
 *  @return Qt database object.
 */
QSqlDatabase& database::get_qt_db() {
  return (*_db);
}

/**
 *  Get the Qt driver object associated to this query.
 *
 *  @return Qt driver object.
 */
QSqlDriver const* database::get_qt_driver() const {
  return (_db->driver());
}

/**
 *  Get the number of pending queries.
 *
 *  @return Number of pending queries.
 */
int database::pending_queries() const {
  return (_pending_queries);
}

/**
 *  @brief Return true if at least a transaction was committed.
 *
 *  @return  True if at least a transaction was committed.
 */
bool database::committed() const {
  return (_committed);
}

/**
 *  @brief Clear the committed flag.
 *
 *  It's an implicit commit if there is pending queries.
 */
void database::clear_committed_flag() {
  int qpt(_db_cfg.get_queries_per_transaction());

  if (qpt > 1) {
    if (_pending_queries != 0)
      commit();
    _committed = false;
  }
}

/**
 *  Get Qt DB type matching Broker DB type.
 *
 *  @param[in] broker_type  Broker DB type.
 *
 *  @return Qt DB type (driver name).
 */
QString database::qt_db_type(std::string const& broker_type) {
  QString type(broker_type.c_str());
  if (!type.compare("db2", Qt::CaseInsensitive))
    type = "QDB2";
  else if (!type.compare("ibase", Qt::CaseInsensitive)
           || !type.compare("interbase", Qt::CaseInsensitive))
    type = "QIBASE";
  else if (!type.compare("mysql", Qt::CaseInsensitive))
    type = "QMYSQL";
  else if (!type.compare("oci", Qt::CaseInsensitive)
           || !type.compare("oracle", Qt::CaseInsensitive))
    type = "QOCI";
  else if (!type.compare("odbc", Qt::CaseInsensitive))
    type = "QODBC";
  else if (!type.compare("psql", Qt::CaseInsensitive)
           || !type.compare("postgres", Qt::CaseInsensitive)
           || !type.compare("postgresql", Qt::CaseInsensitive))
    type = "QPSQL";
  else if (!type.compare("sqlite", Qt::CaseInsensitive))
    type = "QSQLITE";
  else if (!type.compare("tds", Qt::CaseInsensitive)
           || !type.compare("sybase", Qt::CaseInsensitive))
    type = "QTDS";
  return (type);
}

/**
 *  Let the database class know that a query was just executed.
 */
void database::query_executed() {
  int qpt(_db_cfg.get_queries_per_transaction());
  if (qpt > 1) {
    ++_pending_queries;
    if (_pending_queries >= qpt) {
      _commit();
      _new_transaction();
    }
  }
  return ;
}

/**
 *  Get schema version.
 *
 *  @return v2 for a 2.x schema and v3 for a 3.x schema.
 */
database::version database::schema_version() const {
  return (_version);
}

/**
 *  Set the error flag of the database connection.
 */
void database::set_error() {
  _error = true;
  return ;
}

/**
 *  Commit the current transaction.
 */
void database::_commit() {
  if (!_db->commit())
    throw (exceptions::msg() << "could not commit to database '"
           << _db_cfg.get_name() << "' on host '"
           << _db_cfg.get_host() << "': "
           << _db->lastError().text());
  _pending_queries = 0;
  _committed = true;
  return ;
}

/**
 *  Create a new transaction on this database.
 */
void database::_new_transaction() {
  if (_db_cfg.get_queries_per_transaction() > 1) {
    if (!_db->transaction())
      throw (exceptions::msg()
             << "could not create new transaction on database '"
             << _db_cfg.get_name() << "' on host '"
             << _db_cfg.get_host() << "': " << _db->lastError().text());
  }
  return ;
}
