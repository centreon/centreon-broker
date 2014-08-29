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
#include <QHash>
#include <QMultiHash>
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
#include "com/centreon/broker/notification/utilities/data_loggers.hh"
#include "com/centreon/broker/notification/stream.hh"

#include "com/centreon/broker/notification/builders/composed_acknowledgement_builder.hh"
#include "com/centreon/broker/notification/builders/composed_command_builder.hh"
#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"
#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"
#include "com/centreon/broker/notification/builders/composed_downtime_builder.hh"
#include "com/centreon/broker/notification/builders/composed_escalation_builder.hh"
#include "com/centreon/broker/notification/builders/composed_node_builder.hh"
#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"

#include "com/centreon/broker/notification/builders/acknowledgement_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/command_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_node_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/escalation_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/node_set_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_by_name_builder.hh"

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
          QString const& centreon_db,
          QString const& centreon_storage_db,
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

  // Open centreon database.
  _open_db(_centreon_db,
           t,
           host,
           port,
           user,
           password,
           centreon_db,
           id,
           check_replication);

  // Open centreon storage database.
  id.setNum(((qulonglong)this) + 1, 16);
  _open_db(_centreon_storage_db,
           t,
           host,
           port,
           user,
           password,
           centreon_storage_db,
           id,
           check_replication);
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
  QString id2;
  id.setNum(((qulonglong)this) + 1, 16);

  // Clone centreon database.
  _clone_db(_centreon_db, s._centreon_db, id);
  // Clone centreon storage database.
  _clone_db(_centreon_storage_db, s._centreon_storage_db, id);
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
    if (_centreon_db->isOpen()) {
      if (_queries_per_transaction > 1)
        _centreon_db->commit();
      _centreon_db->close();
    }
    _centreon_db.reset();
  }

  {
    QMutexLocker lock(&global_lock);
    // Close database.
    if (_centreon_storage_db->isOpen()) {
      if (_queries_per_transaction > 1)
        _centreon_storage_db->commit();
      _centreon_storage_db->close();
    }
    _centreon_storage_db.reset();
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
  _update_objects_from_db();
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
  if (data.isNull())
    return 1;

  unsigned int type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));

  if (cat == io::events::neb)  {
    if (elem == neb::de_downtime) {
      // Load downtime
    }
    else if (elem == neb::de_acknowledgement) {
      // Load acknowledgement
    }
  }

  return (retval);
}

void stream::_open_db(std::auto_ptr<QSqlDatabase>& db,
                      QString const& t,
                      QString const& host,
                      unsigned short port,
                      QString const& user,
                      QString const& password,
                      QString const& db_name,
                      QString const& id,
                      bool check_replication) {
   // Add database connection.
  db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(t, id)));
  try {
    if (t == "QMYSQL")
      db->setConnectOptions("CLIENT_FOUND_ROWS");

    // Open database.
    db->setHostName(host);
    db->setPort(port);
    db->setUserName(user);
    db->setPassword(password);
    db->setDatabaseName(db_name);

    {
      QMutexLocker lock(&global_lock);
      if (!db->open())
        throw (exceptions::msg() << "NOTIFICATION: could not open SQL database: "
               << db->lastError().text());
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "NOTIFICATION: checking replication status";
      QSqlQuery q(*db);
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
      if (db->isOpen())
        db->close();
      db.reset();
    }

    // Add this connection to the connections to be deleted.
    QSqlDatabase::removeDatabase(id);
    throw ;
  }
}

void stream::_clone_db(std::auto_ptr<QSqlDatabase>& db,
                       std::auto_ptr<QSqlDatabase> const& db_to_clone,
                       QString const& id) {
  // Clone database.
  db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*db_to_clone, id)));

  try {
    {
      QMutexLocker lock(&global_lock);
      // Open database.
      if (!db->open())
        throw (exceptions::msg() << "NOTIFICATION: could not open SQL database: "
               << db->lastError().text());
    }

    // First transaction.
    if (_queries_per_transaction > 1)
      db->transaction();
  }
  catch (...) {

    {
      QMutexLocker lock(&global_lock);
      // Close database if open.
      if (db->isOpen())
        db->close();
      db.reset();
    }

    // Add this connection to the connections to be deleted.
    QSqlDatabase::removeDatabase(id);
    throw ;
  }
}

void stream::_update_objects_from_db() {
  {
    node_loader node;
    composed_node_builder composed;
    node_set_builder set_builder(_nodes);
    composed.push_back(set_builder);
    node.load(_centreon_db.get(), &composed);
  }
  {
    command_loader command;
    composed_command_builder composed;
    command_by_name_builder by_name_builder(_commands);
    composed.push_back(by_name_builder);
    command.load(_centreon_db.get(), &composed);
  }
  {
    contact_loader contact;
    composed_contact_builder composed;
    contact_by_name_builder by_name_builder(_contact_by_name);
    contact_by_node_builder by_node_builder(_contacts);
    composed.push_back(by_name_builder);
    composed.push_back(by_node_builder);
    contact.load(_centreon_db.get(), &composed);
  }
  {
    dependency_loader dependency;
    composed_dependency_builder composed;
    dependency_by_node_id_builder by_node_builder(_dependency_by_child_id,
                                                  _dependency_by_parent_id);
    composed.push_back(by_node_builder);
    dependency.load(_centreon_db.get(), &composed);
  }
  {
    escalation_loader escalation;
    composed_escalation_builder composed;
    escalation_by_node_id_builder by_node_builder(_escalations);
    composed.push_back(by_node_builder);
    escalation.load(_centreon_db.get(), &composed);
  }
  {
    timeperiod_loader timeperiod;
    composed_timeperiod_builder composed;
    timeperiod_by_name_builder by_name_builder(_timeperiod_by_name);
    composed.push_back(by_name_builder);
    timeperiod.load(_centreon_db.get(), &composed);
  }
  {
    downtime_loader downtime;
    composed_downtime_builder composed;
    downtime_by_node_id_builder by_node_builder(_downtimes);
    composed.push_back(by_node_builder);
    downtime.load(_centreon_storage_db.get(), &composed);
  }
  {
    acknowledgement_loader ack;
    composed_acknowledgement_builder composed;
    acknowledgement_by_node_id_builder by_node_builder(_acks);
    composed.push_back(by_node_builder);
    ack.load(_centreon_storage_db.get(), &composed);
  }

  // Debug logging for all the data loaded.
#ifndef NDEBUG
    data_logger::log_container("_nodes", _nodes);
    data_logger::log_container("_acks", _acks);
    data_logger::log_container("_commands", _commands);
    data_logger::log_container("_contact_by_name", _contact_by_name);
    data_logger::log_container("_contacts", _contacts);
    data_logger::log_container("_dependency_by_child_id", _dependency_by_child_id);
    data_logger::log_container("_dependency_by_parent_id", _dependency_by_parent_id);
    data_logger::log_container("_downtimes", _downtimes);
    data_logger::log_container("_escalations", _escalations);
    data_logger::log_container("_timeperiod_by_name", _timeperiod_by_name);
#endif //!NDEBUG
}
