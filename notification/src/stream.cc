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

#include <ctime>
#include <QPair>
//#include <QSqlDriver>
//#include <QSqlError>
//#include <QSqlField>
//#include <QSqlRecord>
#include <QTextStream>
#include <QVector>
#include <QMutexLocker>
#include <QHash>
#include <QMultiHash>
#include <mutex>
#include <sstream>
#include <limits>
#include "com/centreon/engine/common.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/utilities/data_loggers.hh"
#include "com/centreon/broker/notification/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

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
 *  @param[in] centreon_db             Database name.
 *  @param[in] check_replication       true to check replication status.
 *  @param[in] node_cache              A loaded node event cache.
 */
stream::stream(
          std::string const& type,
          std::string const& host,
          unsigned short port,
          std::string const& user,
          std::string const& password,
          std::string const& centreon_db,
          bool check_replication,
          node_cache& cache)
  : _node_cache(cache) {
  // Get the driver ID.
    std::string t("QMYSQL");
//  if (!type.compare("db2", Qt::CaseInsensitive))
//    t = "QDB2";
//  else if (!type.compare("ibase", Qt::CaseInsensitive)
//           || !type.compare("interbase", Qt::CaseInsensitive))
//    t = "QIBASE";
//  else if (!type.compare("mysql", Qt::CaseInsensitive))
//    t = "QMYSQL";
//  else if (!type.compare("oci", Qt::CaseInsensitive)
//           || !type.compare("oracle", Qt::CaseInsensitive))
//    t = "QOCI";
//  else if (!type.compare("odbc", Qt::CaseInsensitive))
//    t = "QODBC";
//  else if (!type.compare("psql", Qt::CaseInsensitive)
//           || !type.compare("postgres", Qt::CaseInsensitive)
//           || !type.compare("postgresql", Qt::CaseInsensitive))
//    t = "QPSQL";
//  else if (!type.compare("sqlite", Qt::CaseInsensitive))
//    t = "QSQLITE";
//  else if (!type.compare("tds", Qt::CaseInsensitive)
//           || !type.compare("sybase", Qt::CaseInsensitive))
//    t = "QTDS";
//  else
//    t = type;

  // Connection ID.
//  QString id;
//  id.setNum((qulonglong)this, 16);

  // Open centreon database.
  _open_db(
    _centreon_db,
    t,
    host,
    port,
    user,
    password,
    centreon_db,
//    id,
    check_replication);

  // Create the process manager.
  process_manager::instance();

  // Create notification scheduler
  _notif_scheduler.reset(new notification_scheduler(_state, _node_cache));
  _notif_scheduler->start();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
stream::stream(stream const& other) : io::stream(other),
  _node_cache(other._node_cache) {
  _centreon_db = std::move(const_cast<stream&>(other)._centreon_db);
  // Connection ID.
//  QString id;
//  id.setNum((qulonglong)this, 16);
//
//  // Clone centreon database.
//  _clone_db(_centreon_db, other._centreon_db, id);

  // Create the process manager.
  process_manager::instance();

  // Move the notification scheduler thread from the first stream.
  _notif_scheduler.reset(
    const_cast<stream&>(other)._notif_scheduler.release());
  _notif_scheduler->start();
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Connection ID.
//  QString id;
//  id.setNum((qulonglong)this, 16);
//
//  {
//    QMutexLocker lock(&global_lock);
//    // Close database.
//    if (_centreon_db->isOpen())
//      _centreon_db->close();
//    _centreon_db.reset();
//  }
//
//  // Add this connection to the connections to be deleted.
//  QSqlDatabase::removeDatabase(id);

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
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Unused.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw (exceptions::shutdown()
         << "attempt to read from a notification stream");
  return (true);
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
int stream::write(std::shared_ptr<io::data> const& data) {
  // Check that data exists.
  if (!validate(data, "notification"))
    return (1);

  unsigned int retval(1);

  // Update node cache.
  _node_cache.write(data);

  // Process events.
  if (data->type() == neb::host_status::static_type())
    _process_host_status_event(*std::static_pointer_cast<neb::host_status>(data));
  else if (data->type() == neb::service_status::static_type())
    _process_service_status_event(*std::static_pointer_cast<neb::service_status>(data));
  else if (data->type() == correlation::issue_parent::static_type())
    _process_issue_parent_event(*std::static_pointer_cast<correlation::issue_parent>(data));
  else if (data->type() == neb::acknowledgement::static_type())
    _process_ack(*std::static_pointer_cast<neb::acknowledgement>(data));
  else if (data->type() == neb::downtime::static_type())
    _process_downtime(*std::static_pointer_cast<neb::downtime>(data));

  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Open a database connexion.
 *
 *  @param[out] db                The pointer to the new database connection.
 *  @param[in]  t                  The type of the database.
 *  @param[in]  host               The host of the database.
 *  @param[in]  port               The port of the database.
 *  @param[in]  user               The user to connect with the database.
 *  @param[in]  password           The password to use.
 *  @param[in]  db_name            The name of the db to connect with.
 *  @param[in]  id                 An unique id identifying the connection.
 *  @param[in]  check_replication  True if we need to check the replication.
 */
void stream::_open_db(
               std::unique_ptr<mysql>& ms,
               std::string const& t,
               std::string const& host,
               unsigned short port,
               std::string const& user,
               std::string const& password,
               std::string const& db_name,
               //QString const& id,
               bool check_replication) {
  int queries_per_transaction(1);
  // Add database connection.
  ms.reset(new mysql(database_config(
                   t,
                   host,
                   port,
                   user,
                   password,
                   db_name,
                   queries_per_transaction,
                   check_replication,
                   1)));
//  try {
//    if (t == "QMYSQL")
//      db->setConnectOptions("CLIENT_FOUND_ROWS");
//
//    // Open database.
//    db->setHostName(host);
//    db->setPort(port);
//    db->setUserName(user);
//    db->setPassword(password);
//    db->setDatabaseName(db_name);

//    {
//      QMutexLocker lock(&global_lock);
//      if (!db->open())
//        throw (exceptions::msg()
//          << "notification: could not open SQL database: "
//          << db->lastError().text());
//    }

    // Check that replication is OK.
  if (check_replication) {
    logging::debug(logging::medium)
      << "notification: checking replication status";
    std::promise<database::mysql_result> promise;
    ms->run_query_and_get_result(
            "SHOW SLAVE STATUS",
            &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      if (ms->fetch_row(res)) {
        for (int i(0); i < res.get_num_fields(); ++i) {
          std::string field(res.get_field_name(i));
          if ((field == "Slave_IO_Running"
               && res.value_as_str(i) != "Yes")
              || (field == "Slave_SQL_Running"
                  && res.value_as_str(i) != "Yes")
              || (field == "Seconds_Behind_Master"
                  && res.value_as_i32(i) != 0))
            throw exceptions::msg() << "notification: replication is not "
                          "complete: " << field << "="
                       << res.value_as_str(i);
        }
        logging::info(logging::medium)
          << "notification: database replication is complete, "
             "connection granted";
      }
      else {
        logging::info(logging::medium)
          << "notification: database is not under replication";
      }
    }
    catch (std::exception const& e) {
      logging::info(logging::medium)
        << "notification: could not check replication status";
    }
  }
  else
    logging::debug(logging::medium)
      << "notification: NOT checking replication status";
//  catch (...) {
//    {
//      QMutexLocker lock(&global_lock);
//      // Close database if open.
//      if (db->isOpen())
//        db->close();
//      db.reset();
//    }
//
//    // Add this connection to the connections to be deleted.
//    QSqlDatabase::removeDatabase(id);
//    throw ;
//  }
}

/**
 *  Clone a database connection.
 *
 *  @param[out] db          A pointer to the new db connection.
 *  @param[in] db_to_clone  A pointer to the db connection to clone.
 *  @param[in] id           An unique id identifiying the new connection.
 */
//void stream::_clone_db(
//               std::unique_ptr<QSqlDatabase>& db,
//               std::unique_ptr<QSqlDatabase> const& db_to_clone,
//               QString const& id) {
//  // Clone database.
//  db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*db_to_clone, id)));
//
//  try {
//    QMutexLocker lock(&global_lock);
//    // Open database.
//    if (!db->open())
//      throw (exceptions::msg()
//             << "notification: could not open SQL database: "
//             << db->lastError().text());
//  }
//  catch (...) {
//
//    {
//      QMutexLocker lock(&global_lock);
//      // Close database if open.
//      if (db->isOpen())
//        db->close();
//      db.reset();
//    }
//
//    // Add this connection to the connections to be deleted.
//    QSqlDatabase::removeDatabase(id);
//    throw ;
//  }
//}

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
  logging::debug(logging::medium)
    << "notification: processing status of service " << event.service_id
    << " of host " << event.host_id << " (state "
    << event.last_hard_state << ")";

  node_id id(event.host_id, event.service_id);
  short old_hard_state;
  time_t when_to_schedule(::time(NULL) + 1);

  // Get the node corresponding to this id.
  {
    // Get the state lock.
    // TODO: The write lock here kills the perf. Use a read lock instead.
    //       and lock on an individual node level.
    std::unique_ptr<QWriteLocker> lock(_state.write_lock());
    node::ptr n = _state.get_node_by_id(id);
    if (!n)
      throw (exceptions::msg()
        << "notification: got an unknown service id: "
        << id.get_service_id() << ", host_id: " << id.get_host_id());

    // Save the old state and copy the current state.
    old_hard_state = n->get_hard_state();
    n->set_hard_state(event.last_hard_state);
    n->set_soft_state(event.current_state);
  }

  // From OK to NOT-OK
  if (old_hard_state != event.last_hard_state
      && old_hard_state == node_state::ok) {
    logging::debug(logging::medium)
      << "notification: state of service " << event.service_id
      << " of host " << event.host_id << " changed from 0 to "
      << event.last_hard_state << ", scheduling notification attempt";
    _notif_scheduler->remove_actions_of_node(id);
    action a;
    a.set_type(action::notification_processing);
    a.set_forwarded_type(action::notification_attempt);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
  // From NOT-OK to OK
  else if (old_hard_state != event.last_hard_state &&
           old_hard_state != node_state::ok) {
    _notif_scheduler->remove_actions_of_node(id);
    action a;
    a.set_type(action::notification_processing);
    a.set_forwarded_type(action::notification_up);;
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
}

/**
 *  Process a host status event.
 *
 *  @param event  The event to process.
 */
void stream::_process_host_status_event(neb::host_status const& event) {
  logging::debug(logging::medium)
    << "notification: processing status of host " << event.host_id
    << " (state " << event.last_hard_state << ")";

  node_id id(event.host_id);
  short old_hard_state;
  time_t when_to_schedule(::time(NULL) + 1);

  // Get the node corresponding to this id.
  {
    // Get the state lock.
    // TODO: The write lock here kills the perf. Use a read lock instead.
    //       and lock on an individual node level.
    std::unique_ptr<QWriteLocker> lock(_state.write_lock());
    node::ptr n = _state.get_node_by_id(id);
    if (!n)
      throw (exceptions::msg()
        << "notification: got an unknown host id: "
        << id.get_host_id());

    // Save the old state and copy the current state.
    old_hard_state = n->get_hard_state();
    n->set_hard_state(event.last_hard_state);
    n->set_soft_state(event.current_state);
  }

  // From OK to NOT-OK
  if (old_hard_state != event.last_hard_state &&
      old_hard_state == node_state::ok) {
    _notif_scheduler->remove_actions_of_node(id);
    action a;
    a.set_type(action::notification_processing);
    a.set_forwarded_type(action::notification_attempt);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
  // From NOT-OK to OK
  else if(old_hard_state != event.last_hard_state &&
          old_hard_state != node_state::ok) {
    _notif_scheduler->remove_actions_of_node(id);
    action a;
    a.set_type(action::notification_processing);
    a.set_forwarded_type(action::notification_up);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
}

/**
 *  Process an issue parent event.
 *
 *  @param event  The event to process.
 */
void stream::_process_issue_parent_event(
               correlation::issue_parent const& event) {
  // Node IDs.
  node_id child_id(event.child_host_id, event.child_service_id);
  node_id parent_id(event.parent_host_id, event.parent_service_id);

  // Get the state lock.
  // TODO: The write lock here kills the perf. Use a read lock instead.
  //       and lock on an individual node level.
  std::unique_ptr<QWriteLocker> lock(_state.write_lock());
  node::ptr n = _state.get_node_by_id(child_id);
  if (!n)
    throw (exceptions::msg()
           << "notification: got an unknown issue parent on node ("
           << child_id.get_host_id() << ", "
           << child_id.get_service_id() << ") by node ("
           << parent_id.get_host_id() << ", "
           << parent_id.get_service_id() << ")");

  // Log message.
  bool terminated_event((event.end_time != (time_t)-1)
                        && (event.end_time != (time_t)0));
  logging::debug(logging::medium)
    << "notification: node (" << child_id.get_host_id() << ", "
    << child_id.get_service_id() << ") "
    << (terminated_event ? "had" : "has") << " parent issue from node ("
    << parent_id.get_host_id() << ", " << parent_id.get_service_id()
    << ")";

  // Add a parent relationship between correlated node.
  if (!terminated_event)
    n->add_parent(parent_id);
  // Remove a parent relationship between correlated node.
  else
    n->remove_parent(parent_id);

  return ;
}

/**
 *  Process an ack event.
 *
 *  @param event  The event to process.
 */
void stream::_process_ack(neb::acknowledgement const& event) {
  objects::node_id id(event.host_id, event.service_id);

  logging::debug(logging::medium)
    << "notification: processing acknowledgement of node ("
    << event.host_id << ", " << event.service_id << ")";

  // End of ack.
  if (!event.deletion_time.is_null())
    return ;

  // Add the ack.
  if (event.notify_contacts &&
        (!event.notify_only_if_not_already_acknowledged
           || (event.notify_only_if_not_already_acknowledged
                 && !_node_cache.node_acknowledged(id)))) {
    time_t when_to_schedule(::time(NULL) + 1);
    action a;
    a.set_type(action::notification_processing);
    a.set_forwarded_type(action::notification_ack);
    a.set_node_id(id);
    _notif_scheduler->add_action_to_queue(when_to_schedule, a);
  }
}

/**
 *  Process a downtime event.
 *
 *  @param event  The event to process.
 */
void stream::_process_downtime(neb::downtime const& event) {
  objects::node_id id(event.host_id, event.service_id);

  logging::debug(logging::medium)
    << "notification: processing downtime of node ("
    << event.host_id << ", " << event.service_id << ") starting at "
    << event.start_time << " and ending at " << event.end_time;

  // End of downtime.
  if (!event.actual_end_time.is_null())
    return ;

  // Add the downtime.
  time_t when_to_schedule(::time(NULL) + 1);
  action a;
  a.set_type(action::notification_processing);
  a.set_forwarded_type(action::notification_downtime);
  a.set_node_id(id);
  _notif_scheduler->add_action_to_queue(when_to_schedule, a);
}
