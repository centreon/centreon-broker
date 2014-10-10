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

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

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

  // Create the process manager.
  process_manager::instance();

  // Create notification scheduler
  _notif_scheduler.reset(new notification_scheduler(_state));
  _notif_scheduler->start();
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

  // Create the process manager.
  process_manager::instance();

  // Move the notification scheduler thread from the first stream.
  _notif_scheduler.reset(const_cast<stream&>(s)._notif_scheduler.release());
  _notif_scheduler->start();
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

  // Add this connection to the connections to be deleted.
  QSqlDatabase::removeDatabase(id);

  // Wait for the termination of the thread.
  _notif_scheduler->exit();
  _notif_scheduler->wait();

  // Wait for the termination of the process manager.
  process_manager::release();
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
    else if (elem == neb::de_host_status) {
      _process_host_status_event(*data.staticCast<neb::host_status>());
    }
    else if (elem == neb::de_service_status) {
      _process_service_status_event(*data.staticCast<neb::service_status>());
    }
  }
  else if(cat == io::events::correlation) {
    if (elem == correlation::de_issue_parent) {
      _process_issue_parent_event(*data.staticCast<correlation::issue_parent>());
    }
  }

  return (retval);
}

/**
 *  Open a database connexion.
 *
 *  @param[out] db                The pointer to the new database connection.
 *  @param[in] t                  The type of the database.
 *  @param[in] host               The host of the database.
 *  @param[in] port               The port of the database.
 *  @param[in] user               The user to connect with the database.
 *  @param[in] password           The password to use.
 *  @param[in] db_name            The name of the db to connect with.
 *  @param[in] id                 An unique id identifying the connection.
 *  @param[in] check_replication  True if we need to check the replication.
 */
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
        throw (exceptions::msg()
          << "NOTIFICATION: could not open SQL database: "
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

/**
 *  Clone a database connection.
 *
 *  @param[out] db          A pointer to the new db connection.
 *  @param[in] db_to_clone  A pointer to the db connection to clone.
 *  @param[in] id           An unique id identifiying the new connection.
 */
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
        throw (exceptions::msg()
          << "NOTIFICATION: could not open SQL database: "
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

/**
 *  Get the objects from the db.
 */
void stream::_update_objects_from_db() {
  if (_centreon_db.get())
    _state.update_objects_from_db(*_centreon_db.get());
}

/**
 *  Process a service status event.
 *
 *  @param event  The event to process.
 */
void stream::_process_service_status_event(neb::service_status const& event) {
  node_id id(event.host_id, event.service_id);
  short old_hard_state;
  short old_soft_state;
  time_t when_to_schedule(time(NULL) + 1);

  // Get the node corresponding to this id.
  {
    // Get the state lock.
    // TODO: The write lock here kills the perf. Use a read lock instead.
    std::auto_ptr<QWriteLocker> lock(_state.write_lock());
    node::ptr n = _state.get_node_by_id(id);
    if (!n)
      throw (exceptions::msg()
        << "NOTIFICATION: got an unknown service id: "
        << id.get_service_id() << ", host_id: " << id.get_host_id());

    // Save the old state and copy the current state.
    old_hard_state = n->get_hard_state();
    old_soft_state = n->get_soft_state();
    n->set_hard_state(event.last_hard_state);
    n->set_soft_state(event.current_state);
  }

  // From OK to NOT-OK
  if (old_hard_state != event.last_hard_state &&
      old_hard_state == node_state::ok) {
    action a;
    a.set_type(action::notification_attempt);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(time(NULL) + 1, a);
  }
  // From NOT-OK to OK
  else if (old_hard_state != event.last_hard_state &&
           old_hard_state != node_state::ok)
    _notif_scheduler->remove_actions_of_node(id);
}

/**
 *  Process a host status event.
 *
 *  @param event  The event to process.
 */
void stream::_process_host_status_event(neb::host_status const& event) {
  node_id id(event.host_id);
  short old_hard_state;
  short old_soft_state;
  time_t when_to_schedule(time(NULL) + 1);

  // Get the node corresponding to this id.
  {
    // Get the state lock.
    // TODO: The write lock here kills the perf. Use a read lock instead.
    std::auto_ptr<QWriteLocker> lock(_state.write_lock());
    node::ptr n = _state.get_node_by_id(id);
    if (!n)
      throw (exceptions::msg()
        << "NOTIFICATION: got an unknown host id: "
        << id.get_host_id());

    // Save the old state and copy the current state.
    old_hard_state = n->get_hard_state();
    old_soft_state = n->get_soft_state();
    n->set_hard_state(event.last_hard_state);
    n->set_soft_state(event.current_state);
  }

  // From OK to NOT-OK
  if (old_hard_state != event.last_hard_state &&
      old_hard_state == node_state::ok) {
    action a;
    a.set_type(action::notification_attempt);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
  // From NOT-OK to OK
  else if(old_hard_state != event.last_hard_state &&
          old_hard_state != node_state::ok)
    _notif_scheduler->remove_actions_of_node(id);
}

/**
 *  Process an issue parent event.
 *
 *  @param event  The event to process.
 */
void stream::_process_issue_parent_event(correlation::issue_parent const& event) {
  node_id id(event.child_host_id, event.child_service_id);

  // Get the node corresponding to this id.
  // Get the state lock.
  // TODO: The write lock here kills the perf. Use a read lock instead.
  std::auto_ptr<QWriteLocker> lock(_state.write_lock());
  node::ptr n = _state.get_node_by_id(id);
  if (!n)
    throw (exceptions::msg()
      << "NOTIFICATION: got an unknown issue parent (child host id: "
      << id.get_host_id() << ", child service id: " << id.get_service_id())
      << ")";

  // Add a parent relationship between correlated node.
  n->add_parent(node_id(event.parent_host_id, event.parent_service_id));
}
