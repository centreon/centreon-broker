/*
** Copyright 2009-2013 Merethis
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

#include <cstdlib>
#include <QPair>
#include <QSqlError>
#include <QSqlRecord>
#include <QThread>
#include <QVariant>
#include <QVector>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "mapping.hh"
#include "com/centreon/broker/sql/internal.hh"
#include "com/centreon/broker/sql/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Processing tables.
void (stream::* const stream::_correlation_processing_table[])(io::data const&) = {
  NULL,
  &stream::_process_engine,
  &stream::_process_host_state,
  &stream::_process_issue,
  &stream::_process_issue_parent,
  &stream::_process_service_state
};
void (stream::* const stream::_neb_processing_table[])(io::data const&) = {
  NULL,
  &stream::_process_acknowledgement,
  &stream::_process_comment,
  &stream::_process_custom_variable,
  &stream::_process_custom_variable_status,
  &stream::_process_downtime,
  &stream::_process_event_handler,
  &stream::_process_flapping_status,
  &stream::_process_host_check,
  &stream::_process_host_dependency,
  &stream::_process_host_group,
  &stream::_process_host_group_member,
  &stream::_process_host,
  &stream::_process_host_parent,
  &stream::_process_host_status,
  &stream::_process_instance,
  &stream::_process_instance_status,
  &stream::_process_log,
  &stream::_process_module,
  &stream::_process_notification,
  &stream::_process_service_check,
  &stream::_process_service_dependency,
  &stream::_process_service_group,
  &stream::_process_service_group_member,
  &stream::_process_service,
  &stream::_process_service_status
};

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Clean tables with data associated to the instance.
 *
 *  Rather than delete appropriate entries in tables, they are instead
 *  deactivated using a specific flag.
 *
 *  @param[in] instance_id Instance ID to remove.
 */
void stream::_clean_tables(int instance_id) {
  // Disable hosts and services.
  {
    std::ostringstream ss;
    ss << "UPDATE " << mapped_type<neb::host>::table
       << " LEFT JOIN " << mapped_type<neb::service>::table << " ON "
       << mapped_type<neb::host>::table << ".host_id="
       << mapped_type<neb::service>::table << ".host_id SET "
       << mapped_type<neb::host>::table << ".enabled=0, "
       << mapped_type<neb::service>::table << ".enabled=0"
          " WHERE " << mapped_type<neb::host>::table
       << ".instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove host groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_group>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove service groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::service_group>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove host dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_dependency>::table
       << " WHERE host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR dependent_host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove host parents.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_parent>::table
       << " WHERE child_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR parent_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove service dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::service_dependency>::table
       << " WHERE service_id IN ("
          "  SELECT services.service_id"
          "   FROM " << mapped_type<neb::service>::table << " AS services"
          "   JOIN " << mapped_type<neb::host>::table << " AS hosts"
          "   ON hosts.host_id=services.host_id WHERE hosts.instance_id="
       << instance_id << ")"
          " OR dependent_service_id IN ("
          "  SELECT services.service_id "
          "   FROM " << mapped_type<neb::service>::table << " AS services"
          "   JOIN " << mapped_type<neb::host>::table << " AS hosts"
          "   ON hosts.host_id=services.host_id WHERE hosts.instance_id="
       << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove list of modules.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::module>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove custom variables.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::custom_variable>::table
       << " USING " << mapped_type<neb::custom_variable>::table
       << " JOIN "  << mapped_type<neb::host>::table << " ON "
       << mapped_type<neb::custom_variable>::table << ".host_id="
       << mapped_type<neb::host>::table << ".host_id" << " WHERE "
       << mapped_type<neb::host>::table << ".instance_id="
       << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove comments.
  {
    std::ostringstream ss;
    ss << "UPDATE " << mapped_type<neb::comment>::table << " AS c"
          " JOIN " << mapped_type<neb::host>::table << " AS h"
          " ON c.host_id=h.host_id"
          " SET c.deletion_time=" << time(NULL)
       << " WHERE h.instance_id=" << instance_id
       << " AND c.persistent=0"
          " AND (c.deletion_time IS NULL OR c.deletion_time=0)";
    _execute(ss.str().c_str());
  }

  return ;
}

/**
 *  Execute a plain SQL query.
 *
 *  @param[in] query Query to execute.
 */
void stream::_execute(QString const& query) {
  logging::debug(logging::low) << "SQL: executing query: " << query;
  _db->exec(query);
  QSqlError err(_db->lastError());
  if (err.type() != QSqlError::NoError)
    throw (exceptions::msg() << "SQL: " << err.text());
  return ;
}

/**
 *  Execute a prepare SQL query.
 *
 *  @param[in] query Query to execute.
 */
void stream::_execute(QSqlQuery& query) {
  logging::debug(logging::low) << "SQL: executing query";
  if (!query.exec())
    throw (exceptions::msg() << "SQL: " << query.lastError().text());
  return ;
}

/**
 *  Insert an object in the DB using its mapping.
 *
 *  @param[in] t Object to insert.
 */
template <typename T>
bool stream::_insert(T const& t) {
  // Build query string.
  QString query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->name);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->field);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");

  // Execute query.
  QSqlQuery q(*_db);
  bool ret(q.prepare(query));
  if (ret) {
    q << t;
    ret = q.exec();
  }

  return (ret);
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Prepare insert queries.
  _prepare_insert<neb::acknowledgement>(_acknowledgement_insert);
  _prepare_insert<neb::comment>(_comment_insert);
  _prepare_insert<neb::custom_variable>(_custom_variable_insert);
  _prepare_insert<neb::downtime>(_downtime_insert);
  _prepare_insert<neb::event_handler>(_event_handler_insert);
  _prepare_insert<neb::flapping_status>(_flapping_status_insert);
  _prepare_insert<neb::host>(_host_insert);
  _prepare_insert<neb::host_dependency>(_host_dependency_insert);
  _prepare_insert<neb::host_group>(_host_group_insert);
  _prepare_insert<neb::instance>(_instance_insert);
  _prepare_insert<neb::notification>(_notification_insert);
  _prepare_insert<neb::service>(_service_insert);
  _prepare_insert<neb::service_dependency>(_service_dependency_insert);
  _prepare_insert<neb::service_group>(_service_group_insert);
  _prepare_insert<correlation::host_state>(_host_state_insert);
  _prepare_insert<correlation::issue>(_issue_insert);
  _prepare_insert<correlation::service_state>(_service_state_insert);
  _issue_parent_insert.reset(new QSqlQuery(*_db));
  {
    QString query(
      "INSERT INTO issues_issues_parents (child_id, end_time, start_time, parent_id)"
      " VALUES (:child_id, :end_time, :start_time, :parent_id)");
    logging::info(logging::low) << "SQL: preparing statement: "
      << query;
    _issue_parent_insert->prepare(query);
  }

  // Prepare update queries.
  QVector<QPair<QString, bool> > id;

  id.clear();
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::acknowledgement>(_acknowledgement_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("entry_time"), false));
  _prepare_update<neb::comment>(_comment_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("name"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::custom_variable>(_custom_variable_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("name"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::custom_variable_status>(
    _custom_variable_status_update,
    id);

  {
    std::ostringstream oss;
    oss << "UPDATE " << mapped_type<neb::downtime>::table
        << " SET actual_end_time=GREATEST(COALESCE(actual_end_time, -1), :actual_end_time),"
           "     actual_start_time=COALESCE(actual_start_time, :actual_start_time),"
           "     author=:author, cancelled=:cancelled, comment_data=:comment_data,"
           "     deletion_time=:deletion_time, duration=:duration, end_time=:end_time,"
           "     fixed=:fixed, instance_id=:instance_id, internal_id=:internal_id,"
           "     start_time=:start_time, started=:started, triggered_by=:triggered_by,"
           "     type=:type"
           " WHERE entry_time=:entry_time"
           "        AND host_id=:host_id"
           "        AND COALESCE(service_id, -1)=COALESCE(:service_id, -1)";
    QString query(oss.str().c_str());
    logging::info(logging::low)
      << "SQL: preparing statement: " << query;
    _downtime_update.reset(new QSqlQuery(*_db));
    if (!_downtime_update->prepare(query))
      throw (exceptions::msg() << "SQL: cannot prepare statement: "
             << _downtime_update->lastError().text() << ": " << query);
  }

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<neb::event_handler>(_event_handler_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("event_time"), false));
  _prepare_update<neb::flapping_status>(_flapping_status_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host>(_host_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host_check>(_host_check_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("dependent_host_id"), false));
  _prepare_update<neb::host_dependency>(_host_dependency_update, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  id.push_back(qMakePair(QString("name"), false));
  _prepare_update<neb::host_group>(_host_group_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host_status>(_host_status_update, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  _prepare_update<neb::instance>(_instance_update, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  _prepare_update<neb::instance_status>(_instance_status_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<neb::notification>(_notification_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service>(_service_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service_check>(_service_check_update, id);

  id.clear();
  id.push_back(qMakePair(QString("dependent_host_id"), false));
  id.push_back(qMakePair(QString("dependent_service_id"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service_dependency>(_service_dependency_update, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  id.push_back(qMakePair(QString("name"), false));
  _prepare_update<neb::service_group>(_service_group_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service_status>(_service_status_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::host_state>(_host_state_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::issue>(_issue_update, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::service_state>(_service_state_update,
    id);

  _issue_parent_update.reset(new QSqlQuery(*_db));
  {
    QString query(
      "UPDATE issues_issues_parents SET end_time=:end_time"
      " WHERE child_id=:child_id"
      "       AND start_time=:start_time"
      "       AND parent_id=:parent_id");
    logging::info(logging::low) << "SQL: preparing statement: "
      << query;
    _issue_parent_update->prepare(query);
  }

  return ;
}

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 */
template <typename T>
bool stream::_prepare_insert(std::auto_ptr<QSqlQuery>& st) {
  // Build query string.
  QString query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->name);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->field);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");
  logging::info(logging::low)
    << "SQL: preparing statement: " << query;

  // Prepare statement.
  st.reset(new QSqlQuery(*_db));
  return (st->prepare(query));
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 *  @param[in]  id List of fields that form an UNIQUE.
 */
template <typename T>
bool stream::_prepare_update(
               std::auto_ptr<QSqlQuery>& st,
               QVector<QPair<QString, bool> > const& id) {
  // Build query string.
  QString query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    bool found(false);
    for (QVector<QPair<QString, bool> >::const_iterator
           it2 = id.begin(), end2 = id.end();
         it2 != end2;
         ++it2)
      if (it->name == it2->first)
        found = true;
    if (!found) {
      query.append(it->name);
      query.append("=");
      query.append(it->field);
      query.append(", ");
    }
  }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (QVector<QPair<QString, bool> >::const_iterator
         it = id.begin(), end = id.end();
       it != end;
       ++it) {
    if (it->second) {
      query.append("COALESCE(");
      query.append(it->first);
      query.append(", -1)=COALESCE(:");
      query.append(it->first);
      query.append(", -1)");
    }
    else {
      query.append(it->first);
      query.append("=:");
      query.append(it->first);
    }
    query.append(" AND ");
  }
  query.resize(query.size() - 5);
  logging::info(logging::low)
    << "SQL: preparing statement: " << query;

  // Prepare statement.
  st.reset(new QSqlQuery(*_db));
  return (st->prepare(query));
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void stream::_process_acknowledgement(io::data const& e) {
  // Cast object.
  neb::acknowledgement const&
    ack(*static_cast<neb::acknowledgement const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing acknowledgement event (instance: "
    << ack.instance_id << ", host: " << ack.host_id << ", service: "
    << ack.service_id << ", entry time: " << ack.entry_time
    << ", deletion time: " << ack.deletion_time << ")";

  // Processing.
  _update_on_none_insert(
    *_acknowledgement_insert,
    *_acknowledgement_update,
    ack);

  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e Uncasted comment.
 */
void stream::_process_comment(io::data const& e) {
  // Cast object.
  neb::comment const& com(*static_cast<neb::comment const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing comment event"
       " (instance: " << com.instance_id << ", host: " << com.host_id
    << ", service: " << com.service_id << ", entry time: "
    << com.entry_time << ", expire time: " << com.expire_time
    << ", deletion time: " << com.deletion_time << ", id: "
    << com.internal_id << ")";

  // Processing.
  if (com.host_id)
    _update_on_none_insert(*_comment_insert,
      *_comment_update,
      *static_cast<neb::comment const*>(&e));
  else
    logging::error(logging::low) << "SQL: could not process event " \
      "which does not have an host ID";

  return ;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void stream::_process_custom_variable(io::data const& e) {
  // Cast object.
  neb::custom_variable const&
    cv(*static_cast<neb::custom_variable const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing custom variable event (host: " << cv.host_id
    << ", service: " << cv.service_id << ", name: " << cv.name << ")";

  // Processing.
  _update_on_none_insert(
    *_custom_variable_insert,
    *_custom_variable_update,
    cv);

  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void stream::_process_custom_variable_status(io::data const& e) {
  // Cast object.
  neb::custom_variable_status const&
    cvs(*static_cast<neb::custom_variable_status const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing custom variable status event (host: "
    << cvs.host_id << ", service: " << cvs.service_id << ", name: "
    << cvs.name << ", update time: " << cvs.update_time << ")";

  // Processing.
  *_custom_variable_status_update << cvs;
  _execute(*_custom_variable_status_update);
  if (_custom_variable_status_update->numRowsAffected() != 1)
    logging::error(logging::medium) << "SQL: custom variable ("
      << cvs.host_id << ", " << cvs.service_id << ", " << cvs.name
      << ") was not updated because it was not found in database";

  return ;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 */
void stream::_process_downtime(io::data const& e) {
  // Cast object.
  neb::downtime const&
    d(*static_cast<neb::downtime const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing downtime event (instance: " << d.instance_id
    << ", host: " << d.host_id << ", service: " << d.service_id
    << ", start time: " << d.start_time << ", end_time: " << d.end_time
    << ", actual start time: " << d.actual_start_time
    << ", actual end time: " << d.actual_end_time << ", duration: "
    << d.duration << ", entry time: " << d.entry_time
    << ", deletion time: " << d.deletion_time << ")";

  // Only update in case of downtime termination.
  if (d.actual_end_time) {
    *_downtime_update << d;
    _execute(*_downtime_update);
  }
  // Update or insert if no entry was found, as long as the downtime
  // is valid.
  else
    _update_on_none_insert(
      *_downtime_insert,
      *_downtime_update,
      d);

  return ;
}

/**
 *  Process a correlation engine event.
 *
 *  @param[in] e Uncasted correlation engine event.
 */
void stream::_process_engine(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing correlation engine event";

  // Cast event.
  correlation::engine_state const&
    es(*static_cast<correlation::engine_state const*>(&e));

  // Close issues.
  if (es.started) {
    time_t now(time(NULL));
    {
      std::ostringstream ss;
      ss << "UPDATE issues SET end_time=" << now
         << " WHERE end_time=0 OR end_time IS NULL";
      _execute(ss.str().c_str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE issues_issues_parents SET end_time=" << now
         << " WHERE end_time=0 OR end_time IS NULL";
      _execute(ss.str().c_str());
    }
  }

  return ;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void stream::_process_event_handler(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing event handler event";

  // Processing.
  _update_on_none_insert(*_event_handler_insert,
    *_event_handler_update,
    *static_cast<neb::event_handler const*>(&e));

  return ;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void stream::_process_flapping_status(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing flapping status event";

  // Processing.
  _update_on_none_insert(*_flapping_status_insert,
    *_flapping_status_update,
    *static_cast<neb::flapping_status const*>(&e));

  return ;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void stream::_process_host(io::data const& e) {
  // Cast object.
  neb::host const& h(*static_cast<neb::host const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing host event"
       " (instance: " << h.instance_id << ", id: "
    << h.host_id << ", name: " << h.host_name << ")";

  // Processing
  if (h.host_id)
    _update_on_none_insert(*_host_insert, *_host_update, h);
  else
    logging::error(logging::high) << "SQL: host '" << h.host_name
      << "' of instance " << h.instance_id << " has no ID";

  return ;
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 */
void stream::_process_host_check(io::data const& e) {
  // Cast object.
  neb::host_check const&
    hc(*static_cast<neb::host_check const*>(&e));

  time_t now(time(NULL));
  if (hc.check_type                // - passive result
      || !hc.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
      || (hc.next_check >= now)    // - some future status as already
                                   //   been generated but not yet
                                   //   processed
      || !hc.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing host check event (host: " << hc.host_id
      << ", command: " << hc.command_line << ")";

    // Processing.
    *_host_check_update << hc;
    _execute(*_host_check_update);
    if (_host_check_update->numRowsAffected() != 1)
      logging::error(logging::medium) << "SQL: host check could not "
           "be updated because host " << hc.host_id
        << " was not found in database";
  }
  else
    // Do nothing.
    logging::info(logging::medium)
      << "SQL: not processing host check event (host: " << hc.host_id
      << ", command: " << hc.command_line << ", check type: "
      << hc.check_type << ", next check: " << hc.next_check << ", now: "
      << now << ")";

  return ;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 */
void stream::_process_host_dependency(io::data const& e) {
  // Cast object.
  neb::host_dependency const&
    hd(*static_cast<neb::host_dependency const*>(&e));

  // Insert/Update.
  if (hd.enabled) {
    logging::info(logging::medium)
      << "SQL: enabling host dependency of " << hd.dependent_host_id
      << " on " << hd.host_id;
    _update_on_none_insert(
      *_host_dependency_insert,
      *_host_dependency_update,
      hd);
  }
  // Delete.
  else {
    logging::info(logging::medium)
      << "SQL: removing host dependency of " << hd.dependent_host_id
      << " on " << hd.host_id;
    std::ostringstream oss;
    oss << "DELETE FROM hosts_hosts_dependencies "
           "WHERE dependent_host_id=" << hd.dependent_host_id
        << "  AND host_id=" << hd.host_id;
    _execute(oss.str().c_str());
  }

  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void stream::_process_host_group(io::data const& e) {
  // Cast object.
  neb::host_group const&
    hg(*static_cast<neb::host_group const*>(&e));

  // Insert/Update.
  if (hg.enabled) {
    logging::info(logging::medium) << "SQL: enabling host group '"
      << hg.name << "' of instance " << hg.instance_id;
    _update_on_none_insert(
      *_host_group_insert,
      *_host_group_update,
      hg);
  }
  // Delete.
  else {
    logging::info(logging::medium)
      << "SQL: removing host group '" << hg.name
      << "' on instance " << hg.instance_id;
    QSqlQuery q(*_db);
    q.prepare(
        "DELETE FROM hostgroups "
        "WHERE instance_id=:instance_id"
        "  AND name=:name");
    q.bindValue(":instance_id", hg.instance_id);
    q.bindValue(":name", hg.name);
    _execute(q);
  }

  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void stream::_process_host_group_member(io::data const& e) {
  // Cast object.
  neb::host_group_member const&
    hgm(*static_cast<neb::host_group_member const*>(&e));

  // Insert.
  if (hgm.enabled) {
    // Log message.
    logging::info(logging::medium)
      << "SQL: enabling host group member (group: "
      << hgm.group << ", instance: " << hgm.instance_id
      << ", host: " << hgm.host_id << ")";

    // Fetch host group ID.
    std::ostringstream ss;
    ss << "SELECT hostgroup_id FROM "
       << mapped_type<neb::host_group>::table
       << " WHERE instance_id=" << hgm.instance_id
       << " AND name=\"" << hgm.group.toStdString() << "\"";
    QSqlQuery q(*_db);
    logging::info(logging::low)
      << "SQL: host group member: " << ss.str().c_str();
    if (q.exec(ss.str().c_str()) && q.next()) {
      // Fetch hostgroup ID.
      int hostgroup_id(q.value(0).toInt());
      logging::debug(logging::medium)
        << "SQL: fetch hostgroup of id " << hostgroup_id;

      // Insert hostgroup membership.
      std::ostringstream oss;
      oss << "INSERT INTO "
          << mapped_type<neb::host_group_member>::table
          << " (host_id, hostgroup_id) VALUES("
          << hgm.host_id << ", "
          << hostgroup_id << ")";
      logging::info(logging::low) << "SQL: executing query: "
        << oss.str().c_str();
      _db->exec(oss.str().c_str());
    }
    else
      logging::info(logging::high)
        << "SQL: discarding membership between host " << hgm.host_id
        << " and hostgroup (" << hgm.instance_id << ", " << hgm.group
        << ")";
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
      << "SQL: removing host group member (group: "
      << hgm.group << ", instance: " << hgm.instance_id
      << ", host: " << hgm.host_id << ")";

    // Build query.
    std::ostringstream oss;
    oss << "DELETE hgm"
      "  FROM " << mapped_type<neb::host_group_member>::table << " AS hgm "
      "  INNER JOIN " << mapped_type<neb::host_group>::table << " AS hg "
      "  ON hgm.hostgroup_id=hg.hostgroup_id "
      "  WHERE hg.name=:group"
      "    AND hgm.host_id=:host_id"
      "    AND hg.instance_id=:instance_id ";

    // Execute query.
    QSqlQuery q(*_db);
    if (!q.prepare(oss.str().c_str()))
      throw (exceptions::msg()
             << "SQL: cannot prepare host group membership deletion statement: "
             << q.lastError().text());
    q << hgm;
    _execute(q);
  }

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void stream::_process_host_parent(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing host parent event";

  // Processing (errors are silently ignored).
  _insert(*static_cast<neb::host_parent const*>(&e));

  return ;
}

/**
 *  Process a host state event.
 *
 *  @param[in] e Uncasted host state.
 */
void stream::_process_host_state(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing host state event";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(*_host_state_insert,
      *_host_state_update,
      *static_cast<correlation::host_state const*>(&e));
  }

  return ;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void stream::_process_host_status(io::data const& e) {
  // Processed object.
  neb::host_status const&
    hs(*static_cast<neb::host_status const*>(&e));

  time_t now(time(NULL));
  if (hs.check_type                // - passive result
      || !hs.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
      || (hs.next_check >= now)    // - some future status as already
                                   //   been generated but not yet
                                   //   processed
      || !hs.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing host status event (id: " << hs.host_id
      << ", last check: " << hs.last_check << ", state ("
      << hs.current_state << ", " << hs.state_type << "))";

    // Processing.
    *_host_status_update << hs;
    _execute(*_host_status_update);
    if (_host_status_update->numRowsAffected() != 1)
      logging::error(logging::medium) << "SQL: host could not be "
           "updated because host " << hs.host_id
        << " was not found in database";
  }
  else
    // Do nothing.
    logging::info(logging::medium)
      << "SQL: not processing host status event (id: " << hs.host_id
      << ", check type: " << hs.check_type << ", last check: "
      << hs.last_check << ", next check: " << hs.next_check
      << ", now: " << now << ", state (" << hs.current_state
      << ", " << hs.state_type  << "))";

  return ;
}

/**
 *  Process an instance event.
 *
 *  @param[in] e Uncasted instance.
 */
void stream::_process_instance(io::data const& e) {
  // Cast object.
  neb::instance const& i(*static_cast<neb::instance const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing instance event"
    << "(id: " << i.id << ", name: " << i.name << ", running: "
    << (i.is_running ? "yes" : "no") << ")";

  // Clean tables.
  _clean_tables(i.id);

  // Processing.
  _update_on_none_insert(*_instance_insert, *_instance_update, i);

  return ;
}

/**
 *  Process an instance status event.
 *
 *  @param[in] e Uncasted instance status.
 */
void stream::_process_instance_status(io::data const& e) {
  // Cast object.
  neb::instance_status const&
    is(*static_cast<neb::instance_status const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing instance status event (id: " << is.id
    << ", last alive: " << is.last_alive << ")";

  // Processing.
  *_instance_status_update << is;
  _execute(*_instance_status_update);
  if (_instance_status_update->numRowsAffected() != 1)
    logging::error(logging::medium) << "SQL: instance "
      << is.id << " was not updated because no matching entry "
         "was found in database";

  return ;
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void stream::_process_issue(io::data const& e) {
  // Issue object.
  correlation::issue const&
    i(*static_cast<correlation::issue const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing issue event (node: (" << i.host_id << ", "
    << i.service_id << "), start time: " << i.start_time
    << ", end_time: " << i.end_time << ", ack time: " << i.ack_time
    << ")";

  // Processing.
  _update_on_none_insert(*_issue_insert, *_issue_update, i);

  return ;
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void stream::_process_issue_parent(io::data const& e) {
  // Issue parent object.
  correlation::issue_parent const&
    ip(*static_cast<correlation::issue_parent const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing issue parent "
       "event (child: (" << ip.child_host_id << ", "
    << ip.child_service_id << ", " << ip.child_start_time
    << "), parent: (" << ip.parent_host_id << ", "
    << ip.parent_service_id << ", " << ip.parent_start_time
    << "), start time: " << ip.start_time << ", end time: "
    << ip.end_time << ")";

  int child_id;
  int parent_id;

  // Get child ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<correlation::issue>::table << " WHERE host_id="
          << ip.child_host_id << " AND service_id";
    if (ip.child_service_id)
      query << "=" << ip.child_service_id;
    else
      query << " IS NULL";
    query << " AND start_time=" << ip.child_start_time;
    logging::info(logging::low) << "SQL: issue parent: "
      << query.str().c_str();
    QSqlQuery q(*_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      child_id = q.value(0).toInt();
      logging::debug(logging::low)
        << "SQL: child issue ID: " << child_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch child issue "
                  "ID (host=" << ip.child_host_id << ", service="
               << ip.child_service_id << ", start="
               << ip.child_start_time << ")");
  }

  // Get parent ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<correlation::issue>::table << " WHERE host_id="
          << ip.parent_host_id << " AND service_id";
    if (ip.parent_service_id)
      query << "=" << ip.parent_service_id;
    else
      query << " IS NULL";
    query << " AND start_time=" << ip.parent_start_time;
    logging::info(logging::low) << "SQL: issue child: "
      << query.str().c_str();
    QSqlQuery q(*_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      parent_id = q.value(0).toInt();
      logging::debug(logging::low) << "SQL: parent issue ID: "
        << parent_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch parent issue "
                  "ID (host=" << ip.parent_host_id << ", service="
               << ip.parent_service_id << ", start="
               << ip.parent_start_time << ")");
  }

  // End of parenting.
  if (ip.end_time)
    _issue_parent_update->bindValue(
      ":end_time",
      static_cast<unsigned int>(ip.end_time));
  else
    _issue_parent_update->bindValue(
      ":end_time",
      QVariant(QVariant::Int));
  _issue_parent_update->bindValue(":child_id", child_id);
  _issue_parent_update->bindValue(
    ":start_time",
    static_cast<unsigned int>(ip.start_time));
  _issue_parent_update->bindValue(":parent_id", parent_id);
  logging::debug(logging::low) << "SQL: updating issue parenting entry";
  if (!_issue_parent_update->exec())
    throw (exceptions::msg() << "SQL: issue parent update query failed: "
             << _issue_parent_update->lastError().text());
  if (_issue_parent_update->numRowsAffected() <= 0) {
    if (ip.end_time)
      _issue_parent_insert->bindValue(
        ":end_time",
        static_cast<unsigned int>(ip.end_time));
    else
      _issue_parent_insert->bindValue(
        ":end_time",
        QVariant(QVariant::Int));
    _issue_parent_insert->bindValue(":child_id", child_id);
    _issue_parent_insert->bindValue(
      ":start_time",
      static_cast<unsigned int>(ip.start_time));
    _issue_parent_insert->bindValue(":parent_id", parent_id);
    logging::debug(logging::low) << "SQL: inserting issue parenting";
    if (!_issue_parent_insert->exec())
      throw (exceptions::msg() << "SQL: issue parent insert query "
               "failed: " << _issue_parent_insert->lastError().text());
  }

  return ;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void stream::_process_log(io::data const& e) {
  // Fetch proper structure.
  neb::log_entry const& le(
    *static_cast<neb::log_entry const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing log event (ctime: "
    << le.c_time << ")";

  // Fetch issue ID (if any).
  int issue;
  if (le.issue_start_time) {
    std::ostringstream ss;
    ss << "SELECT issue_id FROM "
       << mapped_type<correlation::issue>::table
       << " WHERE host_id=" << le.host_id
       << " AND service_id=" << le.service_id
       << " AND start_time=" << le.issue_start_time;
    logging::info(logging::low) << "SQL: executing query: "
      << ss.str().c_str();
    QSqlQuery q(*_db);
    if (q.exec(ss.str().c_str()) && q.next())
      issue = q.value(0).toInt();
    else
      issue = 0;
  }
  else
    issue = 0;

  // Build insertion query.
  char const* field("issue_id");
  QString query;
  query = "INSERT INTO ";
  query.append(mapped_type<neb::log_entry>::table);
  query.append("(");
  for (std::vector<db_mapped_entry<neb::log_entry> >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(it->name);
    query.append(", ");
  }
  query.append(field);
  query.append(") VALUES(");
  for (std::vector<db_mapped_entry<neb::log_entry> >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(it->field);
    query.append(", ");
  }
  query.append(":");
  query.append(field);
  query.append(")");

  // Execute query.
  logging::info(logging::low)
    << "SQL: executing query: " << query;
  QSqlQuery q(*_db);
  q.prepare(query);
  q << le;
  q.bindValue(field, issue);
  _execute(q);

  return ;
}

/**
 *  Process a module event.
 *
 *  @param[in] e Uncasted module.
 */
void stream::_process_module(io::data const& e) {
  // Cast object.
  neb::module const& m(*static_cast<neb::module const*>(&e));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing module event (instance: " << m.instance_id
    << ", filename: " << m.filename << ", loaded: "
    << (m.loaded ? "yes" : "no") << ")";

  // Processing.
  if (m.enabled)
    _insert(m);
  else {
    QSqlQuery q(*_db);
    q.prepare(
      "DELETE FROM modules "
      "WHERE instance_id=:instance_id"
      "  AND filename=:filename");
    q.bindValue(":instance_id", m.instance_id);
    q.bindValue(":filename", m.filename);
    _execute(q);
  }

  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing notification event";

  // Processing.
  _update_on_none_insert(*_notification_insert,
    *_notification_update,
    *static_cast<neb::notification const*>(&e));

  return ;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void stream::_process_service(io::data const& e) {
  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(&e));

  // Log message.
  logging::info(logging::medium) << "SQL: processing service event "
       "(host id: " << s.host_id << ", service_id: " << s.service_id
    << ", description: " << s.service_description << ")";

  // Processing.
  if (s.host_id && s.service_id) {
    _update_on_none_insert(*_service_insert,
      *_service_update,
      s);
  }
  else
    logging::error(logging::high) << "SQL: service '"
      << s.service_description << "' has no host ID or no service ID";

  return ;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void stream::_process_service_check(io::data const& e) {
  // Cast object.
  neb::service_check const&
    sc(*static_cast<neb::service_check const*>(&e));

  time_t now(time(NULL));
  if (sc.check_type                // - passive result
      || !sc.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
      || (sc.next_check >= now)    // - some future status as already
                                   //   been generated but not yet
                                   //   processed
      || !sc.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing service check event (host: " << sc.host_id
      << ", service: " << sc.service_id << ", command: "
      << sc.command_line << ")";

    // Processing.
    *_service_check_update << sc;
    _execute(*_service_check_update);
    if (_service_check_update->numRowsAffected() != 1)
      logging::error(logging::medium) << "SQL: service check could "
           "not be updated because service (" << sc.host_id << ", "
        << sc.service_id << ") was not found in database";
  }
  else
    // Do nothing.
    logging::info(logging::medium)
      << "SQL: not processing service check event (host: " << sc.host_id
      << ", service: " << sc.service_id << ", command: "
      << sc.command_line << ", check_type: " << sc.check_type
      << ", next_check: " << sc.next_check << ", now: " << now << ")";

  return ;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 */
void stream::_process_service_dependency(io::data const& e) {
  // Cast object.
  neb::service_dependency const&
    sd(*static_cast<neb::service_dependency const*>(&e));

  // Insert/Update.
  if (sd.enabled) {
    logging::info(logging::medium)
      << "SQL: enabling service dependency of (" << sd.dependent_host_id
      << ", " << sd.dependent_service_id << ") on (" << sd.host_id
      << ", " << sd.service_id << ")";
    _update_on_none_insert(
      *_service_dependency_insert,
      *_service_dependency_update,
      sd);
  }
  // Delete.
  else {
    logging::info(logging::medium)
      << "SQL: removing service dependency of (" << sd.dependent_host_id
      << ", " << sd.dependent_service_id << ") on (" << sd.host_id
      << ", " << sd.service_id << ")";
    std::ostringstream oss;
    oss << "DELETE FROM services_services_dependencies "
           "WHERE dependent_host_id=" << sd.dependent_host_id
        << "  AND dependent_service_id=" << sd.dependent_service_id
        << "  AND host_id=" << sd.host_id
        << "  AND service_id=" << sd.service_id;
    _execute(oss.str().c_str());
  }

  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void stream::_process_service_group(io::data const& e) {
  // Cast object.
  neb::service_group const&
    sg(*static_cast<neb::service_group const*>(&e));

  // Insert/Update.
  if (sg.enabled) {
    logging::info(logging::medium) << "SQL: enabling service group '"
      << sg.name << "' of instance: " << sg.instance_id;
    _update_on_none_insert(
      *_service_group_insert,
      *_service_group_update,
      sg);
  }
  // Delete.
  else {
    logging::info(logging::medium)
      << "SQL: removing service group '" << sg.name
      << "' on instance " << sg.instance_id;
    QSqlQuery q(*_db);
    q.prepare(
        "DELETE FROM servicegroups "
        "WHERE instance_id=:instance_id"
        "  AND name=:name");
    q.bindValue(":instance_id", sg.instance_id);
    q.bindValue(":name", sg.name);
    _execute(q);
  }

  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void stream::_process_service_group_member(io::data const& e) {
  // Cast object.
  neb::service_group_member const&
    sgm(*static_cast<neb::service_group_member const*>(&e));

  // Insert.
  if (sgm.enabled) {
    // Log message.
    logging::info(logging::medium)
      << "SQL: enabling service group member (group: "
      << sgm.group << ", instance: " << sgm.instance_id << ", host: "
      << sgm.host_id << ", service: " << sgm.service_id << ")";

    // Fetch service group ID.
    std::ostringstream ss;
    ss << "SELECT servicegroup_id FROM "
       << mapped_type<neb::service_group>::table
       << " WHERE instance_id=" << sgm.instance_id
       << " AND name=\"" << sgm.group.toStdString() << "\"";
    QSqlQuery q(*_db);
    logging::info(logging::low) << "SQL: executing query: "
      << ss.str().c_str();
    if (q.exec(ss.str().c_str()) && q.next()) {
      // Fetch servicegroup ID.
      int servicegroup_id(q.value(0).toInt());
      logging::debug(logging::medium)
        << "SQL: fetch servicegroup of id " << servicegroup_id;

      // Insert servicegroup membership.
      std::ostringstream oss;
      oss << "INSERT INTO "
          << mapped_type<neb::service_group_member>::table
          << " (host_id, service_id, servicegroup_id) VALUES("
          << sgm.host_id << ", "
          << sgm.service_id << ", "
          << servicegroup_id << ")";
      logging::info(logging::low) << "SQL: executing query: "
        << oss.str().c_str();
      _db->exec(oss.str().c_str());
    }
    else
      logging::info(logging::high)
        << "SQL: discarding membership between service ("
        << sgm.host_id << ", " << sgm.service_id << ") and servicegroup ("
        << sgm.instance_id << ", " << sgm.group << ")";
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
      << "SQL: removing service group member (group: "
      << sgm.group << ", instance: " << sgm.instance_id << ", host: "
      << sgm.host_id << ", service: " << sgm.service_id << ")";

    // Build query.
    std::ostringstream oss;
    oss << "DELETE sgm"
           "  FROM " << mapped_type<neb::service_group_member>::table << " AS sgm "
           "  INNER JOIN " << mapped_type<neb::service_group>::table << " AS sg "
           "  ON sgm.servicegroup_id=sg.servicegroup_id "
           "  WHERE sg.name=:group "
           "    AND sgm.host_id=:host_id "
           "    AND sg.instance_id=:instance_id "
           "    AND sgm.service_id=:service_id";

    // Execute query.
    QSqlQuery q(*_db);
    if (!q.prepare(oss.str().c_str()))
      throw (exceptions::msg()
             << "SQL: cannot prepare service group membership deletion statement: "
             << q.lastError().text());
    q << sgm;
    _execute(q);
  }

  return ;
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(io::data const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing service state event";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(*_service_state_insert,
      *_service_state_update,
      *static_cast<correlation::service_state const*>(&e));
  }

  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void stream::_process_service_status(io::data const& e) {
  // Processed object.
  neb::service_status const&
    ss(*static_cast<neb::service_status const*>(&e));

  time_t now(time(NULL));
  if (ss.check_type                // - passive result
      || !ss.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
      || (ss.next_check >= now)    // - some future status as already
                                   //   been generated but not yet
                                   //   processed
      || !ss.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing service status event (host: "
      << ss.host_id << ", service: " << ss.service_id
      << ", last check: " << ss.last_check << ", state ("
      << ss.current_state << ", " << ss.state_type << "))";

    // Processing.
    *_service_status_update << ss;
    _execute(*_service_status_update);
    if (_service_status_update->numRowsAffected() != 1)
      logging::error(logging::medium) << "SQL: service could not be "
           "updated because service (" << ss.host_id << ", "
        << ss.service_id << ") was not found in database";
  }
  else
    // Do nothing.
    logging::info(logging::medium)
      << "SQL: not processing service status event (host: "
      << ss.host_id << ", service: " << ss.service_id
      << ", check_type: " << ss.check_type << ", last check: "
      << ss.last_check << ", next_check: " << ss.next_check << ", now: "
      << now << ", state (" << ss.current_state << ", "
      << ss.state_type << "))";

  return ;
}

/**
 *  Reset statements.
 */
void stream::_unprepare() {
  _acknowledgement_insert.reset();
  _acknowledgement_update.reset();
  _comment_insert.reset();
  _comment_update.reset();
  _custom_variable_insert.reset();
  _custom_variable_update.reset();
  _custom_variable_status_update.reset();
  _downtime_insert.reset();
  _downtime_update.reset();
  _event_handler_insert.reset();
  _event_handler_update.reset();
  _flapping_status_insert.reset();
  _flapping_status_update.reset();
  _host_insert.reset();
  _host_update.reset();
  _host_check_update.reset();
  _host_dependency_insert.reset();
  _host_dependency_update.reset();
  _host_group_insert.reset();
  _host_group_update.reset();
  _host_state_insert.reset();
  _host_state_update.reset();
  _host_status_update.reset();
  _instance_insert.reset();
  _instance_update.reset();
  _instance_status_update.reset();
  _issue_insert.reset();
  _issue_update.reset();
  _notification_insert.reset();
  _notification_update.reset();
  _service_insert.reset();
  _service_update.reset();
  _service_check_update.reset();
  _service_dependency_insert.reset();
  _service_dependency_update.reset();
  _service_group_insert.reset();
  _service_group_update.reset();
  _service_state_insert.reset();
  _service_state_update.reset();
  _service_status_update.reset();
  return ;
}

template <typename T>
void stream::_update_on_none_insert(QSqlQuery& ins,
                                    QSqlQuery& up,
                                    T& t) {
  // Try udpate.
  up << t;
  _execute(up);

  // Try insertion.
  if (up.numRowsAffected() != 1) {
    ins << t;
    _execute(ins);
  }

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] type              Database type.
 *  @param[in] host              Database host.
 *  @param[in] port              Database port.
 *  @param[in] user              User.
 *  @param[in] password          Password.
 *  @param[in] db                Database name.
 *  @param[in] qpt               Queries per transaction.
 *  @param[in] check_replication true to check replication status.
 *  @param[in] wse               With state events.
 */
stream::stream(
          QString const& type,
          QString const& host,
          unsigned short port,
          QString const& user,
          QString const& password,
          QString const& db,
          unsigned int qpt,
          bool check_replication,
          bool wse)
  : _process_out(true),
    _queries_per_transaction((qpt >= 2) ? qpt : 1),
    _transaction_queries(0),
    _with_state_events(wse) {
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
        throw (exceptions::msg() << "SQL: could not open SQL database: "
               << _db->lastError().text());
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "SQL: checking replication status";
      QSqlQuery q(*_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "SQL: could not check replication status";
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "SQL: database is not under replication";
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
              throw (exceptions::msg() << "SQL: replication is not "
                          "complete: " << field << "="
                       << q.value(i).toString());
          logging::info(logging::medium)
            << "SQL: database replication is complete, "
               "connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "SQL: NOT checking replication status";

    // Prepare queries.
    _prepare();

    // First transaction.
    if (_queries_per_transaction > 1)
      _db->transaction();
  }
  catch (...) {
    // Unprepare queries.
    _unprepare();

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
        throw (exceptions::msg() << "SQL: could not open SQL database: "
               << _db->lastError().text());
    }

    // Prepare queries.
    _prepare();

    // First transaction.
    if (_queries_per_transaction > 1)
      _db->transaction();
  }
  catch (...) {
    // Unprepare queries.
    _unprepare();

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

  // Reset statements.
  _unprepare();

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
 *  Initialize SQL layer.
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
         << "SQL: attempt to read from a SQL stream (not supported yet)");
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
           << "SQL stream is shutdown");

  // Check that data exists.
  unsigned int retval(1);
  if (!data.isNull()) {
    unsigned int type(data->type());
    unsigned short cat(io::events::category_of_type(type));
    unsigned short elem(io::events::element_of_type(type));
    if (cat == io::events::neb) {
      (this->*(_neb_processing_table[elem]))(*data);
      ++_transaction_queries;
      retval = 0;
    }
    else if (cat == io::events::correlation) {
      (this->*(_correlation_processing_table[elem]))(*data);
      ++_transaction_queries;
      retval = 0;
    }
  }

  // Commit transaction.
  if (_queries_per_transaction > 1) {
    logging::debug(logging::low) << "SQL: current transaction has "
      << _transaction_queries << " pending queries";
    if (_db->isOpen()
        && ((_transaction_queries >= _queries_per_transaction)
            || data.isNull())) {
      logging::info(logging::medium) << "SQL: committing transaction";
      _db->commit();
      retval += _transaction_queries;
      _db->transaction();
      _transaction_queries = 0;
    }
  }

  return (retval);
}
