/*
** Copyright 2009-2015 Centreon
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
#include <QSqlDriver>
#include <QSqlField>
#include <QTextStream>
#include <QThread>
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
void (stream::* const stream::_correlation_processing_table[])(misc::shared_ptr<io::data> const&) = {
  NULL,
  &stream::_process_engine,
  &stream::_process_host_state,
  &stream::_process_issue,
  &stream::_process_issue_parent,
  &stream::_process_service_state
};
void (stream::* const stream::_neb_processing_table[])(misc::shared_ptr<io::data> const&) = {
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
  &stream::_null_process,
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
  &stream::_null_process,
  &stream::_process_service,
  &stream::_process_service_status,
  &stream::_null_process,
  &stream::_process_host_group_member,
  &stream::_process_service_group_member
};

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Clean the deleted cache of instance ids.
 */
void stream::_cache_clean() {
  _cache_deleted_instance_id.clear();
}

/**
 *  Create the deleted cache of instance ids.
 */
void stream::_cache_create() {
  std::ostringstream ss;
  ss << "SELECT instance_id"
     << " FROM " << mapped_type<neb::instance>::table
     << " WHERE deleted=1";
  try {
    database_query q(_db);
    q.run_query(ss.str());
    while (q.next())
      _cache_deleted_instance_id.insert(q.value(0).toUInt());
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "SQL: could not get list of deleted instances: "
      << e.what();
  }
  return ;
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
  // Query object.
  database_query q(_db);

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
    q.run_query(
        ss.str(),
        "SQL: could not clean hosts and services tables");
  }

  // Remove host group memberships.
  {
    std::ostringstream ss;
    ss << "DELETE hgm"
       << " FROM " << mapped_type<neb::host_group_member>::table << " AS hgm"
       << " LEFT JOIN " << mapped_type<neb::host>::table << " AS h"
       << " ON hgm.host_id=h.host_id"
       << " WHERE h.instance_id=" << instance_id;
    q.run_query(
        ss.str(),
        "SQL: could not clean host groups memberships tables");
  }

  // Remove service group memberships
  {
    std::ostringstream ss;
    ss << "DELETE sgm"
       << " FROM " << mapped_type<neb::service_group_member>::table << " AS sgm"
       << " LEFT JOIN " << mapped_type<neb::host>::table << " AS h"
       << " ON sgm.host_id=h.host_id"
       << " WHERE h.instance_id=" << instance_id;
    q.run_query(
        ss.str(),
        "SQL: could not clean service groups memberships tables");
  }

  // Remove host groups.
  _empty_host_group_delete.run_statement("SQL");

  // Remove service groups.
  _empty_service_group_delete.run_statement("SQL");

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
    q.run_query(
        ss.str(),
        "SQL: could not clean host dependencies table");
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
    q.run_query(
        ss.str(),
        "SQL: could not clean host parents table");
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
    q.run_query(
        ss.str(),
        "SQL: could not clean service dependencies tables");
  }

  // Remove list of modules.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::module>::table
       << " WHERE instance_id=" << instance_id;
    q.run_query(ss.str(), "SQL: could not clean modules table");
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
    q.run_query(
        ss.str(),
        "SQL: could not clean custom variables table");
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
    q.run_query(ss.str(), "SQL: could not clean comments table");
  }

  // Remove downtimes.
  {
    std::ostringstream ss;
    ss << "UPDATE " << mapped_type<neb::downtime>::table << " AS d"
          "  SET d.deletion_time=" << time(NULL)
       << "  WHERE d.instance_id=" << instance_id
       << "  AND (d.deletion_time IS NULL OR d.deletion_time=0)";
    q.run_query(ss.str(), "SQL: could not clean downtimes table");
  }

  return ;
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
  _prepare_insert<neb::host_group_member>(_host_group_member_insert);
  _prepare_insert<neb::host_parent>(_host_parent_insert);
  _prepare_insert<neb::instance>(_instance_insert);
  _prepare_insert<neb::module>(_module_insert);
  _prepare_insert<neb::notification>(_notification_insert);
  _prepare_insert<neb::service>(_service_insert);
  _prepare_insert<neb::service_dependency>(_service_dependency_insert);
  _prepare_insert<neb::service_group>(_service_group_insert);
  _prepare_insert<neb::service_group_member>(_service_group_member_insert);
  _prepare_insert<correlation::host_state>(_host_state_insert);
  _prepare_insert<correlation::issue>(_issue_insert);
  _prepare_insert<correlation::service_state>(_service_state_insert);
  {
    std::string query(
      "INSERT INTO issues_issues_parents (child_id, end_time, start_time, parent_id)"
      " VALUES (:child_id, :end_time, :start_time, :parent_id)");
    _issue_parent_insert.prepare(query, "SQL: could not prepare query");
  }
  {
    std::string query(
      "SELECT issue_id FROM issues"
      " WHERE host_id=:host_id"
      " AND service_id=:service_id"
      " AND start_time=:start_time");
    _issue_select.prepare(query, "SQL: could not prepare query");
  }

  // Prepare update queries.
  std::map<std::string, bool> id;

  id.clear();
  id["entry_time"] = false;
  id["host_id"] = false;
  id["service_id"] = true;
  _prepare_update<neb::acknowledgement>(_acknowledgement_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = true;
  id["entry_time"] = false;
  _prepare_update<neb::comment>(_comment_update, id);

  id.clear();
  id["host_id"] = false;
  id["name"] = false;
  id["service_id"] = true;
  _prepare_update<neb::custom_variable>(_custom_variable_update, id);
  _prepare_delete<neb::custom_variable>(_custom_variable_delete, id);

  id.clear();
  id["host_id"] = false;
  id["name"] = false;
  id["service_id"] = true;
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
    std::string query(oss.str());
    _downtime_update.prepare(query, "SQL: could not prepare query");
  }

  id.clear();
  id["host_id"] = false;
  id["service_id"] = true;
  id["start_time"] = false;
  _prepare_update<neb::event_handler>(_event_handler_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = true;
  id["event_time"] = false;
  _prepare_update<neb::flapping_status>(_flapping_status_update, id);

  id.clear();
  id["host_id"] = false;
  _prepare_update<neb::host>(_host_update, id);

  id.clear();
  id["host_id"] = false;
  _prepare_update<neb::host_check>(_host_check_update, id);

  id.clear();
  id["host_id"] = false;
  id["dependent_host_id"] = false;
  _prepare_update<neb::host_dependency>(_host_dependency_update, id);

  id.clear();
  id["hostgroup_id"] = false;
  _prepare_update<neb::host_group>(_host_group_update, id);

  id.clear();
  id["hostgroup_id"] = false;
  id["host_id"] = false;
  _prepare_delete<neb::host_group_member>(_host_group_member_delete, id);

  id.clear();
  id["host_id"] = false;
  _prepare_update<neb::host_status>(_host_status_update, id);

  id.clear();
  id["instance_id"] = false;
  _prepare_update<neb::instance>(_instance_update, id);

  id.clear();
  id["instance_id"] = false;
  _prepare_update<neb::instance_status>(_instance_status_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = true;
  id["start_time"] = false;
  _prepare_update<neb::notification>(_notification_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = false;
  _prepare_update<neb::service>(_service_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = false;
  _prepare_update<neb::service_check>(_service_check_update, id);

  id.clear();
  id["dependent_host_id"] = false;
  id["dependent_service_id"] = false;
  id["host_id"] = false;
  id["service_id"] = false;
  _prepare_update<neb::service_dependency>(_service_dependency_update, id);

  id.clear();
  id["servicegroup_id"] = false;
  _prepare_update<neb::service_group>(_service_group_update, id);

  id.clear();
  id["servicegroup_id"] = false;
  id["host_id"] = false;
  id["service_id"] = false;
  _prepare_delete<neb::service_group_member>(_service_group_member_delete, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = false;
  _prepare_update<neb::service_status>(_service_status_update, id);

  id.clear();
  id["host_id"] = false;
  id["start_time"] = false;
  _prepare_update<correlation::host_state>(_host_state_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = true;
  id["start_time"] = false;
  _prepare_update<correlation::issue>(_issue_update, id);

  id.clear();
  id["host_id"] = false;
  id["service_id"] = false;
  id["start_time"] = false;
  _prepare_update<correlation::service_state>(_service_state_update, id);

  {
    std::string query(
      "UPDATE issues_issues_parents SET end_time=:end_time"
      " WHERE child_id=:child_id"
      "       AND start_time=:start_time"
      "       AND parent_id=:parent_id");
    _issue_parent_update.prepare(
      query,
      "SQL: could not prepare issues links update query");
  }

  // Prepare select queries.
  _prepare_select<neb::host_parent>(_host_parent_select);

  // Prepare cleanup queries.
  {
    std::string query(
      "DELETE FROM hostgroups"
      "  WHERE hostgroup_id"
      "    NOT IN (SELECT DISTINCT hostgroup_id FROM hosts_hostgroups)");
    _empty_host_group_delete.prepare(
      query,
      "SQL: could not prepare host group cleanup query");
  }
  {
    std::string query(
      "DELETE FROM servicegroups"
      "  WHERE servicegroup_id"
      "    NOT IN (SELECT DISTINCT servicegroup_id FROM services_servicegroups)");
    _empty_service_group_delete.prepare(
      query,
      "SQL: could not prepare service group cleanup query");
  }

  return ;
}

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st  Query object.
 */
template <typename T>
void stream::_prepare_insert(database_query& st) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->name.toStdString());
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->field.toStdString());
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare insertion query");

  return ;
}
/**
 *  Prepare a select statement for later execution.
 *
 *  @param[out] st  Query object.
 */
template <typename T>
void stream::_prepare_select(database_query& st) {
  // Build query string.
  std::string query;
  query = "SELECT * FROM ";
  query.append(mapped_type<T>::table);
  query.append(" WHERE ");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->name.toStdString());
    query.append(" = ");
    query.append(it->field.toStdString());
    query.append(" AND ");
  }

  query.resize(query.size() - 5);

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare select query");
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st  Query object.
 *  @param[in]  id  List of fields that form an UNIQUE.
 */
template <typename T>
void stream::_prepare_update(
               database_query& st,
               std::map<std::string, bool> const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it(db_mapped_type<T>::list.begin()),
         end(db_mapped_type<T>::list.end());
       it != end;
       ++it) {
    bool found(id.find(it->name.toStdString()) != id.end());
    if (!found) {
      query.append(it->name.toStdString());
      query.append("=");
      query.append(it->field.toStdString());
      query.append(", ");
    }
  }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (std::map<std::string, bool>::const_iterator
         it(id.begin()),
         end(id.end());
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

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare update query");

  return ;
}

/**
 *  Prepare a deletion query.
 *
 *  @param[out] st  Query object.
 *  @param[in]  id  List of fields that form an UNIQUE.
 */
template <typename T>
void stream::_prepare_delete(
               database_query& st,
               std::map<std::string, bool> const& id) {
  // Build query string.
  std::string query;
  query = "DELETE FROM ";
  query.append(mapped_type<T>::table);
  query.append(" WHERE ");
  for (std::map<std::string, bool>::const_iterator
         it(id.begin()),
         end(id.end());
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

  // Prepare statement.
  st.prepare(query, "SQL: could not prepare deletion query");

  return ;
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void stream::_process_acknowledgement(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::acknowledgement const&
    ack(*static_cast<neb::acknowledgement const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing acknowledgement event (instance: "
    << ack.instance_id << ", host: " << ack.host_id << ", service: "
    << ack.service_id << ", entry time: " << ack.entry_time
    << ", deletion time: " << ack.deletion_time << ")";

  // Processing.
  _update_on_none_insert(
    _acknowledgement_insert,
    _acknowledgement_update,
    ack);

  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e Uncasted comment.
 */
void stream::_process_comment(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::comment const& com(*static_cast<neb::comment const*>(e.data()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing comment event"
       " (instance: " << com.instance_id << ", host: " << com.host_id
    << ", service: " << com.service_id << ", entry time: "
    << com.entry_time << ", expire time: " << com.expire_time
    << ", deletion time: " << com.deletion_time << ", id: "
    << com.internal_id << ")";

  // Processing.
  if (com.host_id)
    _update_on_none_insert(
      _comment_insert,
      _comment_update,
      com);
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
void stream::_process_custom_variable(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::custom_variable const&
    cv(*static_cast<neb::custom_variable const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing custom variable event (host: " << cv.host_id
    << ", service: " << cv.service_id << ", name: " << cv.name << ")";

  // Processing.
  if (cv.enabled) {
    _update_on_none_insert(
      _custom_variable_insert,
      _custom_variable_update,
      cv);
  }
  else {
    _custom_variable_delete.bind_value(":host_id", cv.host_id);
    _custom_variable_delete.bind_value(
      ":service_id",
      (cv.service_id ? QVariant(cv.service_id) : QVariant(QVariant::Int)));
    _custom_variable_delete.bind_value(":name", cv.name);
    try { _custom_variable_delete.run_statement(); }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "SQL: could not remove custom variable (host: "
             << cv.host_id << ", service: " << cv.service_id
             << ", name '" << cv.name << "'): " << e.what());
    }
  }
  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void stream::_process_custom_variable_status(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::custom_variable_status const&
    cvs(*static_cast<neb::custom_variable_status const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing custom variable status event (host: "
    << cvs.host_id << ", service: " << cvs.service_id << ", name: "
    << cvs.name << ", update time: " << cvs.update_time << ")";

  // Processing.
  _custom_variable_status_update << cvs;
  _custom_variable_status_update.run_statement("SQL");
  if (_custom_variable_status_update.num_rows_affected() != 1)
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
void stream::_process_downtime(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::downtime const&
    d(*static_cast<neb::downtime const*>(e.data()));

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
    _downtime_update << d;
    _downtime_update.run_statement("SQL");
  }
  // Update or insert if no entry was found, as long as the downtime
  // is valid.
  else
    _update_on_none_insert(
      _downtime_insert,
      _downtime_update,
      d);

  return ;
}

/**
 *  Process a correlation engine event.
 *
 *  @param[in] e Uncasted correlation engine event.
 */
void stream::_process_engine(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing correlation engine event";

  // Cast event.
  correlation::engine_state const&
    es(*static_cast<correlation::engine_state const*>(e.data()));

  // Close issues.
  if (es.started) {
    time_t now(time(NULL));
    {
      std::ostringstream ss;
      ss << "UPDATE issues SET end_time=" << now
         << " WHERE end_time=0 OR end_time IS NULL";
      database_query q(_db);
      q.run_query(ss.str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE issues_issues_parents SET end_time=" << now
         << " WHERE end_time=0 OR end_time IS NULL";
      database_query q(_db);
      q.run_query(ss.str());
    }
  }

  return ;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void stream::_process_event_handler(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing event handler event";

  // Processing.
  _update_on_none_insert(
    _event_handler_insert,
    _event_handler_update,
    *static_cast<neb::event_handler const*>(e.data()));

  return ;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void stream::_process_flapping_status(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing flapping status event";

  // Processing.
  _update_on_none_insert(
    _flapping_status_insert,
    _flapping_status_update,
    *static_cast<neb::flapping_status const*>(e.data()));

  return ;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void stream::_process_host(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host const& h(*static_cast<neb::host const*>(e.data()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing host event"
       " (instance: " << h.instance_id << ", id: "
    << h.host_id << ", name: " << h.host_name << ")";

  // Processing
  if (h.host_id)
    _update_on_none_insert(_host_insert, _host_update, h);
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
void stream::_process_host_check(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_check const&
    hc(*static_cast<neb::host_check const*>(e.data()));

  time_t now(time(NULL));
  if (hc.check_type                // - passive result
      || !hc.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
                                   // - normal case
      || (hc.next_check >= now - 5 * 60)
      || !hc.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing host check event (host: " << hc.host_id
      << ", command: " << hc.command_line
      << ", from instance: " << hc.instance_id << ")";

    // Processing.
    _host_check_update << hc;
    _host_check_update.run_statement("SQL");
    if (_host_check_update.num_rows_affected() != 1)
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
void stream::_process_host_dependency(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_dependency const&
    hd(*static_cast<neb::host_dependency const*>(e.data()));

  // Insert/Update.
  if (hd.enabled) {
    logging::info(logging::medium)
      << "SQL: enabling host dependency of " << hd.dependent_host_id
      << " on " << hd.host_id;
    _update_on_none_insert(
      _host_dependency_insert,
      _host_dependency_update,
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
    database_query q(_db);
    q.run_query(oss.str(), "SQL");
  }

  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void stream::_process_host_group(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_group const&
    hg(*static_cast<neb::host_group const*>(e.data()));

  if (hg.enabled) {
    logging::info(logging::medium) << "SQL: enabling host group '"
      << hg.name << "' of instance " << hg.instance_id;
    _update_on_none_insert(
      _host_group_insert,
      _host_group_update,
      hg);
  }
  else {
    logging::info(logging::medium) << "SQL: disabling host group '"
      << hg.name << "' of instance " << hg.instance_id;

    // Delete group members.
    {
      std::ostringstream oss;
      oss << "DELETE hgm"
          << "  FROM " << mapped_type<neb::host_group_member>::table << " AS hgm"
          << "  LEFT JOIN " << mapped_type<neb::host>::table << " AS h"
          << "    ON hgm.host_id=h.host_id"
          << "  WHERE hgm.hostgroup_id=" << hg.id
          << "    AND h.instance_id=" << hg.instance_id;
      database_query q(_db);
      q.run_query(oss.str(), "SQL");
    }

    // Delete empty group.
    _empty_host_group_delete.run_statement("SQL");
  }

  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void stream::_process_host_group_member(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_group_member const&
    hgm(*static_cast<neb::host_group_member const*>(e.data()));

  // Insert.
  if (hgm.enabled) {
    // Log message.
    logging::info(logging::medium)
      << "SQL: enabling host group member (group: "
      << hgm.group_id << ", instance: " << hgm.instance_id
      << ", host: " << hgm.host_id << ")";

    // We only need to try to insert in this table as the
    // host_id/hostgroup_id should be UNIQUE.
    try {
      _host_group_member_insert << hgm;
      _host_group_member_insert.run_statement("SQL");
    }
    catch (std::exception const& e) {
      logging::info(logging::high)
        << "SQL: discarding membership between host " << hgm.host_id
        << " and host group " << hgm.group_id << " on instance "
        << hgm.instance_id << ": " << e.what();
    }
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
      << "SQL: removing host group member (group: "
      << hgm.group_id << ", instance: " << hgm.instance_id
      << ", host: " << hgm.host_id << ")";

    try {
      _host_group_member_delete << hgm;
      _host_group_member_delete.run_statement();
    }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "SQL: cannot delete membership of host " << hgm.host_id
             << " to host group " << hgm.group_id << " on instance "
             << hgm.instance_id << ": " << e.what());
    }
  }

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void stream::_process_host_parent(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  neb::host_parent const&
    hp(*static_cast<neb::host_parent const*>(e.data()));
  logging::info(logging::medium)
    << "SQL: processing host parent (host: " << hp.host_id << ", parent: "
    << hp.parent_id << ")";

  // Insert.
  try {
    _host_parent_select << hp;
    _host_parent_select.run_statement();
    if (_host_parent_select.size() == 1)
      return ;

    _host_parent_insert << hp;
    _host_parent_insert.run_statement();
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "SQL: could not process host parent declaration: "
      << e.what();
  }

  return ;
}

/**
 *  Process a host state event.
 *
 *  @param[in] e Uncasted host state.
 */
void stream::_process_host_state(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing host state event";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(
      _host_state_insert,
      _host_state_update,
      *static_cast<correlation::host_state const*>(e.data()));
  }

  return ;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void stream::_process_host_status(
               misc::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::host_status const&
    hs(*static_cast<neb::host_status const*>(e.data()));

  time_t now(time(NULL));
  if (hs.check_type                // - passive result
      || !hs.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
                                   // - normal case
      || (hs.next_check >= now - 5 * 60)
      || !hs.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing host status event (id: " << hs.host_id
      << ", last check: " << hs.last_check << ", state ("
      << hs.current_state << ", " << hs.state_type << "))";

    // Processing.
    _host_status_update << hs;
    _host_status_update.run_statement("SQL");
    if (_host_status_update.num_rows_affected() != 1)
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
void stream::_process_instance(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::instance const& i(*static_cast<neb::instance const*>(e.data()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing instance event"
    << "(id: " << i.id << ", name: " << i.name << ", running: "
    << (i.is_running ? "yes" : "no") << ")";

  // Clean tables.
  _clean_tables(i.id);

  // Processing.
  _update_on_none_insert(_instance_insert, _instance_update, i);

  return ;
}

/**
 *  Process an instance status event.
 *
 *  @param[in] e Uncasted instance status.
 */
void stream::_process_instance_status(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::instance_status const&
    is(*static_cast<neb::instance_status const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing instance status event (id: " << is.id
    << ", last alive: " << is.last_alive << ")";

  // Processing.
  _instance_status_update << is;
  _instance_status_update.run_statement("SQL");
  if (_instance_status_update.num_rows_affected() != 1)
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
void stream::_process_issue(
               misc::shared_ptr<io::data> const& e) {
  // Issue object.
  correlation::issue const&
    i(*static_cast<correlation::issue const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing issue event (node: (" << i.host_id << ", "
    << i.service_id << "), start time: " << i.start_time
    << ", end_time: " << i.end_time << ", ack time: " << i.ack_time
    << ")";

  // Processing.
  _update_on_none_insert(_issue_insert, _issue_update, i);

  return ;
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void stream::_process_issue_parent(
               misc::shared_ptr<io::data> const& e) {
  // Issue parent object.
  correlation::issue_parent const&
    ip(*static_cast<correlation::issue_parent const*>(e.data()));

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
    database_query q(_db);
    try {
      q.run_query(query.str());
      if (!q.next())
        throw (exceptions::msg() << "child issue does not exist");
    }
    catch (std::exception const& e) {
      throw (exceptions::msg() << "SQL: could not fetch child issue "
                "ID (host: " << ip.child_host_id << ", service: "
             << ip.child_service_id << ", start: "
             << ip.child_start_time << "): " << e.what());
    }
    child_id = q.value(0).toInt();
    logging::debug(logging::low)
      << "SQL: child issue ID of (" << ip.child_host_id << ", "
      << ip.child_service_id << ", " << ip.child_start_time << ") is "
      << child_id;
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
    database_query q(_db);
    try {
      q.run_query(query.str());
      if (!q.next())
        throw (exceptions::msg() << "parent issue does not exist");
    }
    catch (std::exception const& e) {
      throw (exceptions::msg() << "SQL: could not fetch parent issue "
                "ID (host: " << ip.parent_host_id << ", service: "
             << ip.parent_service_id << ", start: "
             << ip.parent_start_time << "): " << e.what());
    }
    parent_id = q.value(0).toInt();
    logging::debug(logging::low)
      << "SQL: parent issue ID of (" << ip.parent_host_id << ", "
      << ip.parent_service_id << ", " << ip.parent_start_time << ") is "
      << parent_id;
  }

  // End of parenting.
  if (ip.end_time)
    _issue_parent_update.bind_value(
      ":end_time",
      static_cast<unsigned int>(ip.end_time));
  else
    _issue_parent_update.bind_value(
      ":end_time",
      QVariant(QVariant::Int));
  _issue_parent_update.bind_value(":child_id", child_id);
  _issue_parent_update.bind_value(
    ":start_time",
    static_cast<unsigned int>(ip.start_time));
  _issue_parent_update.bind_value(":parent_id", parent_id);
  logging::debug(logging::low)
    << "SQL: updating issue parenting between child " << child_id
    << " and parent " << parent_id << " (start: " << ip.start_time
    << ", end: " << ip.end_time << ")";
  _issue_parent_update.run_statement(
                         "SQL: issue parent update query failed");
  if (_issue_parent_update.num_rows_affected() <= 0) {
    if (ip.end_time)
      _issue_parent_insert.bind_value(
        ":end_time",
        static_cast<unsigned int>(ip.end_time));
    else
      _issue_parent_insert.bind_value(
        ":end_time",
        QVariant(QVariant::Int));
    _issue_parent_insert.bind_value(":child_id", child_id);
    _issue_parent_insert.bind_value(
      ":start_time",
      static_cast<unsigned int>(ip.start_time));
    _issue_parent_insert.bind_value(":parent_id", parent_id);
    logging::debug(logging::low)
      << "SQL: inserting issue parenting between child " << child_id
      << " and parent " << parent_id << " (start: " << ip.start_time
      << ", end: " << ip.end_time << ")";
    _issue_parent_insert.run_statement(
                           "SQL: issue parent insert query failed");
  }

  return ;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void stream::_process_log(
               misc::shared_ptr<io::data> const& e) {
  // Fetch proper structure.
  neb::log_entry const& le(
    *static_cast<neb::log_entry const*>(e.data()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing log event (ctime: "
    << le.c_time << ")";

  // Enqueue log and eventually process it.
  _log_queue.push_back(e);

  return ;
}

/**
 *  Process a module event.
 *
 *  @param[in] e Uncasted module.
 */
void stream::_process_module(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::module const& m(*static_cast<neb::module const*>(e.data()));

  // Log message.
  logging::info(logging::medium)
    << "SQL: processing module event (instance: " << m.instance_id
    << ", filename: " << m.filename << ", loaded: "
    << (m.loaded ? "yes" : "no") << ")";

  // Processing.
  if (m.enabled) {
    _module_insert << m;
    _module_insert.run_statement("SQL");
  }
  else {
    database_query q(_db);
    q.prepare(
      "DELETE FROM modules "
      "WHERE instance_id=:instance_id"
      "  AND filename=:filename",
      "SQL");
    q.bind_value(":instance_id", m.instance_id);
    q.bind_value(":filename", m.filename);
    q.run_statement("SQL");
  }

  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing notification event";

  // Processing.
  _update_on_none_insert(
    _notification_insert,
    _notification_update,
    *static_cast<neb::notification const*>(e.data()));

  return ;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void stream::_process_service(
               misc::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(e.data()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing service event "
       "(host id: " << s.host_id << ", service_id: " << s.service_id
    << ", description: " << s.service_description << ")";

  // Processing.
  if (s.host_id && s.service_id)
    _update_on_none_insert(
      _service_insert,
      _service_update,
      s);
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
void stream::_process_service_check(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_check const&
    sc(*static_cast<neb::service_check const*>(e.data()));

  time_t now(time(NULL));
  if (sc.check_type                // - passive result
      || !sc.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
                                   // - normal case
      || (sc.next_check >= now - 5 * 60)
      || !sc.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing service check event (host: " << sc.host_id
      << ", service: " << sc.service_id << ", command: "
      << sc.command_line << ", from instance: " << sc.instance_id << ")";

    // Processing.
    _service_check_update << sc;
    _service_check_update.run_statement("SQL");
    if (_service_check_update.num_rows_affected() != 1)
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
void stream::_process_service_dependency(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_dependency const&
    sd(*static_cast<neb::service_dependency const*>(e.data()));

  // Insert/Update.
  if (sd.enabled) {
    logging::info(logging::medium)
      << "SQL: enabling service dependency of (" << sd.dependent_host_id
      << ", " << sd.dependent_service_id << ") on (" << sd.host_id
      << ", " << sd.service_id << ")";
    _update_on_none_insert(
      _service_dependency_insert,
      _service_dependency_update,
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
    database_query q(_db);
    q.run_query(oss.str(), "SQL");
  }

  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void stream::_process_service_group(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_group const&
    sg(*static_cast<neb::service_group const*>(e.data()));

  if (sg.enabled) {
    logging::info(logging::medium) << "SQL: enabling service group '"
      << sg.name << "' of instance: " << sg.instance_id;
    _update_on_none_insert(
      _service_group_insert,
      _service_group_update,
      sg);
  }
  else {
    logging::info(logging::medium) << "SQL: disabling service group '"
      << sg.name << "' of instance: " << sg.instance_id;

    // Delete group members.
    {
      std::ostringstream oss;
      oss << "DELETE " << mapped_type<neb::service_group_member>::table
          << "  FROM " << mapped_type<neb::service_group_member>::table << " AS sgm"
          << "  LEFT JOIN " << mapped_type<neb::host>::table << " AS h"
          << "    ON sgm.host_id=h.host_id"
          << "  WHERE sgm.servicegroup_id=" << sg.id
          << "    AND h.instance_id=" << sg.instance_id;
      database_query q(_db);
      q.run_query(oss.str(), "SQL");
    }

    // Delete empty groups.
    _empty_service_group_delete.run_statement("SQL");
  }

  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void stream::_process_service_group_member(
               misc::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_group_member const&
    sgm(*static_cast<neb::service_group_member const*>(e.data()));

  // Insert.
  if (sgm.enabled) {
    // Log message.
    logging::info(logging::medium)
      << "SQL: enabling service group member (group: "
      << sgm.group_id << ", instance: " << sgm.instance_id << ", host: "
      << sgm.host_id << ", service: " << sgm.service_id << ")";

    // We only need to try to insert in this table as the
    // host_id/service_id/servicegroup_id combo should be UNIQUE.
    try {
      _service_group_member_insert << sgm;
      _service_group_member_insert.run_statement("SQL");
    }
    catch (std::exception const& e) {
      logging::info(logging::high)
        << "SQL: discarding membership between service ("
        << sgm.host_id << ", " << sgm.service_id
        << ") and service group " << sgm.group_id << " on instance "
        << sgm.instance_id << ": " << e.what();
    }
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
      << "SQL: removing service group member (group: "
      << sgm.group_id << ", instance: " << sgm.instance_id << ", host: "
      << sgm.host_id << ", service: " << sgm.service_id << ")";

    try {
      _service_group_member_delete << sgm;
      _service_group_member_delete.run_statement();
    }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "SQL: cannot delete membership of service ("
             << sgm.host_id << ", " << sgm.service_id
             << ") to service group " << sgm.group_id << " on instance "
             << sgm.instance_id << ": " << e.what());
    }
  }

  return ;
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium)
    << "SQL: processing service state event";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(
      _service_state_insert,
      _service_state_update,
      *static_cast<correlation::service_state const*>(e.data()));
  }

  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void stream::_process_service_status(
               misc::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::service_status const&
    ss(*static_cast<neb::service_status const*>(e.data()));

  time_t now(time(NULL));
  if (ss.check_type                // - passive result
      || !ss.active_checks_enabled // - active checks are disabled,
                                   //   status might not be updated
                                   // - normal case
      || (ss.next_check >= now - 5 * 60)
      || !ss.next_check) {         // - initial state
    // Apply to DB.
    logging::info(logging::medium)
      << "SQL: processing service status event (host: "
      << ss.host_id << ", service: " << ss.service_id
      << ", last check: " << ss.last_check << ", state ("
      << ss.current_state << ", " << ss.state_type << "))";

    // Processing.
    _service_status_update << ss;
    _service_status_update.run_statement("SQL");
    if (_service_status_update.num_rows_affected() != 1)
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
 *  Don't process an event.
 *
 *  @param[in] e  The event that won't be processed.
 */
void stream::_null_process(misc::shared_ptr<io::data> const& e)  {
  (void) e;
}

template <typename T>
void stream::_update_on_none_insert(
               database_query& ins,
               database_query& up,
               T& t) {
  // Try update.
  up << t;
  up.run_statement("SQL");

  // Try insertion.
  if (up.num_rows_affected() != 1) {
    ins << t;
    ins.run_statement("SQL");
  }

  return ;
}

/**
 *  Write logs to the DB.
 */
void stream::_write_logs() {
  if (!_log_queue.empty()) {
    // Driver object used for escaping.
    QSqlDriver const* drivr(_db.get_qt_driver());

    // Log insertion query.
    QString q;
    QTextStream query(&q);
    query << "INSERT INTO " << mapped_type<neb::log_entry>::table
          << "  (ctime, host_id, host_name, instance_name, issue_id, "
          << "  msg_type, notification_cmd, notification_contact, "
          << "  output, retry, service_description, service_id, status, "
          << "  type) "
          << "VALUES ";

    // Fields used to escape strings.
    QSqlField host_name_field(
                "host_name",
                QVariant::String);
    QSqlField instance_name_field(
                "instance_name",
                QVariant::String);
    QSqlField notification_cmd_field(
                "notification_cmd",
                QVariant::String);
    QSqlField notification_contact_field(
                "notification_contact",
                QVariant::String);
    QSqlField output_field(
                "output",
                QVariant::String);
    QSqlField service_description_field(
                "service_description",
                QVariant::String);

    // Browse log queue.
    while (!_log_queue.empty()) {
      // Get log object.
      misc::shared_ptr<neb::log_entry>
        le(_log_queue.front().staticCast<neb::log_entry>());
      _log_queue.pop_front();

      // Fetch issue ID (if any).
      int issue;
      if (le->issue_start_time) {
        _issue_select.bind_value(":host_id", le->host_id);
        _issue_select.bind_value(
                        ":service_id",
                        (le->service_id
                         ? QVariant(le->service_id)
                         : QVariant(QVariant::Int)));
        _issue_select.bind_value(
                        ":start_time",
                        static_cast<qlonglong>(
                          le->issue_start_time.get_time_t()));
        _issue_select.run_statement();
        if (_issue_select.next())
          issue = _issue_select.value(0).toInt();
        else
          issue = 0;
      }
      else
        issue = 0;

      // Build insertion query.
      static QString const empty_string("''");
      static QString const null_string("NULL");
      host_name_field.setValue(le->host_name);
      instance_name_field.setValue(le->instance_name);
      notification_cmd_field.setValue(le->notification_cmd);
      notification_contact_field.setValue(le->notification_contact);
      output_field.setValue(le->output);
      service_description_field.setValue(le->service_description);
      query << "(" << le->c_time << ", ";
      if (le->host_id)
        query << le->host_id;
      else
        query << "NULL";
      query << ", " << (host_name_field.isNull()
                        ? null_string
                        : drivr->formatValue(host_name_field)) << ", "
            << (instance_name_field.isNull()
                ? empty_string
                : drivr->formatValue(instance_name_field)) << ", ";
      if (issue)
        query << issue;
      else
        query << "NULL";
      query << ", " << le->msg_type << ", "
            << (notification_cmd_field.isNull()
                ? null_string
                : drivr->formatValue(notification_cmd_field)) << ", "
            << (notification_contact_field.isNull()
                ? null_string
                : drivr->formatValue(notification_contact_field))
            << ", "
            << (output_field.isNull()
                ? empty_string
                : drivr->formatValue(output_field)) << ", " << le->retry
            << ", " << (service_description_field.isNull()
                        ? null_string
                        : drivr->formatValue(service_description_field))
            << ", ";
      if (le->service_id)
        query << le->service_id;
      else
        query << "NULL";
      query << ", " << le->status << ", " << le->log_type << ")";
      if (!_log_queue.empty())
        query << ", ";
    }

    // Execute query.
    query.flush();
    database_query dbq(_db);
    dbq.run_query(
          q.toStdString(),
          "SQL: could not insert some logs");
  }
  return ;
}

/**
 *  Update the store of living instance timestamps.
 *
 *  @param instance_id The id of the instance to have its timestamp updated.
 */
void stream::_update_timestamp(unsigned int instance_id) {
  stored_timestamp::state_type
      s(stored_timestamp::responsive);

  // Find the state of an existing timestamp of it exists.
  std::map<unsigned int, stored_timestamp>::iterator found =
      _stored_timestamps.find(instance_id);
  if (found != _stored_timestamps.end())
    s = found->second.get_state();

  // Update a suddenly alive instance
  if (s == stored_timestamp::unresponsive)
  {
    _update_hosts_and_services_of_instance(instance_id, true);
    s = stored_timestamp::responsive;
  }

  // Insert the timestamp and its state in the store.
  stored_timestamp& timestamp = _stored_timestamps[instance_id];
  timestamp = stored_timestamp(instance_id, s);
  if (_oldest_timestamp > timestamp.get_timestamp())
    _oldest_timestamp = timestamp.get_timestamp();
}

/**
 *  Get all the outdated instances from the database and store them.
 */
void stream::_get_all_outdated_instances_from_db() {
  std::ostringstream ss;
  ss << "SELECT instance_id"
     << " FROM " << mapped_type<neb::instance>::table
     << " WHERE outdated=TRUE";
  database_query q(_db);
  q.run_query(
      ss.str(),
      "SQL: could not get the list of outdated instances");
  while (q.next()) {
    unsigned int instance_id = q.value(0).toUInt();
    stored_timestamp& ts = _stored_timestamps[instance_id];
    ts = stored_timestamp(instance_id, stored_timestamp::unresponsive);
    ts.set_timestamp(timestamp(std::numeric_limits<time_t>::max()));
  }
  return ;
}

/**
 *  Update all the hosts and services of unresponsive instances.
 */
void stream::_update_hosts_and_services_of_unresponsive_instances() {
  // Don't do anything if timeout is deactivated.
  if (_instance_timeout == 0)
    return ;

  if (_stored_timestamps.size() == 0 ||
      std::difftime(std::time(NULL), _oldest_timestamp) <= _instance_timeout)
    return ;

  // Update unresponsive instances which were responsive
  for (std::map<unsigned int, stored_timestamp>::iterator it =
       _stored_timestamps.begin(),
       end = _stored_timestamps.end(); it != end; ++it) {
    if (it->second.get_state() == stored_timestamp::responsive &&
        it->second.timestamp_outdated(_instance_timeout)) {
      it->second.set_state(stored_timestamp::unresponsive);
      _update_hosts_and_services_of_instance(it->second.get_id(), false);
    }
  }

  // Update new oldest timestamp
  _oldest_timestamp = timestamp(std::numeric_limits<time_t>::max());
  for (std::map<unsigned int, stored_timestamp>::iterator it =
       _stored_timestamps.begin(),
       end = _stored_timestamps.end(); it != end; ++it) {
    if (it->second.get_state() == stored_timestamp::responsive &&
        _oldest_timestamp > it->second.get_timestamp())
      _oldest_timestamp = it->second.get_timestamp();
  }
}

/**
 *  Update the hosts and services of one instance.
 *
 *  @param[in] id         The instance id.
 *  @param[in] responsive True if the instance is responsive, false otherwise.
 */
void stream::_update_hosts_and_services_of_instance(
               unsigned int id,
               bool responsive) {
  std::ostringstream ss;
  if (responsive) {
    ss << "UPDATE " << mapped_type<neb::instance>::table
       << "  SET outdated=FALSE"
       << "  WHERE instance_id=" << id;
    database_query q(_db);
    q.run_query(ss.str(), "SQL: could not restore outdated instance");
    ss.str("");
    ss.clear();
    ss << "UPDATE " << mapped_type<neb::host>::table << " AS h"
       << "  LEFT JOIN " << mapped_type<neb::service>::table << " AS s"
       << "  ON h.host_id=s.host_id"
       << "  SET h.state=h.real_state,"
       << "      s.state=s.real_state"
       << "  WHERE h.instance_id = " << id;
    q.run_query(ss.str(), "SQL: could not restore outdated instance");
  }
  else {
    ss << "UPDATE " << mapped_type<neb::instance>::table
       << "  SET outdated=TRUE"
       << "  WHERE instance_id=" << id;
    database_query q(_db);
    q.run_query(ss.str(), "SQL: could not outdate instance");
    ss.str("");
    ss.clear();
    ss << "UPDATE " << mapped_type<neb::host>::table << " AS h"
       << "  LEFT JOIN " << mapped_type<neb::service>::table << " AS s"
       << "  ON h.host_id=s.host_id"
       << "  SET h.real_state=h.state,"
       << "      s.real_state=s.state,"
       << "      h.state=" << HOST_UNREACHABLE << ","
       << "      s.state=" << STATE_UNKNOWN
       << "  WHERE h.instance_id=" << id;
    q.run_query(ss.str(), "SQL: could not outdate instance");
  }
}

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
          std::string const& type,
          std::string const& host,
          unsigned short port,
          std::string const& user,
          std::string const& password,
          std::string const& db,
          unsigned int qpt,
          unsigned int cleanup_check_interval,
          unsigned int instance_timeout,
          bool check_replication,
          bool wse)
  : _db(database_config(
          type,
          host,
          port,
          user,
          password,
          db,
          qpt,
          check_replication)),
    _acknowledgement_insert(_db),
    _acknowledgement_update(_db),
    _comment_insert(_db),
    _comment_update(_db),
    _custom_variable_insert(_db),
    _custom_variable_update(_db),
    _custom_variable_delete(_db),
    _custom_variable_status_update(_db),
    _downtime_insert(_db),
    _downtime_update(_db),
    _empty_host_group_delete(_db),
    _empty_service_group_delete(_db),
    _event_handler_insert(_db),
    _event_handler_update(_db),
    _flapping_status_insert(_db),
    _flapping_status_update(_db),
    _host_insert(_db),
    _host_update(_db),
    _host_check_update(_db),
    _host_dependency_insert(_db),
    _host_dependency_update(_db),
    _host_group_insert(_db),
    _host_group_update(_db),
    _host_group_member_insert(_db),
    _host_group_member_delete(_db),
    _host_parent_insert(_db),
    _host_parent_select(_db),
    _host_state_insert(_db),
    _host_state_update(_db),
    _host_status_update(_db),
    _instance_insert(_db),
    _instance_update(_db),
    _instance_status_update(_db),
    _issue_insert(_db),
    _issue_select(_db),
    _issue_update(_db),
    _issue_parent_insert(_db),
    _issue_parent_update(_db),
    _module_insert(_db),
    _notification_insert(_db),
    _notification_update(_db),
    _service_insert(_db),
    _service_update(_db),
    _service_check_update(_db),
    _service_dependency_insert(_db),
    _service_dependency_update(_db),
    _service_group_insert(_db),
    _service_group_update(_db),
    _service_group_member_insert(_db),
    _service_group_member_delete(_db),
    _service_state_insert(_db),
    _service_state_update(_db),
    _service_status_update(_db),
    _cleanup_thread(
      type,
      host,
      port,
      user,
      password,
      db,
      cleanup_check_interval),
    _pending_events(0),
    _process_out(true),
    _with_state_events(wse),
    _instance_timeout(instance_timeout),
    _oldest_timestamp(std::numeric_limits<time_t>::max()) {
  // Prepare queries.
  _prepare();

  // Get oudated instances.
  _get_all_outdated_instances_from_db();

  // Run cleanup thread.
  _cleanup_thread.start();
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Stop cleanup thread.
  _cleanup_thread.exit();
  _cleanup_thread.wait(-1);
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
 *  Update internal stream cache.
 */
void stream::update() {
  _cache_clean();
  _cache_create();
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

  // Take this event into account.
  ++_pending_events;

  // Check that data exists.
  if (!data.isNull()) {
    // Check that event does not refer to a deleted instance.
    bool deleted(false);
    if ((_cache_deleted_instance_id.find(data->instance_id)
        != _cache_deleted_instance_id.end())
        && (data->type()
            != io::events::data_type<io::events::neb, neb::de_log_entry>::value)) {
      logging::info(logging::low)
        << "SQL: discarding some event related to a deleted instance ("
        << data->instance_id << ")";
      deleted = true;
    }
    else if (io::events::category_of_type(data->type())
                == io::events::correlation
             && io::events::element_of_type(data->type())
                == correlation::de_issue_parent) {
      correlation::issue_parent const&
        ip(*static_cast<correlation::issue_parent const*>(data.data()));
      if (_cache_deleted_instance_id.find(ip.child_instance_id)
          != _cache_deleted_instance_id.end()
          && _cache_deleted_instance_id.find(ip.parent_instance_id)
          != _cache_deleted_instance_id.end()) {
        logging::info(logging::low)
          << "SQL: discarding some issue parent correlation event related to "
          << "a deleted instance (child instance: " << ip.child_instance_id
          << ", parent instance: " << ip.parent_instance_id << ")";
        deleted = true;
      }
    }
    if (!deleted) {
      // Update the timestamp of this instance.
      _update_timestamp(data->instance_id);
      logging::debug(logging::low)
        << "SQL: updating timestamp of instance " << data->instance_id
        << " (" << _oldest_timestamp << ")";

      // Process event.
      unsigned int type(data->type());
      unsigned short cat(io::events::category_of_type(type));
      unsigned short elem(io::events::element_of_type(type));
      if (cat == io::events::neb)
        (this->*(_neb_processing_table[elem]))(data);
      else if (cat == io::events::correlation)
        (this->*(_correlation_processing_table[elem]))(data);
    }
  }
  else {
    logging::info(logging::medium)
      << "SQL: committing transaction";
    _db.commit();
  }

  // Update hosts and services of stopped instances
  _update_hosts_and_services_of_unresponsive_instances();

  // Event acknowledgement.
  logging::debug(logging::low) << "SQL: " << _pending_events
    << " events have not yet been acknowledged";
  if (_db.committed()) {
    _db.clear_committed_flag();
    _write_logs();
    int retval(_pending_events);
    _pending_events = 0;
    return (retval);
  }
  else
    return (0);
}
