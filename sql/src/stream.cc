/*
** Copyright 2009-2017 Centreon
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

#include "com/centreon/broker/sql/stream.hh"
#include <ctime>
#include <limits>
#include <sstream>
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/host.hh"
#include "com/centreon/engine/service.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::sql;
using namespace com::centreon::broker::database;

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Processing tables.
void (stream::*const stream::_correlation_processing_table[])(
    std::shared_ptr<io::data> const&) = {nullptr,
                                         &stream::_process_engine,
                                         &stream::_process_issue,
                                         &stream::_process_issue_parent,
                                         &stream::_process_state,
                                         &stream::_process_log_issue};

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
     << "  FROM "
     << ((_mysql.schema_version() == mysql::v2) ? "instances" : "rt_instances")
     << " WHERE deleted=1";

  std::promise<mysql_result> promise;
  _mysql.run_query_and_get_result(ss.str(), &promise);
  try {
    mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res))
      _cache_deleted_instance_id.insert(res.value_as_u32(0));
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "SQL: could not get list of deleted instances: " << e.what();
  }
}

/**
 * Create the cache to link host id to instance id.
 */
void stream::_host_instance_cache_create() {
  _cache_host_instance.clear();
  std::ostringstream oss;

  std::promise<mysql_result> promise;
  _mysql.run_query_and_get_result("SELECT host_id, instance_id FROM hosts",
                                  &promise);

  try {
    mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res))
      _cache_host_instance[res.value_as_u32(0)] = res.value_as_u32(1);
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "SQL: could not get the list of host/instance pairs: " << e.what();
  }
}

/**
 *  Remove host groups with no members from host groups table.
 *
 * @param instance_id Poller instance id
 */
void stream::_clean_empty_host_groups(int instance_id) {
//  logging::debug(logging::low)
//      << "SQL: remove empty host groups (instance_id:" << instance_id << ")";
//  _transversal_mysql.run_query(
//      "DELETE hg FROM hostgroups AS hg"
//      " LEFT JOIN hosts_hostgroups AS hhg"
//      " ON hg.hostgroup_id=hhg.hostgroup_id"
//      " WHERE hhg.hostgroup_id IS NULL",
//      "SQL: could not remove empty host groups", false);
}

/**
 *  Remove service groups with no members from service groups table.
 *
 * @param instance_id Poller instance id
 */
void stream::_clean_empty_service_groups(int instance_id) {
//  logging::debug(logging::low)
//      << "SQL: remove empty service groups (instance_id:" << instance_id << ")";
//
//  _transversal_mysql.run_query(
//      "DELETE sg FROM servicegroups AS sg"
//      " LEFT JOIN services_servicegroups as ssg"
//      " ON sg.servicegroup_id=ssg.servicegroup_id"
//      " WHERE ssg.servicegroup_id IS NULL",
//      "SQL: could not remove empty service groups", false);
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
//  // Database version.
//  bool db_v2(_mysql.schema_version() == mysql::v2);
//
//  int thread_id(_mysql.choose_connection_by_instance(instance_id));
//  logging::debug(logging::low)
//      << "SQL: disable hosts and services (instance_id: " << instance_id << ")";
//  // Disable hosts and services.
//  std::ostringstream oss;
//  oss << "UPDATE hosts AS h LEFT JOIN services AS s ON h.host_id = s.host_id SET h.enabled=0, s.enabled=0 WHERE h.instance_id=" << instance_id;
//  _mysql.run_query(oss.str(),
//                   "SQL: could not clean hosts and services tables: ", false,
//                   thread_id);
//  _mysql.commit(thread_id);
//
//  // Remove host group memberships.
//  if (db_v2) {
//    logging::debug(logging::low)
//        << "SQL: remove host group memberships (instance_id:" << instance_id
//        << ")";
//    oss.str("");
//    oss << "DELETE hosts_hostgroups"
//        << " FROM hosts_hostgroups"
//        << " LEFT JOIN hosts"
//        << " ON hosts_hostgroups.host_id=hosts.host_id"
//        << " WHERE hosts.instance_id=" << instance_id;
//    _transversal_mysql.run_query(
//        oss.str(),
//        "SQL: could not clean host groups memberships table: ", false);
//  }
//
//  // Remove service group memberships
//  if (db_v2) {
//    logging::debug(logging::low)
//        << "SQL: remove service group memberships (instance_id:" << instance_id
//        << ")";
//    oss.str("");
//    oss << "DELETE services_servicegroups"
//        << " FROM services_servicegroups"
//        << " LEFT JOIN hosts"
//        << "   ON services_servicegroups.host_id=hosts.host_id"
//        << " WHERE hosts.instance_id=" << instance_id;
//    _transversal_mysql.run_query(
//        oss.str(),
//        "SQL: could not clean service groups memberships table: ", false);
//  }
//
//  // Remove host groups.
//  if (db_v2)
//    _clean_empty_host_groups(instance_id);
//
//  // Remove service groups.
//  if (db_v2)
//    _clean_empty_service_groups(instance_id);
//
//  // Remove host dependencies.
//  logging::debug(logging::low)
//      << "SQL: remove host dependencies (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "DELETE hhd FROM hosts_hosts_dependencies AS hhd"
//         " INNER JOIN hosts as h"
//         " ON hhd.host_id=h.host_id OR hhd.dependent_host_id=h.host_id"
//         " WHERE h.instance_id="
//      << instance_id;
//  _transversal_mysql.run_query(
//      oss.str(), "SQL: could not clean host dependencies table: ", false);
//
//  // Remove host parents.
//  logging::debug(logging::low)
//      << "SQL: remove host parents (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "DELETE hhp FROM hosts_hosts_parents AS hhp"
//         " INNER JOIN hosts as h"
//         " ON hhp.child_id=h.host_id OR hhp.parent_id=h.host_id"
//         " WHERE h.instance_id="
//      << instance_id;
//  _transversal_mysql.run_query(
//      oss.str(), "SQL: could not clean host parents table: ", false);
//
//  // Remove service dependencies.
//  logging::debug(logging::low)
//      << "SQL: remove service dependencies (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "DELETE ssd FROM services_services_dependencies AS ssd"
//         " INNER JOIN services as s"
//         " ON ssd.service_id=s.service_id OR "
//         "ssd.dependent_service_id=s.service_id"
//         " INNER JOIN hosts as h"
//         " ON s.host_id=h.host_id"
//         " WHERE h.instance_id="
//      << instance_id;
//  _transversal_mysql.run_query(
//      oss.str(), "SQL: could not clean service dependencies table: ", false);
//
//  // Remove list of modules.
//  logging::debug(logging::low)
//      << "SQL: remove list of modules (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "DELETE FROM " << (db_v2 ? "modules" : "rt_modules")
//      << " WHERE instance_id=" << instance_id;
//  _transversal_mysql.run_query(oss.str(),
//                               "SQL: could not clean modules table: ", false);
//
//  // Cancellation of downtimes.
//  logging::debug(logging::low)
//      << "SQL: Cancellation of downtimes (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "UPDATE downtimes AS d"
//         " INNER JOIN hosts AS h"
//         " ON d.host_id=h.host_id"
//         " SET d.cancelled=1"
//         " WHERE d.actual_end_time IS NULL"
//         " AND d.cancelled=0"
//         " AND h.instance_id="
//      << instance_id;
//  _transversal_mysql.run_query(oss.str(),
//                               "SQL: could not clean downtimes table: ", false);
//
//  // Remove comments.
//  if (db_v2) {
//    logging::debug(logging::low)
//        << "SQL: remove comments (instance_id:" << instance_id << ")";
//    oss.str("");
//    oss << "UPDATE comments AS c"
//           " JOIN hosts AS h"
//           " ON c.host_id=h.host_id"
//           " SET c.deletion_time="
//        << time(nullptr) << " WHERE h.instance_id=" << instance_id
//        << " AND c.persistent=0"
//           " AND (c.deletion_time IS NULL OR c.deletion_time=0)";
//    _transversal_mysql.run_query(
//        oss.str(), "SQL: could not clean comments table: ", false);
//  }
//
//  // Remove custom variables. No need to choose the good instance, there are
//  // no constraint between custom variables and instances.
//  logging::debug(logging::low)
//      << "SQL: remove custom variables (instance_id:" << instance_id << ")";
//  oss.str("");
//  oss << "DELETE cv"
//      << " FROM " << (db_v2 ? "customvariables" : "rt_customvariables")
//      << "  AS cv"
//         " INNER JOIN "
//      << (db_v2 ? "hosts" : "rt_hosts")
//      << " AS h"
//         "  ON cv.host_id = h.host_id"
//         " WHERE h.instance_id="
//      << instance_id;
//
//  _transversal_mysql.run_query(
//      oss.str(), "SQL: could not clean custom variables table: ", false);
}

/**
 *  Check if an instance is a valid instance.
 *
 *  @param[in] poller_id  Instance ID.
 *
 *  @return True if instance is valid.
 */
//bool stream::_is_valid_poller(unsigned int poller_id) {
//  // Check if poller is deleted.
//  bool deleted(false);
//  if (_cache_deleted_instance_id.find(poller_id) !=
//      _cache_deleted_instance_id.end()) {
//    logging::info(logging::low)
//        << "SQL: discarding some event related to a deleted poller ("
//        << poller_id << ")";
//    deleted = true;
//  }
//
//  // Update poller timestamp.
//  if (!deleted)
//    _update_timestamp(poller_id);
//
//  // Return whether poller is valid or not.
//  return !deleted;
//}

/**
 *  Process a correlation engine event.
 *
 *  @param[in] e Uncasted correlation engine event.
 */
void stream::_process_engine(std::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium) << "SQL: processing correlation engine event";

  // Cast event.
  correlation::engine_state const& es(
      *static_cast<correlation::engine_state const*>(e.get()));

  // Database schema version.
  bool db_v2(_mysql.schema_version() == mysql::v2);

  // Close issues.
  if (es.started) {
    time_t now(time(nullptr));
    {
      std::ostringstream ss;
      ss << "UPDATE " << (db_v2 ? "issues" : "rt_issues")
         << "  SET end_time=" << now
         << "  WHERE end_time=0 OR end_time IS NULL";
      _mysql.run_query(ss.str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE "
         << (db_v2 ? "issues_issues_parents" : "rt_issues_issues_parents")
         << "  SET end_time=" << now
         << "  WHERE end_time=0 OR end_time IS NULL";
      _mysql.run_query(ss.str());
    }
  }
}

void stream::_prepare_hg_insupdate_statement() {
//  if (!_host_group_insupdate.prepared()) {
//    query_preparator::event_unique unique;
//    unique.insert("hostgroup_id");
//    query_preparator qp(neb::host_group::static_type(), unique);
//    _host_group_insupdate = qp.prepare_insert_or_update(_transversal_mysql);
//  }
}

void stream::_prepare_sg_insupdate_statement() {
//  if (!_service_group_insupdate.prepared()) {
//    query_preparator::event_unique unique;
//    unique.insert("servicegroup_id");
//    query_preparator qp(neb::service_group::static_type(), unique);
//    _service_group_insupdate = qp.prepare_insert_or_update(_transversal_mysql);
//  }
}

/**
 *  Process a host state event.
 *
 *  @param[in] e Uncasted host state.
 */
void stream::_process_host_state(std::shared_ptr<io::data> const& e) {
  bool db_v2(_mysql.schema_version() == mysql::v2);
  // Log message.
  correlation::state const& s(*static_cast<correlation::state const*>(e.get()));
  logging::info(logging::medium)
      << "SQL: processing host state event (host: " << s.host_id
      << ", state: " << s.current_state << ", start time: " << s.start_time
      << ", end time: " << s.end_time << ")";

  // Prepare queries.
  if (!_host_state_insupdate.prepared()) {
    std::ostringstream ss;
    ss << "INSERT INTO " << (db_v2 ? "hoststateevents" : "rt_hoststateevents")
       << " (host_id, start_time, ack_time,"
          "            end_time, in_downtime, state)"
          "  VALUES (:host_id, :start_time, :ack_time, :end_time,"
          "          :in_downtime, :state)"
          " ON DUPLICATE KEY UPDATE"
          " ack_time=:ack_time,"
          " end_time=:end_time, in_downtime=:in_downtime,"
          " state=:state";
    _host_state_insupdate = mysql_stmt(ss.str(), true);
    _mysql.prepare_statement(_host_state_insupdate);
  }

  // Processing.
  if (_with_state_events) {
    std::ostringstream oss;
    oss << "SQL: could not store host state event (host: " << s.host_id
        << ", start time " << s.start_time << "): ";

    _host_state_insupdate << s;
    _mysql.run_statement(_host_state_insupdate, oss.str(), true,
                         _mysql.choose_connection_by_instance(s.poller_id));
  }
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void stream::_process_issue(std::shared_ptr<io::data> const& e) {
  // Issue object.
  correlation::issue const& i(*static_cast<correlation::issue const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing issue event (node: (" << i.host_id << ", "
      << i.service_id << "), start time: " << i.start_time
      << ", end_time: " << i.end_time << ", ack time: " << i.ack_time << ")";

  // Prepare queries.
  if (!_issue_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("start_time");
    query_preparator qp(correlation::issue::static_type(), unique);
    _issue_insupdate = qp.prepare_insert_or_update(_mysql);
  }

  // Processing.
  std::ostringstream oss;
  oss << "SQL: could not store issue (host: " << i.host_id
      << ", service: " << i.service_id << ", start time: " << i.start_time
      << "): ";

  _issue_insupdate << i;
  _mysql.run_statement(
      _issue_insupdate, oss.str(), true,
      _mysql.choose_connection_by_instance(_cache_host_instance[i.host_id]));
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void stream::_process_issue_parent(std::shared_ptr<io::data> const& e) {
  // Issue parent object.
  correlation::issue_parent const& ip(
      *static_cast<correlation::issue_parent const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing issue parent "
         "event (child: ("
      << ip.child_host_id << ", " << ip.child_service_id << ", "
      << ip.child_start_time << "), parent: (" << ip.parent_host_id << ", "
      << ip.parent_service_id << ", " << ip.parent_start_time
      << "), start time: " << ip.start_time << ", end time: " << ip.end_time
      << ")";

  // Database schema version.
  bool db_v2(_mysql.schema_version() == mysql::v2);

  // Prepare queries.
  if (!_issue_parent_insert.prepared() || !_issue_parent_update.prepared()) {
    {
      std::ostringstream ss;
      ss << "INSERT INTO "
         << (db_v2 ? "issues_issues_parents" : "rt_issues_issues_parents")
         << "  (child_id, end_time, start_time, parent_id)"
            "  VALUES (:child_id, :end_time, :start_time, :parent_id)";
      _issue_parent_insert = _mysql.prepare_query(ss.str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE "
         << (db_v2 ? "issues_issues_parents" : "rt_issues_issues_parents")
         << "  SET end_time=:end_time"
            "  WHERE child_id=:child_id"
            "    AND start_time=:start_time"
            "    AND parent_id=:parent_id";
      _issue_parent_update = _mysql.prepare_query(ss.str());
    }
  }

  int child_id;
  int parent_id;

  // Get child ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id"
          << "  FROM " << (db_v2 ? "issues" : "rt_issues")
          << "  WHERE host_id=" << ip.child_host_id << " AND service_id";
    if (ip.child_service_id)
      query << "=" << ip.child_service_id;
    else
      query << " IS NULL";
    query << " AND start_time=" << ip.child_start_time;

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(
        query.str(), &promise,
        _mysql.choose_connection_by_instance(
            _cache_host_instance[ip.child_host_id]));
    try {
      mysql_result res(promise.get_future().get());
      if (!_mysql.fetch_row(res))
        throw exceptions::msg() << "child issue does not exist";

      child_id = res.value_as_i32(0);
      logging::debug(logging::low)
          << "SQL: child issue ID of (" << ip.child_host_id << ", "
          << ip.child_service_id << ", " << ip.child_start_time << ") is "
          << child_id;
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not fetch child issue "
             "ID (host: "
          << ip.child_host_id << ", service: " << ip.child_service_id
          << ", start: " << ip.child_start_time << "): " << e.what();
    }
  }

  // Get parent ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id"
             "  FROM "
          << (db_v2 ? "issues" : "rt_issues")
          << "  WHERE host_id=" << ip.parent_host_id << "    AND service_id";
    if (ip.parent_service_id)
      query << "=" << ip.parent_service_id;
    else
      query << " IS NULL";
    query << " AND start_time=" << ip.parent_start_time;

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(
        query.str(), &promise,
        _mysql.choose_connection_by_instance(
            _cache_host_instance[ip.parent_host_id]));
    try {
      mysql_result res(promise.get_future().get());
      if (!_mysql.fetch_row(res))
        throw(exceptions::msg() << "parent issue does not exist");

      parent_id = res.value_as_i32(0);
      logging::debug(logging::low)
          << "SQL: parent issue ID of (" << ip.parent_host_id << ", "
          << ip.parent_service_id << ", " << ip.parent_start_time << ") is "
          << parent_id;
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not fetch parent issue "
             "ID (host: "
          << ip.parent_host_id << ", service: " << ip.parent_service_id
          << ", start: " << ip.parent_start_time << "): " << e.what();
    }
  }

  // End of parenting.
  if (ip.end_time != (time_t)-1) {
    _issue_parent_update.bind_value_as_u32(":end_time",
                                           static_cast<long long>(ip.end_time));
  } else
    _issue_parent_update.bind_value_as_null(":end_time");
  _issue_parent_update.bind_value_as_i32(":child_id", child_id);
  _issue_parent_update.bind_value_as_u32(":start_time",
                                         static_cast<long long>(ip.start_time));
  _issue_parent_update.bind_value_as_i32(":parent_id", parent_id);
  logging::debug(logging::low)
      << "SQL: updating issue parenting between child " << child_id
      << " and parent " << parent_id << " (start: " << ip.start_time
      << ", end: " << ip.end_time << ")";
  std::promise<int> promise;
  _mysql.run_statement_and_get_int(_issue_parent_update, &promise,
                                   mysql_task::AFFECTED_ROWS);
  try {
    if (promise.get_future().get() <= 0) {
      if (ip.end_time != (time_t)-1)
        _issue_parent_insert.bind_value_as_u32(
            ":end_time", static_cast<long long>(ip.end_time));
      else
        _issue_parent_insert.bind_value_as_null(":end_time");
      logging::debug(logging::low)
          << "SQL: inserting issue parenting between child " << child_id
          << " and parent " << parent_id << " (start: " << ip.start_time
          << ", end: " << ip.end_time << ")";
      _mysql.run_statement(_issue_parent_insert,
                           "SQL: issue parent insert query failed");
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "SQL: issue parent update query failed: " << e.what();
  }
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(std::shared_ptr<io::data> const& e
                                   __attribute__((unused))) {
  // XXX
  // // Log message.
  // logging::info(logging::medium)
  //   << "SQL: processing notification event";

  // // Processing.
  // _update_on_none_insert(
  //   _notification_insert,
  //   _notification_update,
  //   *static_cast<neb::notification const*>(e.data()));
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(std::shared_ptr<io::data> const& e) {
  // Log message.
  correlation::state const& s(*static_cast<correlation::state const*>(e.get()));
  logging::info(logging::medium)
      << "SQL: processing service state event (host: " << s.host_id
      << ", service: " << s.service_id << ", state: " << s.current_state
      << ", start time: " << s.start_time << ", end time: " << s.end_time
      << ")";

  // Processing.
  if (_with_state_events) {
    // Prepare queries.
    if (!_service_state_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("service_id");
      unique.insert("start_time");
      query_preparator qp(correlation::state::static_type(), unique);
      _service_state_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store service state event (host: " << s.host_id
        << ", service: " << s.service_id << ", start time: " << s.start_time
        << "): ";

    _service_state_insupdate << s;
    _mysql.run_statement(
        _service_state_insupdate, oss.str(), true,
        _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]));
  }
}

/**
 *  Process a state event.
 *
 *  @param[in] e  Uncasted state.
 */
void stream::_process_state(std::shared_ptr<io::data> const& e) {
  if (std::static_pointer_cast<correlation::state const>(e)->service_id == 0)
    _process_host_state(e);
  else
    _process_service_state(e);
}

/**
 *  Process log issue event.
 *
 *  @param[in] e  Uncasted log issue.
 */
void stream::_process_log_issue(std::shared_ptr<io::data> const& e) {
  // XXX : TODO
  (void)e;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] dbcfg                   Database configuration.
 *  @param[in] instance_timeout        Timeout of instances.
 *  @param[in] with_state_events       With state events.
 */
stream::stream(database_config const& dbcfg,
               unsigned int cleanup_check_interval,
               unsigned int instance_timeout,
               bool with_state_events)
    : _mysql(dbcfg),
//      _cleanup_thread(dbcfg.get_type(),
//                      dbcfg.get_host(),
//                      dbcfg.get_port(),
//                      dbcfg.get_user(),
//                      dbcfg.get_password(),
//                      dbcfg.get_name(),
//                      cleanup_check_interval),
      _ack_events(0),
      _pending_events(0),
      _with_state_events(with_state_events) {
//      _transversal_mysql(database_config(dbcfg.get_type(),
//                                         dbcfg.get_host(),
//                                         dbcfg.get_port(),
//                                         dbcfg.get_user(),
//                                         dbcfg.get_password(),
//                                         dbcfg.get_name())),
//      _oldest_timestamp(std::numeric_limits<time_t>::max()) {
//  // Get oudated instances.
//
//  // Run cleanup thread.
//  _cleanup_thread.start();

  conflict_manager::init_sql(dbcfg, instance_timeout);
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Stop cleanup thread.
  //_cleanup_thread.exit();
  logging::debug(logging::low) << "SQL: sql stream is closed.";
  mysql_manager::instance().clear();
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  // Update hosts and services of stopped instances
  //_update_hosts_and_services_of_unresponsive_instances();

  // Commit transaction.
//  logging::info(logging::medium) << "SQL: committing transaction";
//  _mysql.commit();
  int32_t retval = conflict_manager::instance().get_acks(conflict_manager::sql);
  _pending_events -= retval;

  // Event acknowledgement.
  logging::debug(logging::low)
      << "SQL: " << _pending_events << " events have not yet been acknowledged";
  return retval;
}

/**
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown() << "cannot read from SQL database";
  return true;
}

/**
 *  Update internal stream cache.
 */
void stream::update() {
  _cache_clean();
  _cache_create();
  _host_instance_cache_create();
  _cache_svc_cmd.clear();
  _cache_hst_cmd.clear();
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::write(std::shared_ptr<io::data> const& data) {
  if (!validate(data, "SQL"))
    return 0;

  // Take this event into account.
  ++_pending_events;

  // Process event.
  conflict_manager::instance().send_event(conflict_manager::sql, data);

  return 0;
}
