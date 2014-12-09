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

#include <cassert>
#include <cstdlib>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] type                     DB type.
 *  @param[in] host                     DB server.
 *  @param[in] port                     Port of the DB server.
 *  @param[in] user                     DB user.
 *  @param[in] password                 DB user's password.
 *  @param[in] db_name                  DB name.
 *  @param[in] queries_per_transaction  Number of queries within a
 *                                      transaction before a commit
 *                                      should occur.
 *  @param[in] check_replication        Should we check the replication
 *                                      status of the database ?
 */
database::database(
            std::string const& type,
            std::string const& host,
            unsigned short port,
            std::string const& user,
            std::string const& password,
            std::string const& db_name,
            int queries_per_transaction,
            bool check_replication)
  : _db_name(db_name),
    _host(host),
    _password(password),
    _pending_queries(0),
    _port(port),
    _queries_per_transaction(queries_per_transaction),
    _type(type),
    _user(user) {
  // Qt type.
  QString qt_type(qt_db_type(_type));

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
    _db->setHostName(_host.c_str());
    _db->setPort(_port);
    _db->setUserName(_user.c_str());
    _db->setPassword(_password.c_str());
    _db->setDatabaseName(_db_name.c_str());
    {
      QMutexLocker lock(&misc::global_lock);
      if (!_db->open())
        throw (exceptions::msg() << "could not open database: "
               << _db->lastError().text());
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "core: checking replication status of database '"
        << _db_name << "' on host '" << _host << "'";
      QSqlQuery q(*_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "core: could not check replication status of database '"
          << _db_name << "' on host '" << _host << "': "
          << q.lastError().text();
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "core: database '" << _db_name << "' on host '"
            << _host << "' is not under replication";
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
              throw (exceptions::msg()
                     << "replication of database '" << _db_name
                     << "' on host '" << _host << "' is not complete: "
                     << field << "=" << q.value(i).toString());
          logging::info(logging::medium)
            << "core: replication of database '" << _db_name
            << "' on host '" << _host
            << "' is complete, connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "core: NOT checking replication status of database '"
        << _db_name << "' on host '" << _host << "'";

    // Initialize transaction.
    _new_transaction();
  }
  catch (...) {
    QSqlDatabase::removeDatabase(_connection_id);
    throw ;
  }
}

/**
 *  Destructor.
 */
database::~database() {
  _commit();
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
 *  @brief Copy constructor.
 *
 *  This method will abort the program.
 *
 *  @param[in] other  Unused.
 */
database::database(database const& other) {
  (void)other;
  assert(!"database objects are not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  This method will abort the program.
 *
 *  @param[in] other  Unused.
 *
 *  @return This method will call abort() and won't return.
 */
database& database::operator=(database const& other) {
  (void)other;
  assert(!"database objects are not copyable");
  abort();
  return (*this);
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
  if (_queries_per_transaction > 1) {
    ++_pending_queries;
    if (_pending_queries >= _queries_per_transaction) {
      _commit();
      _new_transaction();
    }
  }
  return ;
}

/**
 *  Commit the current transaction.
 */
void database::_commit() {
  if (!_db->commit())
    throw (exceptions::msg() << "could not commit to database '"
           << _db_name << "' on host '" << _host << "': "
           << _db->lastError().text());
  _pending_queries = 0;
  return ;
}

/**
 *  Create a new transaction on this database.
 */
void database::_new_transaction() {
  if (_queries_per_transaction > 1) {
    if (!_db->transaction())
      throw (exceptions::msg()
             << "could not create new transaction on database '"
             << _db_name << "' on host '" << _host << "': "
             << _db->lastError().text());
  }
  return ;
}
