/*
** Copyright 2009-2015 Merethis
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
#include <QSqlDriver>
#include <QSqlField>
#include <QTextStream>
#include <QThread>
#include <QMutexLocker>
#include <sstream>
#include <limits>
#include "com/centreon/engine/common.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/database_preparator.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
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
  &stream::_process_issue,
  &stream::_process_issue_parent,
  &stream::_process_state,
  &stream::_process_log_issue
};
void (stream::* const stream::_neb_processing_table[])(misc::shared_ptr<io::data> const&) = {
  NULL,
  &stream::_process_acknowledgement,
  NULL,
  &stream::_process_custom_variable,
  &stream::_process_custom_variable_status,
  &stream::_process_downtime,
  NULL,
  &stream::_process_event_handler,
  &stream::_process_flapping_status,
  &stream::_process_host_check,
  &stream::_process_host_dependency,
  NULL,
  NULL,
  &stream::_process_host,
  &stream::_process_host_parent,
  &stream::_process_host_status,
  &stream::_process_instance,
  &stream::_process_instance_status,
  &stream::_process_log,
  &stream::_process_module,
  //&stream::_process_notification,
  &stream::_process_service_check,
  &stream::_process_service_dependency,
  NULL,
  NULL,
  &stream::_process_service,
  &stream::_process_service_status
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
     << " FROM rt_instances"
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
void stream::_clean_tables(unsigned int instance_id) {
  // Query object.
  database_query q(_db);

  // Disable hosts and services.
  {
    std::ostringstream ss;
    ss << "UPDATE rt_hosts"
          " LEFT JOIN rt_services "
          "  ON rt_hosts.host_id = rt_services.host_id"
          " SET rt_hosts.enabled=0, rt_services.enabled=0"
          " WHERE rt_hosts.instance_id=" << instance_id;
    q.run_query(
        ss.str(),
        "SQL: could not clean hosts and services tables");
  }

  // Remove host dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM rt_hosts_hosts_dependencies"
          " WHERE host_id IN ("
          "  SELECT host_id"
          "   FROM rt_hosts"
          "   WHERE instance_id=" << instance_id << ")"
          " OR dependent_host_id IN ("
          "  SELECT host_id"
          "   FROM rt_hosts"
          "   WHERE instance_id=" << instance_id << ")";
    q.run_query(
        ss.str(),
        "SQL: could not clean host dependencies table");
  }

  // Remove host parents.
  {
    std::ostringstream ss;
    ss << "DELETE FROM rt_hosts_hosts_parents"
          " WHERE child_id IN ("
          "  SELECT host_id"
          "   FROM rt_hosts"
          "   WHERE instance_id=" << instance_id << ")"
          " OR parent_id IN ("
          "  SELECT host_id"
          "   FROM rt_hosts"
          "   WHERE instance_id=" << instance_id << ")";
    q.run_query(
        ss.str(),
        "SQL: could not clean host parents table");
  }

  // Remove service dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM rt_services_services_dependencies"
          " WHERE service_id IN ("
          "  SELECT services.service_id"
          "   FROM rt_services AS services"
          "   JOIN rt_hosts AS hosts"
          "   ON hosts.host_id=services.host_id WHERE hosts.instance_id="
       << instance_id << ")"
          " OR dependent_service_id IN ("
          "  SELECT services.service_id "
          "   FROM rt_services AS services"
          "   JOIN rt_hosts AS hosts"
          "   ON hosts.host_id=services.host_id WHERE hosts.instance_id="
       << instance_id << ")";
    q.run_query(
        ss.str(),
        "SQL: could not clean service dependencies tables");
  }

  // Remove list of modules.
  {
    std::ostringstream ss;
    ss << "DELETE FROM rt_modules"
          " WHERE instance_id=" << instance_id;
    q.run_query(ss.str(), "SQL: could not clean modules table");
  }

  // Remove custom variables.
  {
    std::ostringstream ss;
    ss << "DELETE FROM rt_customvariables"
          " USING rt_customvariables"
          " JOIN rt_hosts"
          "  ON rt_customvariables.host_id = rt_hosts.host_id"
          " WHERE rt_hosts.instance_id=" << instance_id;
    q.run_query(
        ss.str(),
        "SQL: could not clean custom variables table");
  }

  return ;
}

/**
 *  Check if an instance is a valid instance.
 *
 *  @param[in] poller_id  Instance ID.
 *
 *  @return True if instance is valid.
 */
bool stream::_is_valid_poller(unsigned int poller_id) {
  // Check if poller is deleted.
  bool deleted(false);
  if (_cache_deleted_instance_id.find(poller_id)
      != _cache_deleted_instance_id.end()) {
    logging::info(logging::low)
      << "SQL: discarding some event related to a deleted poller ("
      << poller_id << ")";
    deleted = true;
  }

  // Update poller timestamp.
  if (!deleted)
    _update_timestamp(poller_id);

  // Return whether poller is valid or not.
  return (!deleted);
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Prepare queries.
  database_query::excluded_fields excluded;
  database_preparator::event_unique unique;
  {
    unique.clear();
    unique.insert("entry_time");
    unique.insert("host_id");
    unique.insert("service_id");
    database_preparator dbp(
                          neb::acknowledgement::static_type(),
                          unique);
    dbp.prepare_insert(_acknowledgement_insert);
    dbp.prepare_update(_acknowledgement_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    database_preparator dbp(
                          neb::custom_variable::static_type(),
                          unique);
    dbp.prepare_insert(_custom_variable_insert);
    dbp.prepare_update(_custom_variable_update);
    dbp.prepare_delete(_custom_variable_delete);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    database_preparator dbp(
                          neb::custom_variable_status::static_type(),
                          unique);
    dbp.prepare_update(_custom_variable_status_update);
  }
  {
    database_preparator dbp(neb::downtime::static_type());
    dbp.prepare_insert(_downtime_insert);
  }
  {
    std::ostringstream oss;
    oss << "UPDATE rt_downtimes"
           " SET actual_end_time=GREATEST(COALESCE(actual_end_time, -1), :actual_end_time),"
           "     actual_start_time=COALESCE(actual_start_time, :actual_start_time),"
           "     author=:author, cancelled=:cancelled, comment_data=:comment_data,"
           "     deletion_time=:deletion_time, duration=:duration, end_time=:end_time,"
           "     fixed=:fixed, instance_id=:instance_id, internal_id=:internal_id,"
           "     start_time=:start_time, started=:started, triggered_by=:triggered_by,"
           "     type=:type"
           " WHERE entry_time=:entry_time"
           "        AND host_id=:host_id"
           "        AND is_recurring=:is_recurring"
           "        AND recurring_timeperiod=:recurring_timeperiod"
           "        AND COALESCE(service_id, -1)=COALESCE(:service_id, -1)";
    std::string query(oss.str());
    _downtime_update.prepare(query, "SQL: could not prepare query");
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("start_time");
    database_preparator dbp(
                          neb::event_handler::static_type(),
                          unique);
    dbp.prepare_insert(_event_handler_insert);
    dbp.prepare_update(_event_handler_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("event_time");
    database_preparator dbp(
                          neb::flapping_status::static_type(),
                          unique);
    dbp.prepare_insert(_flapping_status_insert);
    dbp.prepare_update(_flapping_status_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    database_preparator dbp(neb::host::static_type(), unique);
    dbp.prepare_insert(_host_insert);
    dbp.prepare_update(_host_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    database_preparator dbp(neb::host_check::static_type(), unique);
    dbp.prepare_update(_host_check_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("dependent_host_id");
    database_preparator dbp(
                          neb::host_dependency::static_type(),
                          unique);
    dbp.prepare_insert(_host_dependency_insert);
    dbp.prepare_update(_host_dependency_update);
  }
  {
    database_preparator dbp(neb::host_parent::static_type());
    dbp.prepare_insert(_host_parent_insert);
  }
  {
    unique.clear();
    unique.insert("host_id");
    database_preparator dbp(neb::host_status::static_type(), unique);
    dbp.prepare_update(_host_status_update);
  }
  {
    unique.clear();
    unique.insert("instance_id");
    database_preparator dbp(neb::instance::static_type(), unique);
    dbp.prepare_insert(_instance_insert);
    dbp.prepare_update(_instance_update);
  }
  {
    unique.clear();
    unique.insert("instance_id");
    database_preparator dbp(
                          neb::instance_status::static_type(),
                          unique);
    dbp.prepare_update(_instance_status_update);
  }
  {
    database_preparator dbp(neb::module::static_type());
    dbp.prepare_insert(_module_insert);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    database_preparator dbp(neb::service::static_type(), unique);
    dbp.prepare_insert(_service_insert);
    dbp.prepare_update(_service_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    database_preparator dbp(neb::service_check::static_type(), unique);
    dbp.prepare_update(_service_check_update);
  }
  {
    unique.clear();
    unique.insert("dependent_host_id");
    unique.insert("dependent_service_id");
    unique.insert("host_id");
    unique.insert("service_id");
    database_preparator dbp(
                          neb::service_dependency::static_type(),
                          unique);
    dbp.prepare_insert(_service_dependency_insert);
    dbp.prepare_update(_service_dependency_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    database_preparator dbp(
                          neb::service_status::static_type(),
                          unique);
    dbp.prepare_update(_service_status_update);
  }
  {
    excluded.clear();
    excluded.insert("service_id");
    _host_state_insert.prepare(
      "INSERT INTO rt_hoststateevents (host_id, start_time, ack_time,"
      "            end_time, in_downtime, state)"
      "  VALUES (:host_id, :start_time, :ack_time, :end_time,"
      "          :in_downtime, :state)");
    _host_state_insert.set_excluded(excluded);
    _host_state_update.prepare(
      "UPDATE rt_hoststateevents SET ack_time=:ack_time,"
      "       end_time=:end_time, in_downtime=:in_downtime,"
      "       state=:state"
      "  WHERE host_id=:host_id AND start_time=:start_time");
    _host_state_update.set_excluded(excluded);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("start_time");
    database_preparator dbp(
                          correlation::state::static_type(),
                          unique);
    dbp.prepare_insert(_service_state_insert);
    dbp.prepare_update(_service_state_update);
  }
  {
    unique.clear();
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("start_time");
    database_preparator dbp(
                          correlation::issue::static_type(),
                          unique);
    dbp.prepare_insert(_issue_insert);
    dbp.prepare_update(_issue_update);
  }
  {
    std::string query(
      "INSERT INTO rt_issues_issues_parents (child_id, end_time,"
      "            start_time, parent_id)"
      " VALUES (:child_id, :end_time, :start_time, :parent_id)");
    _issue_parent_insert.prepare(query);
  }
  {
    std::string query(
      "SELECT issue_id FROM rt_issues"
      " WHERE host_id=:host_id"
      " AND service_id=:service_id"
      " AND start_time=:start_time");
    _issue_select.prepare(query);
  }
  {
    std::string query(
      "UPDATE rt_issues_issues_parents SET end_time=:end_time"
      " WHERE child_id=:child_id"
      "       AND start_time=:start_time"
      "       AND parent_id=:parent_id");
    _issue_parent_update.prepare(query, "SQL: could not prepare query");
  }
  _prepare_select<neb::host_parent>(
                        _host_parent_select,
                        "rt_hosts_hosts_parents");
  return ;
}

/**
 *  Prepare a select statement for later execution.
 *
 *  @param[out] st          Query object.
 *  @param[in]  table_name  The name of the table.
 */
template <typename T>
void stream::_prepare_select(
               database_query& st,
               std::string const& table_name) {
  // Build query string.
  std::string query;
  query = "SELECT * FROM ";
  query.append(table_name);
  query.append(" WHERE ");
  mapping::entry const* entries = T::entries;
  for (size_t i = 0; !entries[i].is_null(); ++i) {
    char const* entry_name(entries[i].get_name());
    if (!entry_name || !entry_name[0])
      continue;
    query.append(entry_name);
    query.append(" = :");
    query.append(entry_name);
    query.append(" AND ");
  }

  query.resize(query.size() - 5);

  // Prepare statement.
  try {
    st.prepare(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "SQL: could not prepare selection query from table '"
           << table_name << "': " << e.what());
  }

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
    << "SQL: processing acknowledgement event (poller: "
    << ack.poller_id << ", host: " << ack.host_id << ", service: "
    << ack.service_id << ", entry time: " << ack.entry_time
    << ", deletion time: " << ack.deletion_time << ")";

  // Processing.
  if (_is_valid_poller(ack.poller_id)) {
    _update_on_none_insert(
      _acknowledgement_insert,
      _acknowledgement_update,
      ack);

    // Update the associated host or service table.
    std::ostringstream query;
    if (ack.service_id == 0)
      query << "UPDATE rt_hosts SET acknowledged ="
            << ack.deletion_time.is_null()
            << "  WHERE host_id = " << ack.host_id;
    else
      query << "UPDATE rt_services SET acknowledged ="
            << ack.deletion_time.is_null()
            << "  WHERE host_id = " << ack.host_id
            << "   AND service_id = " << ack.service_id;
    database_query q(_db);
    q.run_query(query.str(), "SQL: couldn't update acknowledgement flags");
  }

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
    << "SQL: processing downtime event (poller: " << d.poller_id
    << ", host: " << d.host_id << ", service: " << d.service_id
    << ", start time: " << d.start_time << ", end_time: " << d.end_time
    << ", actual start time: " << d.actual_start_time
    << ", actual end time: " << d.actual_end_time << ", duration: "
    << d.duration << ", entry time: " << d.entry_time
    << ", deletion time: " << d.deletion_time << ")";

  // Check if poller is valid.
  if (_is_valid_poller(d.poller_id)) {
    _update_on_none_insert(
      _downtime_insert,
      _downtime_update,
      d);

    // Update the associated host or service table.
    if (!d.is_recurring && !d.actual_start_time.is_null()) {
      std::string operation = d.actual_end_time.is_null() ? "+ 1" : "- 1";
      std::ostringstream query;
      if (d.service_id == 0)
        query << "UPDATE rt_hosts"
                 "       SET scheduled_downtime_depth ="
                 "                    scheduled_downtime_depth " << operation
              << "  WHERE host_id = " << d.host_id;
      else
        query << "UPDATE rt_services"
                 "       SET scheduled_downtime_depth ="
                 "                    scheduled_downtime_depth " << operation
              << "  WHERE host_id = " << d.host_id
              << "   AND service_id = " << d.service_id;
      database_query q(_db);
      q.run_query(query.str(), "SQL: couldn't update scheduled downtime depth");
    }
  }

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
      ss << "UPDATE rt_issues SET end_time=" << now
         << " WHERE end_time=0 OR end_time IS NULL";
      database_query q(_db);
      q.run_query(ss.str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE rt_issues_issues_parents SET end_time=" << now
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
       " (poller: " << h.poller_id << ", id: "
    << h.host_id << ", name: " << h.host_name << ")";

  // Processing
  if (_is_valid_poller(h.poller_id)) {
    if (h.host_id)
      _update_on_none_insert(_host_insert, _host_update, h);
    else
      logging::error(logging::high) << "SQL: host '" << h.host_name
        << "' of poller " << h.poller_id << " has no ID";
  }

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
      << ", command: " << hc.command_line << ")";

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
    oss << "DELETE FROM rt_hosts_hosts_dependencies "
           "WHERE dependent_host_id=" << hd.dependent_host_id
        << "  AND host_id=" << hd.host_id;
    database_query q(_db);
    q.run_query(oss.str(), "SQL");
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
  correlation::state const&
    s(*static_cast<correlation::state const*>(e.data()));
  logging::info(logging::medium)
    << "SQL: processing host state event (host: " << s.host_id
    << ", state: " << s.current_state << ", start time: "
    << s.start_time << ", end time: " << s.end_time << ")";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(
      _host_state_insert,
      _host_state_update,
      s);
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
  logging::info(logging::medium) << "SQL: processing poller event "
    << "(id: " << i.poller_id << ", name: " << i.name << ", running: "
    << (i.is_running ? "yes" : "no") << ")";

  // Clean tables.
  _clean_tables(i.poller_id);

  // Processing.
  if (_is_valid_poller(i.poller_id))
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
    << "SQL: processing poller status event (id: " << is.poller_id
    << ", last alive: " << is.last_alive << ")";

  // Processing.
  if (_is_valid_poller(is.poller_id)) {
    _instance_status_update << is;
    _instance_status_update.run_statement("SQL");
    if (_instance_status_update.num_rows_affected() != 1)
      logging::error(logging::medium) << "SQL: poller "
        << is.poller_id << " was not updated because no matching entry "
           "was found in database";
  }
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
    query << "SELECT issue_id FROM rt_issues WHERE host_id="
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
    query << "SELECT issue_id FROM rt_issues WHERE host_id="
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
  if (ip.end_time != (time_t)-1)
    _issue_parent_update.bind_value(
      ":end_time",
      static_cast<long long>(ip.end_time));
  else
    _issue_parent_update.bind_value(
      ":end_time",
      QVariant(QVariant::Int));
  _issue_parent_update.bind_value(":child_id", child_id);
  _issue_parent_update.bind_value(
    ":start_time",
    static_cast<long long>(ip.start_time));
  _issue_parent_update.bind_value(":parent_id", parent_id);
  logging::debug(logging::low)
    << "SQL: updating issue parenting between child " << child_id
    << " and parent " << parent_id << " (start: " << ip.start_time
    << ", end: " << ip.end_time << ")";
  _issue_parent_update.run_statement(
                         "SQL: issue parent update query failed");
  if (_issue_parent_update.num_rows_affected() <= 0) {
    if (ip.end_time != (time_t)-1)
      _issue_parent_insert.bind_value(
        ":end_time",
        static_cast<long long>(ip.end_time));
    else
      _issue_parent_insert.bind_value(
        ":end_time",
        QVariant(QVariant::Int));
    _issue_parent_insert.bind_value(":child_id", child_id);
    _issue_parent_insert.bind_value(
      ":start_time",
      static_cast<long long>(ip.start_time));
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
    << "SQL: processing module event (poller: " << m.poller_id
    << ", filename: " << m.filename << ", loaded: "
    << (m.loaded ? "yes" : "no") << ")";

  // Processing.
  if (_is_valid_poller(m.poller_id)) {
    if (m.enabled) {
      _module_insert << m;
      _module_insert.run_statement("SQL");
    }
    else {
      database_query q(_db);
      q.prepare(
          "DELETE FROM rt_modules "
          "WHERE instance_id=:instance_id"
          "  AND filename=:filename",
          "SQL");
      q.bind_value(":instance_id", m.poller_id);
      q.bind_value(":filename", m.filename);
      q.run_statement("SQL");
    }
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
  // XXX
  // // Log message.
  // logging::info(logging::medium)
  //   << "SQL: processing notification event";

  // // Processing.
  // _update_on_none_insert(
  //   _notification_insert,
  //   _notification_update,
  //   *static_cast<neb::notification const*>(e.data()));

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
      << sc.command_line << ")";

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
    oss << "DELETE FROM rt_services_services_dependencies "
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
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(
               misc::shared_ptr<io::data> const& e) {
  // Log message.
  correlation::state const&
    s(*static_cast<correlation::state const*>(e.data()));
  logging::info(logging::medium)
    << "SQL: processing service state event (host: " << s.host_id
    << ", service: " << s.service_id << ", state: " << s.current_state
    << ", start time: " << s.start_time << ", end time: " << s.end_time
    << ")";

  // Processing.
  if (_with_state_events) {
    _update_on_none_insert(
      _service_state_insert,
      _service_state_update,
      s);
  }

  return ;
}

/**
 *  Process a state event.
 *
 *  @param[in] e  Uncasted state.
 */
void stream::_process_state(misc::shared_ptr<io::data> const& e) {
  if (e.ref_as<correlation::state const>().service_id == 0)
    _process_host_state(e);
  else
    _process_service_state(e);
}

/**
 *  Process log issue event.
 *
 *  @param[in] e  Uncasted log issue.
 */
void stream::_process_log_issue(misc::shared_ptr<io::data> const& e) {
  // XXX : TODO
  (void) e;
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
    query << "INSERT INTO log_logs"
          << "  (ctime, host_id, host_name, instance_name, issue_id, "
          << "  msg_type,  output, retry, service_description, "
          << "  service_id, status, type) "
          << "VALUES ";

    // Fields used to escape strings.
    QSqlField host_name_field(
                "host_name",
                QVariant::String);
    QSqlField instance_name_field(
                "instance_name",
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
      instance_name_field.setValue(le->poller_name);
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
  if (s == stored_timestamp::unresponsive) {
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
     << " FROM rt_instances"
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
    ss << "UPDATE rt_instances"
       << "  SET outdated=FALSE"
       << "  WHERE instance_id=" << id;
    database_query q(_db);
    q.run_query(ss.str(), "SQL: could not restore outdated instance");
    ss.str("");
    ss.clear();
    ss << "UPDATE rt_hosts AS h"
       << "  LEFT JOIN rt_services AS s"
       << "  ON h.host_id=s.host_id"
       << "  SET h.state=h.real_state,"
       << "      s.state=s.real_state"
       << "  WHERE h.instance_id = " << id;
    q.run_query(ss.str(), "SQL: could not restore outdated instance");
  }
  else {
    ss << "UPDATE rt_instances"
       << "  SET outdated=TRUE"
       << "  WHERE instance_id=" << id;
    database_query q(_db);
    q.run_query(ss.str(), "SQL: could not outdate instance");
    ss.str("");
    ss.clear();
    ss << "UPDATE rt_hosts AS h"
       << "  LEFT JOIN rt_services AS s"
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
    _custom_variable_insert(_db),
    _custom_variable_update(_db),
    _custom_variable_delete(_db),
    _custom_variable_status_update(_db),
    _downtime_insert(_db),
    _downtime_update(_db),
    _event_handler_insert(_db),
    _event_handler_update(_db),
    _flapping_status_insert(_db),
    _flapping_status_update(_db),
    _host_insert(_db),
    _host_update(_db),
    _host_check_update(_db),
    _host_dependency_insert(_db),
    _host_dependency_update(_db),
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
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from SQL database");
  return (true);
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
  // Take this event into account.
  ++_pending_events;

  // Check that data exists.
  if (!data.isNull()) {
    // Process event.
    unsigned int type(data->type());
    unsigned short cat(io::events::category_of_type(type));
    unsigned short elem(io::events::element_of_type(type));
    if (cat == io::events::neb)
      (this->*(_neb_processing_table[elem]))(data);
    else if (cat == io::events::correlation)
      (this->*(_correlation_processing_table[elem]))(data);
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
