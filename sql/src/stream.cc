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
#include <iostream>
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
    std::shared_ptr<io::data> const&) = {NULL,
                                         &stream::_process_engine,
                                         &stream::_process_issue,
                                         &stream::_process_issue_parent,
                                         &stream::_process_state,
                                         &stream::_process_log_issue};
void (stream::*const stream::_neb_processing_table[])(
    std::shared_ptr<io::data> const&) = {
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
    &stream::_process_service_check,
    &stream::_process_service_dependency,
    &stream::_process_service_group,
    &stream::_process_service_group_member,
    &stream::_process_service,
    &stream::_process_service_status,
    &stream::_process_instance_configuration,
    &stream::_process_responsive_instance,
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
  logging::debug(logging::low)
      << "SQL: remove empty host groups (instance_id:" << instance_id << ")";
  _transversal_mysql.run_query(
      "DELETE hg FROM hostgroups AS hg"
      " LEFT JOIN hosts_hostgroups AS hhg"
      " ON hg.hostgroup_id=hhg.hostgroup_id"
      " WHERE hhg.hostgroup_id IS NULL",
      "SQL: could not remove empty host groups", false);
}

/**
 *  Remove service groups with no members from service groups table.
 *
 * @param instance_id Poller instance id
 */
void stream::_clean_empty_service_groups(int instance_id) {
  logging::debug(logging::low)
      << "SQL: remove empty service groups (instance_id:" << instance_id << ")";

  _transversal_mysql.run_query(
      "DELETE sg FROM servicegroups AS sg"
      " LEFT JOIN services_servicegroups as ssg"
      " ON sg.servicegroup_id=ssg.servicegroup_id"
      " WHERE ssg.servicegroup_id IS NULL",
      "SQL: could not remove empty service groups", false);
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
  // Database version.
  bool db_v2(_mysql.schema_version() == mysql::v2);

  int thread_id(_mysql.choose_connection_by_instance(instance_id));
  logging::debug(logging::low)
      << "SQL: disable hosts and services (instance_id: " << instance_id << ")";
  // Disable hosts and services.
  std::ostringstream oss;
  oss << "UPDATE " << (db_v2 ? "hosts" : "rt_hosts")
      << " AS h"
         " LEFT JOIN "
      << (db_v2 ? "services" : "rt_services")
      << " AS s"
         " ON h.host_id = s.host_id"
         " SET h.enabled=0, s.enabled=0"
         " WHERE h.instance_id="
      << instance_id;
  _mysql.run_query(oss.str(),
                   "SQL: could not clean hosts and services tables: ", false,
                   thread_id);
  _mysql.commit(thread_id);

  // Remove host group memberships.
  if (db_v2) {
    logging::debug(logging::low)
        << "SQL: remove host group memberships (instance_id:" << instance_id
        << ")";
    oss.str("");
    oss << "DELETE hosts_hostgroups"
        << " FROM hosts_hostgroups"
        << " LEFT JOIN hosts"
        << " ON hosts_hostgroups.host_id=hosts.host_id"
        << " WHERE hosts.instance_id=" << instance_id;
    _transversal_mysql.run_query(
        oss.str(),
        "SQL: could not clean host groups memberships table: ", false);
  }

  // Remove service group memberships
  if (db_v2) {
    logging::debug(logging::low)
        << "SQL: remove service group memberships (instance_id:" << instance_id
        << ")";
    oss.str("");
    oss << "DELETE services_servicegroups"
        << " FROM services_servicegroups"
        << " LEFT JOIN hosts"
        << "   ON services_servicegroups.host_id=hosts.host_id"
        << " WHERE hosts.instance_id=" << instance_id;
    _transversal_mysql.run_query(
        oss.str(),
        "SQL: could not clean service groups memberships table: ", false);
  }

  // Remove host groups.
  if (db_v2)
    _clean_empty_host_groups(instance_id);

  // Remove service groups.
  if (db_v2)
    _clean_empty_service_groups(instance_id);

  // Remove host dependencies.
  logging::debug(logging::low)
      << "SQL: remove host dependencies (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "DELETE hhd FROM hosts_hosts_dependencies AS hhd"
         " INNER JOIN hosts as h"
         " ON hhd.host_id=h.host_id OR hhd.dependent_host_id=h.host_id"
         " WHERE h.instance_id="
      << instance_id;
  _transversal_mysql.run_query(
      oss.str(), "SQL: could not clean host dependencies table: ", false);

  // Remove host parents.
  logging::debug(logging::low)
      << "SQL: remove host parents (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "DELETE hhp FROM hosts_hosts_parents AS hhp"
         " INNER JOIN hosts as h"
         " ON hhp.child_id=h.host_id OR hhp.parent_id=h.host_id"
         " WHERE h.instance_id="
      << instance_id;
  _transversal_mysql.run_query(
      oss.str(), "SQL: could not clean host parents table: ", false);

  // Remove service dependencies.
  logging::debug(logging::low)
      << "SQL: remove service dependencies (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "DELETE ssd FROM services_services_dependencies AS ssd"
         " INNER JOIN services as s"
         " ON ssd.service_id=s.service_id OR "
         "ssd.dependent_service_id=s.service_id"
         " INNER JOIN hosts as h"
         " ON s.host_id=h.host_id"
         " WHERE h.instance_id="
      << instance_id;
  _transversal_mysql.run_query(
      oss.str(), "SQL: could not clean service dependencies table: ", false);

  // Remove list of modules.
  logging::debug(logging::low)
      << "SQL: remove list of modules (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "DELETE FROM " << (db_v2 ? "modules" : "rt_modules")
      << " WHERE instance_id=" << instance_id;
  _transversal_mysql.run_query(oss.str(),
                               "SQL: could not clean modules table: ", false);

  // Cancellation of downtimes.
  logging::debug(logging::low)
      << "SQL: Cancellation of downtimes (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "UPDATE downtimes AS d"
         " INNER JOIN hosts AS h"
         " ON d.host_id=h.host_id"
         " SET d.cancelled=1"
         " WHERE d.actual_end_time IS NULL"
         " AND d.cancelled=0"
         " AND h.instance_id="
      << instance_id;
  _transversal_mysql.run_query(oss.str(),
                               "SQL: could not clean downtimes table: ", false);

  // Remove comments.
  if (db_v2) {
    logging::debug(logging::low)
        << "SQL: remove comments (instance_id:" << instance_id << ")";
    oss.str("");
    oss << "UPDATE comments AS c"
           " JOIN hosts AS h"
           " ON c.host_id=h.host_id"
           " SET c.deletion_time="
        << time(NULL) << " WHERE h.instance_id=" << instance_id
        << " AND c.persistent=0"
           " AND (c.deletion_time IS NULL OR c.deletion_time=0)";
    _transversal_mysql.run_query(
        oss.str(), "SQL: could not clean comments table: ", false);
  }

  // Remove custom variables. No need to choose the good instance, there are
  // no constraint between custom variables and instances.
  logging::debug(logging::low)
      << "SQL: remove custom variables (instance_id:" << instance_id << ")";
  oss.str("");
  oss << "DELETE cv"
      << " FROM " << (db_v2 ? "customvariables" : "rt_customvariables")
      << "  AS cv"
         " INNER JOIN "
      << (db_v2 ? "hosts" : "rt_hosts")
      << " AS h"
         "  ON cv.host_id = h.host_id"
         " WHERE h.instance_id="
      << instance_id;

  _transversal_mysql.run_query(
      oss.str(), "SQL: could not clean custom variables table: ", false);
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
  if (_cache_deleted_instance_id.find(poller_id) !=
      _cache_deleted_instance_id.end()) {
    logging::info(logging::low)
        << "SQL: discarding some event related to a deleted poller ("
        << poller_id << ")";
    deleted = true;
  }

  // Update poller timestamp.
  if (!deleted)
    _update_timestamp(poller_id);

  // Return whether poller is valid or not.
  return !deleted;
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void stream::_process_acknowledgement(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::acknowledgement const& ack(
      *static_cast<neb::acknowledgement const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing acknowledgement event (poller: " << ack.poller_id
      << ", host: " << ack.host_id << ", service: " << ack.service_id
      << ", entry time: " << ack.entry_time
      << ", deletion time: " << ack.deletion_time << ")";

  // Processing.
  if (_is_valid_poller(ack.poller_id)) {
    // Prepare queries.
    if (!_acknowledgement_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("entry_time");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::acknowledgement::static_type(), unique);
      _acknowledgement_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store acknowledgement (poller: " << ack.poller_id
        << ", host: " << ack.host_id << ", service: " << ack.service_id
        << ", entry time: " << ack.entry_time << "): ";

    _acknowledgement_insupdate << ack;
    _mysql.run_statement(_acknowledgement_insupdate, oss.str(), true,
                         _mysql.choose_connection_by_instance(ack.poller_id));
  }
}

/**
 *  Process a comment event.
 *
 *  @param[in] e  Uncasted comment.
 */
void stream::_process_comment(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::comment const& cmmnt(*std::static_pointer_cast<neb::comment const>(e));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing comment of poller " << cmmnt.poller_id << " on ("
      << cmmnt.host_id << ", " << cmmnt.service_id << ")";

  // Prepare queries.
  if (!_comment_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("entry_time");
    unique.insert("instance_id");
    unique.insert("internal_id");
    query_preparator qp(neb::comment::static_type(), unique);
    _comment_insupdate = qp.prepare_insert_or_update(_transversal_mysql);
  }

  // Processing.
  std::ostringstream oss;
  oss << "SQL: could not store comment (poller: " << cmmnt.poller_id
      << ", host: " << cmmnt.host_id << ", service: " << cmmnt.service_id
      << ", entry time: " << cmmnt.entry_time
      << ", internal ID: " << cmmnt.internal_id << "): ";

  _comment_insupdate << cmmnt;
  _transversal_mysql.run_statement(_comment_insupdate, oss.str(), true);
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void stream::_process_custom_variable(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::custom_variable const& cv(
      *static_cast<neb::custom_variable const*>(e.get()));

  // Prepare queries.
  if (!_custom_variable_insupdate.prepared() ||
      !_custom_variable_delete.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    query_preparator qp(neb::custom_variable::static_type(), unique);
    _custom_variable_insupdate =
        qp.prepare_insert_or_update(_transversal_mysql);
    _custom_variable_delete = qp.prepare_delete(_transversal_mysql);
  }

  // Processing.
  if (cv.enabled) {
    logging::info(logging::medium)
        << "SQL: enabling custom variable '" << cv.name << "' of ("
        << cv.host_id << ", " << cv.service_id << ")";
    std::ostringstream oss;
    oss << "SQL: could not store custom variable (name: " << cv.name
        << ", host: " << cv.host_id << ", service: " << cv.service_id << "): ";

    _custom_variable_insupdate << cv;
    _transversal_mysql.run_statement(_custom_variable_insupdate, oss.str(),
                                     true);
  } else {
    logging::info(logging::medium)
        << "SQL: disabling custom variable '" << cv.name << "' of ("
        << cv.host_id << ", " << cv.service_id << ")";
    _custom_variable_delete.bind_value_as_i32(":host_id", cv.host_id);
    _custom_variable_delete.bind_value_as_i32(":service_id", cv.service_id);
    _custom_variable_delete.bind_value_as_str(":name", cv.name);

    std::ostringstream oss;
    oss << "SQL: could not remove custom variable (host: " << cv.host_id
        << ", service: " << cv.service_id << ", name '" << cv.name << "'): ";
    _transversal_mysql.run_statement(
        _custom_variable_delete, oss.str(), true,
        _transversal_mysql.choose_connection_by_instance(
            _cache_host_instance[cv.host_id]));
  }
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void stream::_process_custom_variable_status(
    std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::custom_variable_status const& cvs(
      *static_cast<neb::custom_variable_status const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing custom variable status event (host: " << cvs.host_id
      << ", service: " << cvs.service_id << ", name: " << cvs.name
      << ", update time: " << cvs.update_time << ")";

  // Prepare queries.
  if (!_custom_variable_status_update.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    query_preparator qp(neb::custom_variable_status::static_type(), unique);
    _custom_variable_status_update = qp.prepare_update(_transversal_mysql);
  }

  // Processing.
  _custom_variable_status_update << cvs;
  std::promise<int> promise;
  _transversal_mysql.run_statement_and_get_int(
      _custom_variable_status_update, &promise, mysql_task::AFFECTED_ROWS);
  try {
    if (promise.get_future().get() != 1)
      logging::error(logging::medium)
          << "SQL: custom variable (" << cvs.host_id << ", " << cvs.service_id
          << ", " << cvs.name
          << ") was not updated because it was not found in database";
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "SQL: could not update custom variable (name: " << cvs.name
        << ", host: " << cvs.host_id << ", service: " << cvs.service_id
        << "): " << e.what();
  }
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 */
void stream::_process_downtime(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::downtime const& d(*static_cast<neb::downtime const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing downtime event (poller: " << d.poller_id
      << ", host: " << d.host_id << ", service: " << d.service_id
      << ", start time: " << d.start_time << ", end_time: " << d.end_time
      << ", actual start time: " << d.actual_start_time
      << ", actual end time: " << d.actual_end_time
      << ", duration: " << d.duration << ", entry time: " << d.entry_time
      << ", deletion time: " << d.deletion_time << ")";

  // Check if poller is valid.
  if (_is_valid_poller(d.poller_id)) {
    // Prepare queries.
    if (!_downtime_insupdate.prepared()) {
      std::ostringstream oss;
      oss << "INSERT INTO "
          << ((_transversal_mysql.schema_version() == mysql::v2)
                  ? "downtimes"
                  : "rt_downtimes")
          << " (actual_end_time, "
             "actual_start_time, "
             "author, type, deletion_time, duration, end_time, entry_time, "
             "fixed, host_id, instance_id, internal_id, service_id, "
             "start_time, triggered_by, cancelled, started, comment_data) "
             "VALUES(:actual_end_time,:actual_start_time,:author,:type,:"
             "deletion_time,:duration,:end_time,:entry_time,:fixed,:host_id,:"
             "instance_id,:internal_id,:service_id,:start_time,:triggered_by,:"
             "cancelled,:started,:comment_data) ON DUPLICATE KEY UPDATE "
             "actual_end_time=GREATEST(COALESCE(actual_end_time, -1), "
             ":actual_end_time),"
             "actual_start_time=COALESCE(actual_start_time, "
             ":actual_start_time),"
             "author=:author, cancelled=:cancelled, comment_data=:comment_data,"
             "deletion_time=:deletion_time, duration=:duration, "
             "end_time=:end_time,"
             "fixed=:fixed, host_id=:host_id, service_id=:service_id,"
             "start_time=:start_time, started=:started,"
             "triggered_by=:triggered_by, type=:type";
      _downtime_insupdate = mysql_stmt(oss.str(), true);
      _transversal_mysql.prepare_statement(_downtime_insupdate);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store downtime (poller: " << d.poller_id
        << ", host: " << d.host_id << ", service: " << d.service_id << "): ";

    _downtime_insupdate << d;
    _transversal_mysql.run_statement(_downtime_insupdate, oss.str(), true);
  }
}

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
    time_t now(time(NULL));
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

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void stream::_process_event_handler(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::event_handler const& eh(
      *static_cast<neb::event_handler const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing event handler event (host: " << eh.host_id
      << ", service: " << eh.service_id << ", start time " << eh.start_time
      << ")";

  // Prepare queries.
  if (!_event_handler_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("start_time");
    query_preparator qp(neb::event_handler::static_type(), unique);
    _event_handler_insupdate = qp.prepare_insert_or_update(_mysql);
  }

  // Processing.
  std::ostringstream oss;
  oss << "SQL: could not store event handler (host: " << eh.host_id
      << ", service: " << eh.service_id << ", start time: " << eh.start_time
      << "): ";

  _event_handler_insupdate << eh;
  _mysql.run_statement(
      _event_handler_insupdate, oss.str(), true,
      _mysql.choose_connection_by_instance(_cache_host_instance[eh.host_id]));
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void stream::_process_flapping_status(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::flapping_status const& fs(
      *static_cast<neb::flapping_status const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing flapping status event (host: " << fs.host_id
      << ", service: " << fs.service_id << ", entry time " << fs.event_time
      << ")";

  // Prepare queries.
  if (!_flapping_status_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("event_time");
    query_preparator qp(neb::flapping_status::static_type(), unique);
    _flapping_status_insupdate =
        qp.prepare_insert_or_update(_transversal_mysql);
  }

  // Processing.
  std::ostringstream oss;
  oss << "SQL: could not store flapping status (host: " << fs.host_id
      << ", service: " << fs.service_id << ", event time: " << fs.event_time
      << "): ";

  _flapping_status_insupdate << fs;
  _transversal_mysql.run_statement(_flapping_status_insupdate, oss.str(), true);
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void stream::_process_host(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host const& h(*static_cast<neb::host const*>(e.get()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing host event"
                                    " (poller: "
                                 << h.poller_id << ", id: " << h.host_id
                                 << ", name: " << h.host_name << ")";

  // Processing
  if (_is_valid_poller(h.poller_id)) {
    if (h.host_id) {
      // Prepare queries.
      if (!_host_insupdate.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("host_id");
        query_preparator qp(neb::host::static_type(), unique);
        _host_insupdate = qp.prepare_insert_or_update(_mysql);
      }

      // Process object.
      std::ostringstream oss;
      oss << "SQL: could not store host (poller: " << h.poller_id
          << ", host: " << h.host_id << "): ";

      _host_insupdate << h;
      _mysql.run_statement(_host_insupdate, oss.str(), true,
                           _mysql.choose_connection_by_instance(h.poller_id));

      // Fill the cache...
      _cache_host_instance[h.host_id] = h.poller_id;
    } else
      logging::error(logging::high)
          << "SQL: host '" << h.host_name << "' of poller " << h.poller_id
          << " has no ID";
  }
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 */
void stream::_process_host_check(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_check const& hc(*static_cast<neb::host_check const*>(e.get()));

  time_t now(time(NULL));
  if (hc.check_type                 // - passive result
      || !hc.active_checks_enabled  // - active checks are disabled,
                                    //   status might not be updated
                                    // - normal case
      || (hc.next_check >= now - 5 * 60) ||
      !hc.next_check) {  // - initial state
    // Apply to DB.
    logging::info(logging::medium)
        << "SQL: processing host check event (host: " << hc.host_id
        << ", command: " << hc.command_line << ")";

    // Prepare queries.
    if (!_host_check_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      query_preparator qp(neb::host_check::static_type(), unique);
      _host_check_update = qp.prepare_update(_mysql);
    }

    // Processing.
    bool store(true);
    if (_enable_cmd_cache) {
      size_t str_hash = std::hash<std::string>{}(hc.command_line);
      // Did the command changed since last time?
      if (_cache_hst_cmd[hc.host_id] != str_hash)
        _cache_hst_cmd[hc.host_id] = str_hash;
      else
        store = false;
    }
    if (store) {
      _host_check_update << hc;

      int thread_id(_mysql.choose_connection_by_instance(
          _cache_host_instance[hc.host_id]));
      std::promise<int> promise;
      _mysql.run_statement_and_get_int(_host_check_update, &promise,
                                       mysql_task::AFFECTED_ROWS, thread_id);

      try {
        if (promise.get_future().get() != 1)
          logging::error(logging::medium)
              << "SQL: host check could not "
                 "be updated because host "
              << hc.host_id << " was not found in database";
      } catch (std::exception const& e) {
        throw exceptions::msg()
            << "SQL: could not store host check (host: " << hc.host_id
            << "): " << e.what();
      }
    }
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing host check event (host: " << hc.host_id
        << ", command: " << hc.command_line << ", check type: " << hc.check_type
        << ", next check: " << hc.next_check << ", now: " << now << ")";
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 */
void stream::_process_host_dependency(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_dependency const& hd(
      *static_cast<neb::host_dependency const*>(e.get()));

  // Insert/Update.
  if (hd.enabled) {
    logging::info(logging::medium)
        << "SQL: enabling host dependency of " << hd.dependent_host_id << " on "
        << hd.host_id;

    // Prepare queries.
    if (!_host_dependency_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("dependent_host_id");
      query_preparator qp(neb::host_dependency::static_type(), unique);
      _host_dependency_insupdate =
          qp.prepare_insert_or_update(_transversal_mysql);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store host dependency (host: " << hd.host_id
        << ", dependent host: " << hd.dependent_host_id << "): ";

    _host_dependency_insupdate << hd;
    _transversal_mysql.run_statement(_host_dependency_insupdate, oss.str(),
                                     true);
  }
  // Delete.
  else {
    logging::info(logging::medium)
        << "SQL: removing host dependency of " << hd.dependent_host_id << " on "
        << hd.host_id;
    std::ostringstream oss;
    oss << "DELETE FROM "
        << ((_transversal_mysql.schema_version() == mysql::v2)
                ? "hosts_hosts_dependencies"
                : "rt_hosts_hosts_dependencies")
        << "  WHERE dependent_host_id=" << hd.dependent_host_id
        << "    AND host_id=" << hd.host_id;
    _transversal_mysql.run_query(oss.str(), "SQL: ", true);
  }
}

void stream::_prepare_hg_insupdate_statement() {
  if (!_host_group_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("hostgroup_id");
    query_preparator qp(neb::host_group::static_type(), unique);
    _host_group_insupdate = qp.prepare_insert_or_update(_transversal_mysql);
  }
}

void stream::_prepare_sg_insupdate_statement() {
  if (!_service_group_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("servicegroup_id");
    query_preparator qp(neb::service_group::static_type(), unique);
    _service_group_insupdate = qp.prepare_insert_or_update(_transversal_mysql);
  }
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void stream::_process_host_group(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_group const& hg(*static_cast<neb::host_group const*>(e.get()));

  int thread_id(_mysql.choose_connection_by_instance(hg.poller_id));
  // Only process groups for v2 schema.
  if (_mysql.schema_version() != mysql::v2)
    logging::info(logging::medium)
        << "SQL: discarding host group event (group '" << hg.name
        << "' of instance " << hg.poller_id << ")";
  // Insert/update group.
  else if (hg.enabled) {
    logging::info(logging::medium)
        << "SQL: enabling host group " << hg.id << " ('" << hg.name
        << "') on instance " << hg.poller_id;
    _prepare_hg_insupdate_statement();

    std::ostringstream oss;
    oss << "SQL: could not store host group (poller: " << hg.poller_id
        << ", group: " << hg.id << "): ";

    _host_group_insupdate << hg;
    _transversal_mysql.run_statement(_host_group_insupdate, oss.str(), true);
  }
  // Delete group.
  else {
    logging::info(logging::medium)
        << "SQL: disabling host group " << hg.id << " ('" << hg.name
        << "' on instance " << hg.poller_id;

    // Delete group members.
    {
      std::ostringstream oss;
      oss << "DELETE hosts_hostgroups"
          << "  FROM hosts_hostgroups"
          << "  LEFT JOIN hosts"
          << "    ON hosts_hostgroups.host_id=hosts.host_id"
          << "  WHERE hosts_hostgroups.hostgroup_id=" << hg.id
          << "    AND hosts.instance_id=" << hg.poller_id;
      _transversal_mysql.run_query(oss.str(), "SQL: ", false);
    }
  }
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void stream::_process_host_group_member(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host_group_member const& hgm(
      *static_cast<neb::host_group_member const*>(e.get()));

  // Only process groups for v2 schema.
  if (_transversal_mysql.schema_version() != mysql::v2)
    logging::info(logging::medium)
        << "SQL: discarding membership of host " << hgm.host_id
        << " to host group " << hgm.group_id << " on instance "
        << hgm.poller_id;
  // Insert.
  else if (hgm.enabled) {
    // Log message.
    logging::info(logging::medium)
        << "SQL: enabling membership of host " << hgm.host_id
        << " to host group " << hgm.group_id << " on instance "
        << hgm.poller_id;

    // We only need to try to insert in this table as the
    // host_id/hostgroup_id should be UNIQUE.
    if (!_host_group_member_insert.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("hostgroup_id");
      unique.insert("host_id");
      query_preparator qp(neb::host_group_member::static_type(), unique);
      _host_group_member_insert = qp.prepare_insert(_transversal_mysql);
    }
    _host_group_member_insert << hgm;

    std::promise<mysql_result> promise;
    _transversal_mysql.run_statement_and_get_result(_host_group_member_insert,
                                                    &promise);
    try {
      promise.get_future().get();
    } catch (std::exception const& e) {
      logging::error(logging::low)
          << "SQL: host group member insertion failed: " << e.what();
      _prepare_hg_insupdate_statement();

      neb::host_group hg;
      hg.id = hgm.group_id;
      hg.name = hgm.group_name;
      hg.enabled = true;
      hg.poller_id = _cache_host_instance[hgm.host_id];

      std::ostringstream oss;
      oss << "SQL: could not store host group (poller: " << hg.poller_id
          << ", group: " << hg.id << "): ";

      _host_group_insupdate << hg;
      _transversal_mysql.run_statement(_host_group_insupdate, oss.str(), false);

      oss.str("");
      oss << "SQL: could not store host group membership (poller: "
          << hgm.poller_id << ", host: " << hgm.host_id
          << ", group: " << hgm.group_id << "): ";
      _host_group_member_insert << hgm;
      _transversal_mysql.run_statement(_host_group_member_insert, oss.str(),
                                       false);
    }
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
        << "SQL: disabling membership of host " << hgm.host_id
        << " to host group " << hgm.group_id << " on instance "
        << hgm.poller_id;

    if (!_host_group_member_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("hostgroup_id");
      unique.insert("host_id");
      query_preparator qp(neb::host_group_member::static_type(), unique);
      _host_group_member_delete = qp.prepare_delete(_transversal_mysql);
    }
    std::ostringstream oss;
    oss << "SQL: cannot delete membership of host " << hgm.host_id
        << " to host group " << hgm.group_id << " on instance " << hgm.poller_id
        << ": ";

    _host_group_member_delete << hgm;
    _transversal_mysql.run_statement(_host_group_member_delete, oss.str(),
                                     true);
  }
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void stream::_process_host_parent(std::shared_ptr<io::data> const& e) {
  neb::host_parent const& hp(*static_cast<neb::host_parent const*>(e.get()));

  // Enable parenting.
  if (hp.enabled) {
    // Log message.
    logging::info(logging::medium)
        << "SQL: host " << hp.parent_id << " is parent of host " << hp.host_id;

    // Prepare queries.
    if (!_host_parent_insert.prepared()) {
      query_preparator qp(neb::host_parent::static_type());
      _host_parent_insert = qp.prepare_insert(_transversal_mysql, true);
    }

    // Insert.
    std::ostringstream oss;
    oss << "SQL: could not store host parentship (child host: " << hp.host_id
        << ", parent host: " << hp.parent_id << "): ";

    _host_parent_insert << hp;
    _transversal_mysql.run_statement(_host_parent_insert, oss.str(), false);
  }
  // Disable parenting.
  else {
    logging::info(logging::medium)
        << "SQL: host " << hp.parent_id << " is not parent of host "
        << hp.host_id << " anymore";

    // Prepare queries.
    if (!_host_parent_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("child_id");
      unique.insert("parent_id");
      query_preparator qp(neb::host_parent::static_type(), unique);
      _host_parent_delete = qp.prepare_delete(_transversal_mysql);
    }

    // Delete.
    _host_parent_delete << hp;
    _transversal_mysql.run_statement(_host_parent_delete, "SQL: ", false);
  }
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
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void stream::_process_host_status(std::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::host_status const& hs(*static_cast<neb::host_status const*>(e.get()));

  time_t now(time(NULL));
  if (hs.check_type                 // - passive result
      || !hs.active_checks_enabled  // - active checks are disabled,
                                    //   status might not be updated
                                    // - normal case
      || (hs.next_check >= now - 5 * 60) ||
      !hs.next_check) {  // - initial state
    // Apply to DB.
    logging::info(logging::medium)
        << "SQL: processing host status event (id: " << hs.host_id
        << ", last check: " << hs.last_check << ", state (" << hs.current_state
        << ", " << hs.state_type << "))";

    // Prepare queries.
    if (!_host_status_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      query_preparator qp(neb::host_status::static_type(), unique);
      _host_status_update = qp.prepare_update(_mysql);
    }

    // Processing.
    _host_status_update << hs;
    std::promise<int> promise;
    _mysql.run_statement_and_get_int(
        _host_status_update, &promise, mysql_task::AFFECTED_ROWS,
        _mysql.choose_connection_by_instance(_cache_host_instance[hs.host_id]));
    try {
      if (promise.get_future().get() == 0) {
        std::ostringstream oss;
        oss << "SQL: host could not be "
               "updated because host "
            << hs.host_id << " was not found in database";
        logging::error(logging::medium) << oss.str();
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not store host status (host: " << hs.host_id
          << "): " << e.what();
    }
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing host status event (id: " << hs.host_id
        << ", check type: " << hs.check_type
        << ", last check: " << hs.last_check
        << ", next check: " << hs.next_check << ", now: " << now << ", state ("
        << hs.current_state << ", " << hs.state_type << "))";
}

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 */
void stream::_process_instance(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::instance const& i(*static_cast<neb::instance const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing poller event "
      << "(id: " << i.poller_id << ", name: " << i.name
      << ", running: " << (i.is_running ? "yes" : "no") << ")";

  if (i.is_running) {
    // Clean tables.
    _clean_tables(i.poller_id);
  }

  // Processing.
  if (_is_valid_poller(i.poller_id)) {
    // Prepare queries.
    if (!_instance_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("instance_id");
      query_preparator qp(neb::instance::static_type(), unique);
      _instance_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store poller (poller: " << i.poller_id << "): ";
    _instance_insupdate << i;

    _mysql.run_statement(_instance_insupdate, oss.str(), true,
                         _mysql.choose_connection_by_instance(i.poller_id));
  }
}

/**
 *  Process an instance configuration event.
 *
 *  @param[in] e  Uncasted instance configuration.
 */
void stream::_process_instance_configuration(
    std::shared_ptr<io::data> const& e) {
  // Nothing to do.
  (void)e;
  return;
}

/**
 *  Process an instance status event. To work on an instance status, we must
 *  be sure the instance already exists in the database. So this query must
 *  be done by the same thread as the one that created the instance.
 *
 *  @param[in] e Uncasted instance status.
 */
void stream::_process_instance_status(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::instance_status const& is(
      *static_cast<neb::instance_status const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing poller status event (id: " << is.poller_id
      << ", last alive: " << is.last_alive << ")";

  // Processing.
  if (_is_valid_poller(is.poller_id)) {
    // Prepare queries.
    if (!_instance_status_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("instance_id");
      query_preparator qp(neb::instance_status::static_type(), unique);
      _instance_status_update = qp.prepare_update(_mysql);
    }

    // Process object.
    _instance_status_update << is;
    std::promise<int> promise;
    _mysql.run_statement_and_get_int(
        _instance_status_update, &promise, mysql_task::AFFECTED_ROWS,
        _mysql.choose_connection_by_instance(is.poller_id));

    try {
      if (promise.get_future().get() == 0) {
        std::ostringstream oss;
        oss << "SQL: poller " << is.poller_id
            << " was not updated because no matching entry "
               "was found in database";
        logging::error(logging::medium) << oss.str();
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not update poller (poller: " << is.poller_id
          << "): " << e.what();
    }
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
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void stream::_process_log(std::shared_ptr<io::data> const& e) {
  // Fetch proper structure.
  neb::log_entry const& le(*static_cast<neb::log_entry const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing log of poller '" << le.poller_name
      << "' generated at " << le.c_time << " (type " << le.msg_type << ")";

  // Prepare query.
  if (!_log_insert.prepared()) {
    query_preparator qp(neb::log_entry::static_type());
    _log_insert = qp.prepare_insert(_mysql);
  }

  // Run query.
  _log_insert << le;
  _mysql.run_statement(_log_insert, "SQL: ");
}

/**
 *  Process a module event. We must take care of the thread id sending the
 *  query because the modules table has a constraint on instances.instance_id
 *
 *  @param[in] e Uncasted module.
 */
void stream::_process_module(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::module const& m(*static_cast<neb::module const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing module event (poller: " << m.poller_id
      << ", filename: " << m.filename
      << ", loaded: " << (m.loaded ? "yes" : "no") << ")";

  // Processing.
  if (_is_valid_poller(m.poller_id)) {
    // Prepare queries.
    if (!_module_insert.prepared()) {
      query_preparator qp(neb::module::static_type());
      _module_insert = qp.prepare_insert(_mysql);
    }

    std::ostringstream oss;
    // Process object.
    if (m.enabled) {
      oss << "SQL: could not store module (poller: " << m.poller_id << "): ";
      _module_insert << m;
      _mysql.run_statement(_module_insert, oss.str(), true,
                           _mysql.choose_connection_by_instance(m.poller_id));
    } else {
      oss << "DELETE FROM "
          << ((_mysql.schema_version() == mysql::v2) ? "modules" : "rt_modules")
          << "  WHERE instance_id=" << m.poller_id << "    AND filename='"
          << m.filename << "'";
      _mysql.run_query(oss.str(), "SQL: ", false,
                       _mysql.choose_connection_by_instance(m.poller_id));
    }
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
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void stream::_process_service(std::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(e.get()));

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing service event "
         "(host id: "
      << s.host_id << ", service_id: " << s.service_id
      << ", description: " << s.service_description << ")";

  // Processing.
  if (s.host_id && s.service_id) {
    // Prepare queries.
    if (!_service_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service::static_type(), unique);
      logging::debug(logging::medium) << "mysql: PREPARE INSERT ON SERVICES";
      _service_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    std::ostringstream oss;
    oss << "SQL: could not store service (host: " << s.host_id
        << ", service: " << s.service_id << "): ";
    _service_insupdate << s;
    _mysql.run_statement(
        _service_insupdate, oss.str(), true,
        _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]));
  } else
    logging::error(logging::high) << "SQL: service '" << s.service_description
                                  << "' has no host ID or no service ID";
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void stream::_process_service_check(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_check const& sc(
      *static_cast<neb::service_check const*>(e.get()));

  time_t now(time(NULL));
  if (sc.check_type                 // - passive result
      || !sc.active_checks_enabled  // - active checks are disabled,
                                    //   status might not be updated
                                    // - normal case
      || (sc.next_check >= now - 5 * 60) ||
      !sc.next_check) {  // - initial state
    // Apply to DB.
    logging::info(logging::medium)
        << "SQL: processing service check event (host: " << sc.host_id
        << ", service: " << sc.service_id << ", command: " << sc.command_line
        << ")";

    // Prepare queries.
    if (!_service_check_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_check::static_type(), unique);
      _service_check_update = qp.prepare_update(_mysql);
    }

    // Processing.
    bool store(true);
    if (_enable_cmd_cache) {
      size_t str_hash(std::hash<std::string>{}(sc.command_line));
      // Did the command changed since last time?
      if (_cache_svc_cmd[std::make_pair(sc.host_id, sc.service_id)] != str_hash)
        _cache_svc_cmd[std::make_pair(sc.host_id, sc.service_id)] = str_hash;
      else
        store = false;
    }
    if (store) {
      _service_check_update << sc;
      std::promise<int> promise;
      _mysql.run_statement_and_get_int(_service_check_update, &promise,
                                       mysql_task::AFFECTED_ROWS,
                                       _mysql.choose_connection_by_instance(
                                           _cache_host_instance[sc.host_id]));

      try {
        if (promise.get_future().get() == 0) {
          std::ostringstream oss;
          oss << "SQL: service check could "
                 "not be updated because service ("
              << sc.host_id << ", " << sc.service_id
              << ") was not found in database";
          logging::error(logging::medium) << oss.str();
        }
      } catch (std::exception const& e) {
        throw exceptions::msg()
            << "SQL: could not store service check (host: " << sc.host_id
            << ", service: " << sc.service_id << "): " << e.what();
      }
    }
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing service check event (host: " << sc.host_id
        << ", service: " << sc.service_id << ", command: " << sc.command_line
        << ", check_type: " << sc.check_type
        << ", next_check: " << sc.next_check << ", now: " << now << ")";
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 */
void stream::_process_service_dependency(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_dependency const& sd(
      *static_cast<neb::service_dependency const*>(e.get()));

  // Insert/Update.
  if (sd.enabled) {
    logging::info(logging::medium)
        << "SQL: enabling service dependency of (" << sd.dependent_host_id
        << ", " << sd.dependent_service_id << ") on (" << sd.host_id << ", "
        << sd.service_id << ")";

    // Prepare queries.
    if (!_service_dependency_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("dependent_host_id");
      unique.insert("dependent_service_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_dependency::static_type(), unique);
      _service_dependency_insupdate =
          qp.prepare_insert_or_update(_transversal_mysql);
    }

    // Process object.
    std::ostringstream oss;
    oss << "SQL: could not store service dependency (host: " << sd.host_id
        << ", service: " << sd.service_id
        << ", dependent host: " << sd.dependent_host_id
        << ", dependent service: " << sd.dependent_service_id << "): ";
    _service_dependency_insupdate << sd;
    _transversal_mysql.run_statement(_service_dependency_insupdate, oss.str(),
                                     true);
  }
  // Delete.
  else {
    logging::info(logging::medium)
        << "SQL: removing service dependency of (" << sd.dependent_host_id
        << ", " << sd.dependent_service_id << ") on (" << sd.host_id << ", "
        << sd.service_id << ")";
    std::ostringstream oss;
    oss << "DELETE FROM "
        << ((_transversal_mysql.schema_version() == mysql::v2)
                ? "services_services_dependencies"
                : "rt_services_services_dependencies")
        << "  WHERE dependent_host_id=" << sd.dependent_host_id
        << "    AND dependent_service_id=" << sd.dependent_service_id
        << "    AND host_id=" << sd.host_id
        << "    AND service_id=" << sd.service_id;
    _transversal_mysql.run_query(oss.str(), "SQL: ", false);
  }
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void stream::_process_service_group(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_group const& sg(
      *static_cast<neb::service_group const*>(e.get()));

  // Only process groups for v2 schema.
  if (_transversal_mysql.schema_version() != mysql::v2)
    logging::info(logging::medium)
        << "SQL: discarding service group event (group '" << sg.name
        << "' of instance " << sg.poller_id << ")";
  // Insert/update group.
  else if (sg.enabled) {
    logging::info(logging::medium)
        << "SQL: enabling service group " << sg.id << " ('" << sg.name
        << "') on instance " << sg.poller_id;
    _prepare_sg_insupdate_statement();

    std::stringstream oss;
    oss << "SQL: could not store service group (poller: " << sg.poller_id
        << ", group: " << sg.id << "): ";

    _service_group_insupdate << sg;
    _transversal_mysql.run_statement(_service_group_insupdate, oss.str(), true);
  }
  // Delete group.
  else {
    logging::info(logging::medium)
        << "SQL: disabling service group " << sg.id << " ('" << sg.name
        << "') on instance " << sg.poller_id;

    // Delete group members.
    {
      std::ostringstream oss;
      oss << "DELETE services_servicegroups"
          << "  FROM services_servicegroups"
          << "  LEFT JOIN hosts"
          << "    ON services_servicegroups.host_id=hosts.host_id"
          << "  WHERE services_servicegroups.servicegroup_id=" << sg.id
          << "    AND hosts.instance_id=" << sg.poller_id;
      _transversal_mysql.run_query(oss.str(), "SQL: ", false);
    }
  }
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void stream::_process_service_group_member(std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::service_group_member const& sgm(
      *static_cast<neb::service_group_member const*>(e.get()));

  // Only process groups for v2 schema.
  if (_transversal_mysql.schema_version() != mysql::v2)
    logging::info(logging::medium)
        << "SQL: discarding membership of service (" << sgm.host_id << ", "
        << sgm.service_id << ") to service group " << sgm.group_id
        << " on instance " << sgm.poller_id;
  // Insert.
  else if (sgm.enabled) {
    // Log message.
    logging::info(logging::medium)
        << "SQL: enabling membership of service (" << sgm.host_id << ", "
        << sgm.service_id << ") to service group " << sgm.group_id
        << " on instance " << sgm.poller_id;

    // We only need to try to insert in this table as the
    // host_id/service_id/servicegroup_id combo should be UNIQUE.
    if (!_service_group_member_insert.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("servicegroup_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_group_member::static_type(), unique);
      _service_group_member_insert = qp.prepare_insert(_transversal_mysql);
    }
    _service_group_member_insert << sgm;

    std::promise<mysql_result> promise;

    _transversal_mysql.run_statement_and_get_result(
        _service_group_member_insert, &promise);
    try {
      promise.get_future().get();
    } catch (std::exception const& e) {
      logging::error(logging::low)
          << "SQL: host group not defined: " << e.what();

      _prepare_sg_insupdate_statement();

      neb::service_group sg;
      sg.id = sgm.group_id;
      sg.name = sgm.group_name;
      sg.enabled = true;
      sg.poller_id = sgm.poller_id;

      std::ostringstream oss;
      oss << "SQL: could not store service group (poller: " << sg.poller_id
          << ", group: " << sg.id << "): ";

      _service_group_insupdate << sg;
      _transversal_mysql.run_statement(_service_group_insupdate, oss.str(),
                                       false);

      oss.str("");
      oss << "SQL: could not store service group membership (poller: "
          << sgm.poller_id << ", host: " << sgm.host_id
          << ", service: " << sgm.service_id << ", group: " << sgm.group_id
          << "): ";
      _service_group_member_insert << sgm;
      _transversal_mysql.run_statement(_service_group_member_insert, oss.str(),
                                       false);
    }
  }
  // Delete.
  else {
    // Log message.
    logging::info(logging::medium)
        << "SQL: disabling membership of service (" << sgm.host_id << ", "
        << sgm.service_id << ") to service group " << sgm.group_id
        << " on instance " << sgm.poller_id;

    if (!_service_group_member_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("servicegroup_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_group_member::static_type(), unique);
      _service_group_member_delete = qp.prepare_delete(_transversal_mysql);
    }
    std::ostringstream oss;
    oss << "SQL: cannot delete membership of service (" << sgm.host_id << ", "
        << sgm.service_id << ") to service group " << sgm.group_id
        << " on instance " << sgm.poller_id << ": ";

    _service_group_member_delete << sgm;
    _transversal_mysql.run_statement(_service_group_member_delete, oss.str(),
                                     true);
  }
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

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void stream::_process_service_status(std::shared_ptr<io::data> const& e) {
  // Processed object.
  neb::service_status const& ss(
      *static_cast<neb::service_status const*>(e.get()));

  time_t now(time(NULL));
  if (ss.check_type                 // - passive result
      || !ss.active_checks_enabled  // - active checks are disabled,
                                    //   status might not be updated
                                    // - normal case
      || (ss.next_check >= now - 5 * 60) ||
      !ss.next_check) {  // - initial state
    // Apply to DB.
    logging::info(logging::medium)
        << "SQL: processing service status event (host: " << ss.host_id
        << ", service: " << ss.service_id << ", last check: " << ss.last_check
        << ", state (" << ss.current_state << ", " << ss.state_type << "))";

    // Prepare queries.
    if (!_service_status_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_status::static_type(), unique);
      _service_status_update = qp.prepare_update(_mysql);
    }

    // Processing.
    _service_status_update << ss;
    std::promise<int> promise;
    _mysql.run_statement_and_get_int(
        _service_status_update, &promise, mysql_task::AFFECTED_ROWS,
        _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]));

    try {
      if (promise.get_future().get() != 1)
        logging::error(logging::medium) << "SQL: service could not be "
                                           "updated because service ("
                                        << ss.host_id << ", " << ss.service_id
                                        << ") was not found in database";
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not store service status (host: " << ss.host_id
          << ", service: " << ss.service_id << "): " << e.what();
    }
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing service status event (host: " << ss.host_id
        << ", service: " << ss.service_id << ", check_type: " << ss.check_type
        << ", last check: " << ss.last_check
        << ", next_check: " << ss.next_check << ", now: " << now << ", state ("
        << ss.current_state << ", " << ss.state_type << "))";
}

void stream::_process_responsive_instance(std::shared_ptr<io::data> const& e
                                          __attribute__((unused))) {}

/**
 *  Update the store of living instance timestamps.
 *
 *  @param instance_id The id of the instance to have its timestamp updated.
 */
void stream::_update_timestamp(unsigned int instance_id) {
  stored_timestamp::state_type s(stored_timestamp::responsive);

  // Find the state of an existing timestamp of it exists.
  std::unordered_map<unsigned int, stored_timestamp>::iterator found =
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
     << "  FROM "
     << ((_mysql.schema_version() == mysql::v2) ? "instances" : "rt_instances")
     << " WHERE outdated=TRUE";
  std::promise<mysql_result> promise;
  _mysql.run_query_and_get_result(ss.str(), &promise);
  try {
    mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      unsigned int instance_id = res.value_as_i32(0);
      stored_timestamp& ts = _stored_timestamps[instance_id];
      ts = stored_timestamp(instance_id, stored_timestamp::unresponsive);
      ts.set_timestamp(timestamp(std::numeric_limits<time_t>::max()));
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "SQL: could not get the list of outdated instances: " << e.what();
  }
}

/**
 *  Update all the hosts and services of unresponsive instances.
 */
void stream::_update_hosts_and_services_of_unresponsive_instances() {
  // Log message.
  logging::debug(logging::medium) << "SQL: checking for outdated instances";

  // Don't do anything if timeout is deactivated.
  if (_instance_timeout == 0)
    return;

  if (_stored_timestamps.size() == 0 ||
      std::difftime(std::time(NULL), _oldest_timestamp) <= _instance_timeout)
    return;

  // Update unresponsive instances which were responsive
  for (std::unordered_map<unsigned int, stored_timestamp>::iterator
           it = _stored_timestamps.begin(),
           end = _stored_timestamps.end();
       it != end; ++it) {
    if (it->second.get_state() == stored_timestamp::responsive &&
        it->second.timestamp_outdated(_instance_timeout)) {
      it->second.set_state(stored_timestamp::unresponsive);
      _update_hosts_and_services_of_instance(it->second.get_id(), false);
    }
  }

  // Update new oldest timestamp
  _oldest_timestamp = timestamp(std::numeric_limits<time_t>::max());
  for (std::unordered_map<unsigned int, stored_timestamp>::iterator
           it = _stored_timestamps.begin(),
           end = _stored_timestamps.end();
       it != end; ++it) {
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
void stream::_update_hosts_and_services_of_instance(unsigned int id,
                                                    bool responsive) {
  bool db_v2(_mysql.schema_version() == mysql::v2);
  std::ostringstream ss;
  if (responsive) {
    ss << "UPDATE " << (db_v2 ? "instances" : "rt_instances")
       << "  SET outdated=FALSE"
       << "  WHERE instance_id=" << id;
    _mysql.run_query(ss.str(), "SQL: could not restore outdated instance",
                     false, _mysql.choose_connection_by_instance(id));
    ss.str("");
    ss.clear();
    ss << "UPDATE " << (db_v2 ? "hosts" : "rt_hosts") << " AS h"
       << "  LEFT JOIN " << (db_v2 ? "services" : "rt_services") << "    AS s"
       << "    ON h.host_id=s.host_id"
       << "  SET h.state=h.real_state,"
       << "      s.state=s.real_state"
       << "  WHERE h.instance_id = " << id;
    _mysql.run_query(ss.str(), "SQL: could not restore outdated instance",
                     false, _mysql.choose_connection_by_instance(id));
  } else {
    ss << "UPDATE " << (db_v2 ? "instances" : "rt_instances")
       << "  SET outdated=TRUE"
       << "  WHERE instance_id=" << id;
    _mysql.run_query(ss.str(), "SQL: could not outdate instance", false,
                     _mysql.choose_connection_by_instance(id));
    ss.str("");
    ss.clear();
    ss << "UPDATE " << (db_v2 ? "hosts" : "rt_hosts") << " AS h"
       << "  LEFT JOIN " << (db_v2 ? "services" : "rt_services") << "    AS s"
       << "    ON h.host_id=s.host_id"
       << "  SET h.real_state=h.state,"
       << "      s.real_state=s.state,"
       << "      h.state=" << com::centreon::engine::host::state_unreachable
       << ","
       << "      s.state=" << com::centreon::engine::service::state_unknown
       << "  WHERE h.instance_id=" << id;
    _mysql.run_query(ss.str(), "SQL: could not outdate instance", false,
                     _mysql.choose_connection_by_instance(id));
  }
  std::shared_ptr<neb::responsive_instance> ri(new neb::responsive_instance);
  ri->poller_id = id;
  ri->responsive = responsive;
  multiplexing::publisher().write(ri);
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
      _cleanup_thread(dbcfg.get_type(),
                      dbcfg.get_host(),
                      dbcfg.get_port(),
                      dbcfg.get_user(),
                      dbcfg.get_password(),
                      dbcfg.get_name(),
                      cleanup_check_interval),
      _ack_events(0),
      _pending_events(0),
      _with_state_events(with_state_events),
      _instance_timeout(instance_timeout),
      _oldest_timestamp(std::numeric_limits<time_t>::max()),
      _transversal_mysql(database_config(dbcfg.get_type(),
                                         dbcfg.get_host(),
                                         dbcfg.get_port(),
                                         dbcfg.get_user(),
                                         dbcfg.get_password(),
                                         dbcfg.get_name())) {
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
  _update_hosts_and_services_of_unresponsive_instances();

  // Commit transaction.
  logging::info(logging::medium) << "SQL: committing transaction";
  _mysql.commit();
  int retval(_ack_events + _pending_events);
  _ack_events = 0;
  _pending_events = 0;
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
  throw(exceptions::shutdown() << "cannot read from SQL database");
  return (true);
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
int stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;
  if (!validate(data, "SQL"))
    return 0;

  // Process event.
  unsigned int type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));
  if (cat == io::events::neb)
    (this->*(_neb_processing_table[elem]))(data);
  else if (cat == io::events::correlation)
    (this->*(_correlation_processing_table[elem]))(data);

  // Event acknowledgement.
  logging::debug(logging::low)
      << "SQL: " << _pending_events << " events have not yet been acknowledged";

  int retval(_ack_events);
  _ack_events = 0;
  logging::debug(logging::low) << "SQL: ack events count: " << retval;
  if (retval)
    // Update hosts and services of stopped instances
    _update_hosts_and_services_of_unresponsive_instances();
  // Commit.

  return retval;
}
