/*
** Copyright 2009-2014 Merethis
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

#include <ctime>
#include <QPair>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QTextStream>
#include <QThread>
#include <QVariant>
#include <QVector>
#include <QMutexLocker>
#include <sstream>
#include <limits>
#include "com/centreon/engine/common.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "mapping.hh"
#include "com/centreon/broker/notification/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::notification;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/


/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] type                    Database type.
 *  @param[in] host                    Database host.
 *  @param[in] port                    Database port.
 *  @param[in] user                    User.
 *  @param[in] password                Password.
 *  @param[in] db                      Database name.
 *  @param[in] qpt                     Queries per transaction.
 *  @param[in] cleanup_thread_interval How often the stream must
 *                                     check for cleanup database.
 *  @param[in] check_replication       true to check replication status.
 *  @param[in] wse                     With state events.
 */
stream::stream(
          QString const& type,
          QString const& host,
          unsigned short port,
          QString const& user,
          QString const& password,
          QString const& db,
          unsigned int qpt,
          unsigned int cleanup_check_interval,
          bool check_replication,
          bool wse)
  : _process_out(true),
    _queries_per_transaction((qpt >= 2) ? qpt : 1),
    _transaction_queries(0),
    _with_state_events(wse),
    _instance_timeout(15) {
  // Get the driver ID.
  QString t;
  if (!type.compare("db2", Qt::CaseInsensitive))
    t = "QDB2";
  else if (!type.compare("ibase", Qt::CaseInsensitive)
           || !type.compare("interbase", Qt::CaseInsensitive))
    t = "QIBASE";
  else if (!type.compare("mysql", Qt::CaseInsensitive))
    t = "QMYSQL";
  else if (!type.compare("oci", Qt::CaseInsensitive)
           || !type.compare("oracle", Qt::CaseInsensitive))
    t = "QOCI";
  else if (!type.compare("odbc", Qt::CaseInsensitive))
    t = "QODBC";
  else if (!type.compare("psql", Qt::CaseInsensitive)
           || !type.compare("postgres", Qt::CaseInsensitive)
           || !type.compare("postgresql", Qt::CaseInsensitive))
    t = "QPSQL";
  else if (!type.compare("sqlite", Qt::CaseInsensitive))
    t = "QSQLITE";
  else if (!type.compare("tds", Qt::CaseInsensitive)
           || !type.compare("sybase", Qt::CaseInsensitive))
    t = "QTDS";
  else
    t = type;

  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Add database connection.
  _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(t, id)));
  try {
    if (t == "QMYSQL")
      _db->setConnectOptions("CLIENT_FOUND_ROWS");

    // Open database.
    _db->setHostName(host);
    _db->setPort(port);
    _db->setUserName(user);
    _db->setPassword(password);
    _db->setDatabaseName(db);

    {
      QMutexLocker lock(&global_lock);
      if (!_db->open())
        throw (exceptions::msg() << "NOTIFICATION: could not open SQL database: "
               << _db->lastError().text());
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "NOTIFICATION: checking replication status";
      QSqlQuery q(*_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "NOTIFICATION: could not check replication status";
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "NOTIFICATION: database is not under replication";
        else {
          QSqlRecord record(q.record());
          unsigned int i(0);
          for (QString field = record.fieldName(i);
               !field.isEmpty();
               field = record.fieldName(++i))
            if (((field == "Slave_IO_Running")
                 && (q.value(i).toString() != "Yes"))
                || ((field == "Slave_SQL_Running")
                    && (q.value(i).toString() != "Yes"))
                || ((field == "Seconds_Behind_Master")
                    && (q.value(i).toInt() != 0)))
              throw (exceptions::msg() << "NOTIFICATION: replication is not "
                          "complete: " << field << "="
                       << q.value(i).toString());
          logging::info(logging::medium)
            << "NOTIFICATION: database replication is complete, "
               "connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "NOTIFICATION: NOT checking replication status";  }
  catch (...) {
    {
      QMutexLocker lock(&global_lock);
      // Close database if open.
      if (_db->isOpen())
        _db->close();
      _db.reset();
    }

    // Add this connection to the connections to be deleted.
    QSqlDatabase::removeDatabase(id);
    throw ;
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  // Output processing.
  _process_out = s._process_out;

  // Queries per transaction.
  _queries_per_transaction = s._queries_per_transaction;
  _transaction_queries = 0;

  // Process state events.
  _with_state_events = s._with_state_events;

  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Clone database.
  _db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*s._db, id)));

  try {
    {
      QMutexLocker lock(&global_lock);
      // Open database.
      if (!_db->open())
        throw (exceptions::msg() << "NOTIFICATION: could not open SQL database: "
               << _db->lastError().text());
    }

    // First transaction.
    if (_queries_per_transaction > 1)
      _db->transaction();
  }
  catch (...) {

    {
      QMutexLocker lock(&global_lock);
      // Close database if open.
      if (_db->isOpen())
        _db->close();
      _db.reset();
    }

    // Add this connection to the connections to be deleted.
    QSqlDatabase::removeDatabase(id);
    throw ;
  }
}

/**
 *  Destructor.
 */
stream::~stream() {

  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  {
    QMutexLocker lock(&global_lock);
    // Close database.
    if (_db->isOpen()) {
      if (_queries_per_transaction > 1)
        _db->commit();
      _db->close();
    }
    _db.reset();
  }

  // Add this connection to the connections to be deleted.
  QSqlDatabase::removeDatabase(id);
}

/**
 *  Initialize Notification layer.
 */
void stream::initialize() {
  // Not used anymore.
  return ;
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
  _process_out = in || !out; // Only for immediate shutdown.
  return ;
}

/**
 *  Read from the database.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "NOTIFICATION: attempt to read from a notification stream");
  return ;
}

/**
 *  Update internal stream cache.
 */
void stream::update() {
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that data can be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
           << "Notification stream is shutdown");

  // Check that data exists.
  unsigned int retval(1);
  if (!data.isNull()) {

  }
  return (retval);
}
