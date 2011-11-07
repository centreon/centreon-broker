/*
** Copyright 2009-2011 Merethis
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

#include <assert.h>
#include <QPair>
#include <QSqlError>
#include <QThread>
#include <QVariant>
#include <QVector>
#include <sstream>
#include <stdlib.h>
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "mapping.hh"
#include "com/centreon/broker/sql/internal.hh"
#include "com/centreon/broker/sql/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Processing table.
QHash<QString, void (stream::*)(io::data const&)> stream::_processing_table;

// Delayed connections deletion.
stream::qt_mysql_sucks* stream::delayed_connections = NULL;
static QMutex           stream_mutex;
static unsigned int     stream_instance = 0;

/**
 *  Remove all delayed connections.
 */
stream::qt_mysql_sucks::~qt_mysql_sucks() {
  for (QMap<QThread*, QList<QString> >::iterator
         it1 = streams.begin(),
         end1 = streams.end();
       it1 != end1;
       ++it1)
    for (QList<QString>::iterator
           it2 = it1->begin(),
           end2 = it1->end();
         it2 != end2;
         ++it2)
      QSqlDatabase::removeDatabase(*it2);
}

/**
 *  Remove delayed connections.
 *
 *  @param[in] current Current connection name.
 */
void stream::qt_mysql_sucks::remove_delayed(QString const& current) {
  QThread* thr(QThread::currentThread());
  while (!streams[thr].isEmpty()) {
    if (streams[thr].front() != current)
      QSqlDatabase::removeDatabase(streams[thr].front());
    streams[thr].pop_front();
  }
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Assignment operator.
 *
 *  Should not be used. Any call to this method will result in a call to
 *  abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
  assert(false);
  abort();
  return (*this);
}

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
       << " JOIN " << mapped_type<neb::service>::table << " ON "
       << mapped_type<neb::host>::table << ".host_id="
       << mapped_type<neb::service>::table << ".host_id SET "
       << mapped_type<neb::host>::table << ".enabled=0, "
       << mapped_type<neb::service>::table << ".enabled=0"
       << " WHERE " << mapped_type<neb::host>::table
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
       << "   JOIN " << mapped_type<neb::host>::table << " AS hosts"
       << "   ON hosts.host_id=services.host_id WHERE hosts.instance_id="
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

  return ;
}

/**
 *  Execute a plain SQL query.
 *
 *  @param[in] query Query to execute.
 */
void stream::_execute(QString const& query) {
  logging::debug << logging::LOW << "SQL: executing query: " << query;
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
  logging::debug << logging::LOW << "SQL: executing query";
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
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");

  // Execute query.
  QSqlQuery q(*_db);
  bool ret(q.prepare(query.c_str()));
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
  _prepare_insert<neb::instance>(_instance_insert);
  _prepare_insert<neb::notification>(_notification_insert);
  _prepare_insert<neb::service>(_service_insert);
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
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
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

  id.clear();
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::downtime>(_downtime_update, id);

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
bool stream::_prepare_insert(QScopedPointer<QSqlQuery>& st) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");
  logging::info << logging::LOW << "preparing statement: "
                << query.c_str();

  // Prepare statement.
  st.reset(new QSqlQuery(*_db));
  return (st->prepare(query.c_str()));
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 *  @param[in]  id List of fields that form an UNIQUE.
 */
template <typename T>
bool stream::_prepare_update(QScopedPointer<QSqlQuery>& st,
                             QVector<QPair<QString, bool> > const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    bool found(false);
    for (QVector<QPair<QString, bool> >::const_iterator
           it2 = id.begin(), end2 = id.end();
         it2 != end2;
         ++it2)
      if (it->first == it2->first.toStdString())
        found = true;
    if (!found) {
      query.append(it->first);
      query.append("=:");
      query.append(it->first);
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
      query.append(it->first.toStdString());
      query.append(", -1)=COALESCE(:");
      query.append(it->first.toStdString());
      query.append(", -1)");
    }
    else {
      query.append(it->first.toStdString());
      query.append("=:");
      query.append(it->first.toStdString());
    }
    query.append(" AND ");
  }
  query.resize(query.size() - 5);
  logging::info << logging::LOW << "SQL: preparing statement: "
                << query.c_str();

  // Prepare statement.
  st.reset(new QSqlQuery(*_db));
  return (st->prepare(query.c_str()));
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void stream::_process_acknowledgement(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing acknowledgement event";

  // Processing.
  _update_on_none_insert(*_acknowledgement_insert,
    *_acknowledgement_update,
    *static_cast<neb::acknowledgement const*>(&e));

  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e Uncasted comment.
 */
void stream::_process_comment(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing comment event";

  // Processing.
  neb::comment const& com(*static_cast<neb::comment const*>(&e));
  if (com.host_id)
    _update_on_none_insert(*_comment_insert,
      *_comment_update,
      *static_cast<neb::comment const*>(&e));
  else
    logging::error << logging::LOW << "SQL: could not process event " \
      "which does not have an host ID";

  return ;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void stream::_process_custom_variable(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing custom variable event";

  // Processing.
  _update_on_none_insert(*_custom_variable_insert,
    *_custom_variable_update,
    *static_cast<neb::custom_variable const*>(&e));

  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void stream::_process_custom_variable_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing custom variable status event";

  // Processing.
  neb::custom_variable_status const& cvs(
    *static_cast<neb::custom_variable_status const*>(&e));
  *_custom_variable_status_update << cvs;
  _execute(*_custom_variable_status_update);
  if (_custom_variable_status_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: custom variable ("
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
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing downtime event";

  // Processing.
  _update_on_none_insert(*_downtime_insert,
    *_downtime_update,
    *static_cast<neb::downtime const*>(&e));

  return ;
}

/**
 *  Process a correlation engine event.
 *
 *  @param[in] e Uncasted correlation engine event.
 */
void stream::_process_engine(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
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
  logging::info << logging::MEDIUM
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
  logging::info << logging::MEDIUM
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
  logging::info << logging::MEDIUM << "SQL: processing host event (id: "
    << h.host_id << ", name: " << h.host_name << ", instance: "
    << h.instance_id << ")";

  // Processing
  if (h.host_id)
    _update_on_none_insert(*_host_insert, *_host_update, h);
  else
    logging::error << logging::HIGH << "SQL: host '"
      << h.host_name << "' has no ID";

  return ;
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 */
void stream::_process_host_check(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host check event";

  // Processing.
  neb::host_check const& hc(
    *static_cast<neb::host_check const*>(&e));
  *_host_check_update << hc;
  _execute(*_host_check_update);
  if (_host_check_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: host check could not " \
         "be updated because host " << hc.host_id
      << " was not found in database";

  return ;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 */
void stream::_process_host_dependency(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host dependency event";

  // Processing (errors are silently ignored).
  _insert(*static_cast<neb::host_dependency const*>(&e));

  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void stream::_process_host_group(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host group event";

  // Processing (errors are silently ignored).
  _insert(*static_cast<neb::host_group const*>(&e));

  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void stream::_process_host_group_member(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host group member event";

  // Fetch proper structure.
  neb::host_group_member const& hgm(
    *static_cast<neb::host_group_member const*>(&e));

  // Fetch host group ID.
  std::ostringstream ss;
  ss << "SELECT hostgroup_id FROM "
     << mapped_type<neb::host_group>::table
     << " WHERE instance_id=" << hgm.instance_id
     << " AND name=\"" << hgm.group.toStdString() << "\"";
  QSqlQuery q(*_db);
  logging::info << logging::LOW << "SQL: host group member: "
                << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch hostgroup ID.
    int hostgroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
      << "SQL: fetch hostgroup of id " << hostgroup_id;

    // Insert hostgroup membership.
    std::ostringstream oss;
    oss << "INSERT INTO "
        << mapped_type<neb::host_group_member>::table
        << " (host_id, hostgroup_id) VALUES("
        << hgm.host_id << ", "
        << hostgroup_id << ")";
    logging::info << logging::LOW << "SQL: executing query: "
      << oss.str().c_str();
    _db->exec(oss.str().c_str());
  }
  else
    logging::info << logging::HIGH
      << "SQL: discarding membership between host " << hgm.host_id
      << " and hostgroup (" << hgm.instance_id << ", " << hgm.group
      << ")";

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void stream::_process_host_parent(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
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
  logging::info << logging::MEDIUM
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
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host status event";

  // Processing.
  neb::host_status const& hs(
    *static_cast<neb::host_status const*>(&e));
  *_host_status_update << hs;
  _execute(*_host_status_update);
  if (_host_status_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: host could not be " \
         "updated because host " << hs.host_id
      << " was not found in database";

  return ;
}

/**
 *  Process an instance event.
 *
 *  @param[in] e Uncasted instance.
 */
void stream::_process_instance(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing instance event";

  // Clean tables.
  neb::instance const& i(*static_cast<neb::instance const*>(&e));
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
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing instance status event";

  // Processing.
  neb::instance_status const& is(
    *static_cast<neb::instance_status const*>(&e));
  *_instance_status_update << is;
  _execute(*_instance_status_update);
  if (_instance_status_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: instance "
      << is.id << " was not updated because no matching entry " \
         "was found in database";

  return ;
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void stream::_process_issue(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing issue event";

  // Processing.
  _update_on_none_insert(*_issue_insert,
    *_issue_update,
    *static_cast<correlation::issue const*>(&e));

  return ;
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void stream::_process_issue_parent(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing issue parent event";

  // Fetch proper structure.
  correlation::issue_parent const& ip(
    *static_cast<correlation::issue_parent const*>(&e));
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
    logging::info << logging::LOW << "SQL: issue parent: "
      << query.str().c_str();
    QSqlQuery q(*_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      child_id = q.value(0).toInt();
      logging::debug << logging::LOW << "SQL: child issue ID: "
        << child_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch child issue " \
               << "ID (host=" << ip.child_host_id << ", service="
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
    logging::info << logging::LOW << "SQL: issue child: "
      << query.str().c_str();
    QSqlQuery q(*_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      parent_id = q.value(0).toInt();
      logging::debug << logging::LOW << "SQL: parent issue ID: "
        << parent_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch parent issue " \
               << "ID (host=" << ip.parent_host_id << ", service="
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
      throw (exceptions::msg() << "SQL: issue parent insert query " \
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
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing log event";

  // Fetch proper structure.
  neb::log_entry const& le(
    *static_cast<neb::log_entry const*>(&e));

  // Fetch issue ID (if any).
  int issue;
  if (le.issue_start_time) {
    std::ostringstream ss;
    ss << "SELECT issue_id FROM "
       << mapped_type<correlation::issue>::table
       << " WHERE host_id=" << le.host_id
       << " AND service_id=" << le.service_id
       << " AND start_time=" << le.issue_start_time;
    logging::info << logging::LOW << "SQL: executing query: "
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
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<neb::log_entry>::table);
  query.append("(");
  for (std::list<std::pair<std::string, getter_setter<neb::log_entry> > >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.append(field);
  query.append(") VALUES(");
  for (std::list<std::pair<std::string, getter_setter<neb::log_entry> > >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.append(":");
  query.append(field);
  query.append(")");

  // Execute query.
  logging::info << logging::LOW
    << "SQL: executing query: " << query.c_str();
  QSqlQuery q(*_db);
  q.prepare(query.c_str());
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
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing module event";

  // Processing.
  _insert(*static_cast<neb::module const*>(&e));

  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
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
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing service event";

  // Processing.
  neb::service const& s(*static_cast<neb::service const*>(&e));
  if (s.host_id && s.service_id) {
    _update_on_none_insert(*_service_insert,
      *_service_update,
      s);
  }
  else
    logging::error << logging::HIGH << "SQL: service '"
      << s.service_description << "' has no host ID or no service ID";

  return ;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void stream::_process_service_check(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service check event";

  // Processing.
  neb::service_check const& sc(
    *static_cast<neb::service_check const*>(&e));
  *_service_check_update << sc;
  _execute(*_service_check_update);
  if (_service_check_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: service check could " \
         "not be updated because service (" << sc.host_id << ", "
      << sc.service_id << ") was not found in database";

  return ;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 */
void stream::_process_service_dependency(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service dependency event";

  // Processing (errors are silently ignored).
  _insert(*static_cast<neb::service_dependency const*>(&e));

  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void stream::_process_service_group(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service group event";

  // Processing (errors are silently ignored).
  _insert(*static_cast<neb::service_group const*>(&e));

  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void stream::_process_service_group_member(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service group member event";

  // Fetch proper structure.
  neb::service_group_member const& sgm(
    *static_cast<neb::service_group_member const*>(&e));

  // Fetch service group ID.
  std::ostringstream ss;
  ss << "SELECT servicegroup_id FROM "
     << mapped_type<neb::service_group>::table
     << " WHERE instance_id=" << sgm.instance_id
     << " AND name=\"" << sgm.group.toStdString() << "\"";
  QSqlQuery q(*_db);
  logging::info << logging::LOW << "SQL: executing query: "
    << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch servicegroup ID.
    int servicegroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
      << "SQL: fetch servicegroup of id " << servicegroup_id;

    // Insert servicegroup membership.
    std::ostringstream oss;
    oss << "INSERT INTO "
        << mapped_type<neb::service_group_member>::table
        << " (host_id, service_id, servicegroup_id) VALUES("
        << sgm.host_id << ", "
        << sgm.service_id << ", "
        << servicegroup_id << ")";
    logging::info << logging::LOW << "SQL: executing query: "
      << oss.str().c_str();
    _db->exec(oss.str().c_str());
  }
  else
    logging::info << logging::HIGH
      << "SQL: discarding membership between service ("
      << sgm.host_id << ", " << sgm.service_id << ") and servicegroup ("
      << sgm.instance_id << ", " << sgm.group << ")";

  return ;
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
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
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service status event";

  // Processing.
  neb::service_status const& ss(
    *static_cast<neb::service_status const*>(&e));
  *_service_status_update << ss;
  _execute(*_service_status_update);
  if (_service_status_update->numRowsAffected() != 1)
    logging::error << logging::MEDIUM << "SQL: service could not be " \
         "updated because service (" << ss.host_id << ", "
      << ss.service_id << ") was not found in database";

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
 *  @param[in] type     Database type.
 *  @param[in] host     Database host.
 *  @param[in] port     Database port.
 *  @param[in] user     User.
 *  @param[in] password Password.
 *  @param[in] db       Database name.
 *  @param[in] wse      With state events.
 */
stream::stream(QString const& type,
               QString const& host,
               unsigned short port,
               QString const& user,
               QString const& password,
               QString const& db,
               bool wse)
  : _process_out(true),
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
      QMutexLocker lock(&stream_mutex);
      if (stream_instance++ == 0)
        delayed_connections = new qt_mysql_sucks;
      if (!_db->open())
        throw (exceptions::msg() << "SQL: could not open SQL database");
    }

    // Prepare queries.
    _prepare();

    // Remove previous connections.
    QMutexLocker lock(&stream_mutex);
    delayed_connections->remove_delayed(id);
  }
  catch (...) {
    QMutexLocker lock(&stream_mutex);

    // Remove previous connections.
    delayed_connections->remove_delayed(id);

    // Unprepare queries.
    _unprepare();

    // Close database if open.
    if (_db->isOpen()) {
      _db->close();
      _db.reset();
    }

    // Add this connection to the connections to be deleted.
    delayed_connections->streams[QThread::currentThread()].push_back(id);
    lock.unlock();

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

  // Process state events.
  _with_state_events = s._with_state_events;

  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Clone database.
  _db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*s._db, id)));

  try {
    {
      QMutexLocker lock(&stream_mutex);
      ++stream_instance;
      // Open database.
      if (!_db->open())
        throw (exceptions::msg() << "SQL: could not open SQL database");
    }

    // Prepare queries.
    _prepare();

    QMutexLocker lock(&stream_mutex);
    // Remove previous connections.
    delayed_connections->remove_delayed(id);
  }
  catch (...) {
    QMutexLocker lock(&stream_mutex);

    // Remove previous connections.
    delayed_connections->remove_delayed(id);

    // Unprepare queries.
    _unprepare();

    // Close database if open.
    if (_db->isOpen()) {
      _db->close();
      _db.reset();
    }

    // Add this connection to the connections to be deleted.
    delayed_connections->streams[QThread::currentThread()].push_back(id);
    lock.unlock();

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

  // Close database.
  QMutexLocker lock(&stream_mutex);
  _db->close();
  _db.reset();

  // Add this connection to the connections to be deleted.
  delayed_connections->streams[QThread::currentThread()].push_back(id);
  if (--stream_instance == 0) {
    delete delayed_connections;
    delayed_connections = NULL;
  }
}

/**
 *  Initialize SQL layer.
 */
void stream::initialize() {
  // Fill processing table.
  _processing_table["com::centreon::broker::neb::acknowledgement"]
    = &stream::_process_acknowledgement;
  _processing_table["com::centreon::broker::neb::comment"]
    = &stream::_process_comment;
  _processing_table["com::centreon::broker::neb::custom_variable"]
    = &stream::_process_custom_variable;
  _processing_table["com::centreon::broker::neb::custom_variable_status"]
    = &stream::_process_custom_variable_status;
  _processing_table["com::centreon::broker::neb::downtime"]
    = &stream::_process_downtime;
  _processing_table["com::centreon::broker::neb::event_handler"]
    = &stream::_process_event_handler;
  _processing_table["com::centreon::broker::neb::flapping_status"]
    = &stream::_process_flapping_status;
  _processing_table["com::centreon::broker::neb::host"]
    = &stream::_process_host;
  _processing_table["com::centreon::broker::neb::host_check"]
    = &stream::_process_host_check;
  _processing_table["com::centreon::broker::neb::host_dependency"]
    = &stream::_process_host_dependency;
  _processing_table["com::centreon::broker::neb::host_group"]
    = &stream::_process_host_group;
  _processing_table["com::centreon::broker::neb::host_group_member"]
    = &stream::_process_host_group_member;
  _processing_table["com::centreon::broker::neb::host_parent"]
    = &stream::_process_host_parent;
  _processing_table["com::centreon::broker::neb::host_status"]
    = &stream::_process_host_status;
  _processing_table["com::centreon::broker::neb::instance"]
    = &stream::_process_instance;
  _processing_table["com::centreon::broker::neb::instance_status"]
    = &stream::_process_instance_status;
  _processing_table["com::centreon::broker::neb::log_entry"]
    = &stream::_process_log;
  _processing_table["com::centreon::broker::neb::module"]
    = &stream::_process_module;
  _processing_table["com::centreon::broker::neb::notification"]
    = &stream::_process_notification;
  _processing_table["com::centreon::broker::neb::service"]
    = &stream::_process_service;
  _processing_table["com::centreon::broker::neb::service_check"]
    = &stream::_process_service_check;
  _processing_table["com::centreon::broker::neb::service_dependency"]
    = &stream::_process_service_dependency;
  _processing_table["com::centreon::broker::neb::service_group"]
    = &stream::_process_service_group;
  _processing_table["com::centreon::broker::neb::service_group_member"]
    = &stream::_process_service_group_member;
  _processing_table["com::centreon::broker::neb::service_status"]
    = &stream::_process_service_status;
  _processing_table["com::centreon::broker::correlation::engine_state"]
    = &stream::_process_engine;
  _processing_table["com::centreon::broker::correlation::host_state"]
    = &stream::_process_host_state;
  _processing_table["com::centreon::broker::correlation::issue"]
    = &stream::_process_issue;
  _processing_table["com::centreon::broker::correlation::issue_parent"]
    = &stream::_process_issue_parent;
  _processing_table["com::centreon::broker::correlation::service_state"]
    = &stream::_process_service_state;
  _processing_table.squeeze();
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
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> stream::read() {
  throw (exceptions::msg()
           << "SQL: attempt to read from a SQL stream (software bug)");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 */
void stream::write(QSharedPointer<io::data> data) {
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
             << "SQL stream is shutdown");
  QHash<QString, void (stream::*)(io::data const&)>::const_iterator it;
  it = _processing_table.find(data->type());
  if (it != _processing_table.end())
    (this->*(it.value()))(*data);
  return ;
}
