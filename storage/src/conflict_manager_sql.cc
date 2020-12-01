/*
** Copyright 2019 Centreon
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
#include <fmt/format.h>

#include "com/centreon/broker/database/mysql_result.hh"
#include "com/centreon/broker/database/table_max_size.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/engine/host.hh"
#include "com/centreon/engine/service.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;
using namespace com::centreon::broker::storage;

/**
 *  @brief Clean tables with data associated to the instance.
 *
 *  Rather than delete appropriate entries in tables, they are instead
 *  deactivated using a specific flag.
 *
 *  @param[in] instance_id Instance ID to remove.
 */
void conflict_manager::_clean_tables(uint32_t instance_id) {
  /* Database version. */

  int32_t conn = _mysql.choose_connection_by_instance(instance_id);
  log_v2::sql()->debug(
      "conflict_manager: disable hosts and services (instance_id: {})",
      instance_id);
  /* Disable hosts and services. */
  std::string query(fmt::format(
      "UPDATE hosts AS h LEFT JOIN services AS s ON h.host_id = s.host_id "
      "SET h.enabled=0, s.enabled=0 WHERE h.instance_id={}",
      instance_id));
  _mysql.run_query(
      query,
      "conflict_manager: could not clean hosts and services tables: ", false,
      conn);
  _add_action(conn, actions::hosts);

  /* Remove host group memberships. */
  log_v2::sql()->debug(
      "conflict_manager: remove host group memberships (instance_id: {})",
      instance_id);
  query = fmt::format(
      "DELETE hosts_hostgroups FROM hosts_hostgroups LEFT JOIN hosts ON "
      "hosts_hostgroups.host_id=hosts.host_id WHERE hosts.instance_id={}",
      instance_id);
  _mysql.run_query(
      query,
      "conflict_manager: could not clean host groups memberships table: ",
      false, conn);
  _add_action(conn, actions::hostgroups);

  /* Remove service group memberships */
  log_v2::sql()->debug(
      "conflict_manager: remove service group memberships (instance_id: {})",
      instance_id);
  query = fmt::format(
      "DELETE services_servicegroups FROM services_servicegroups LEFT JOIN "
      "hosts ON services_servicegroups.host_id=hosts.host_id WHERE "
      "hosts.instance_id={}",
      instance_id);
  _mysql.run_query(
      query, "SQL: could not clean service groups memberships table: ", false,
      conn);
  _add_action(conn, actions::servicegroups);

  /* Remove host groups. */
  log_v2::sql()->debug(
      "conflict_manager: remove empty host groups (instance_id: {})",
      instance_id);
  _mysql.run_query(
      "DELETE hg FROM hostgroups AS hg LEFT JOIN hosts_hostgroups AS hhg ON "
      "hg.hostgroup_id=hhg.hostgroup_id WHERE hhg.hostgroup_id IS NULL",
      "conflict_manager: could not remove empty host groups", false, conn);
  _add_action(conn, actions::hostgroups);

  /* Remove service groups. */
  log_v2::sql()->debug(
      "conflict_manager: remove empty service groups (instance_id: {})",
      instance_id);

  _mysql.run_query(
      "DELETE sg FROM servicegroups AS sg LEFT JOIN services_servicegroups as "
      "ssg ON sg.servicegroup_id=ssg.servicegroup_id WHERE ssg.servicegroup_id "
      "IS NULL",
      "conflict_manager: could not remove empty service groups", false, conn);
  _add_action(conn, actions::servicegroups);

  /* Remove host dependencies. */
  log_v2::sql()->debug(
      "conflict_manager: remove host dependencies (instance_id: {})",
      instance_id);
  query = fmt::format(
      "DELETE hhd FROM hosts_hosts_dependencies AS hhd INNER JOIN hosts as "
      "h ON hhd.host_id=h.host_id OR hhd.dependent_host_id=h.host_id WHERE "
      "h.instance_id={}",
      instance_id);
  _mysql.run_query(
      query,
      "conflict_manager: could not clean host dependencies table: ", false,
      conn);
  _add_action(conn, actions::host_dependencies);

  /* Remove host parents. */
  log_v2::sql()->debug(
      "conflict_manager: remove host parents (instance_id: {})", instance_id);
  query = fmt::format(
      "DELETE hhp FROM hosts_hosts_parents AS hhp INNER JOIN hosts as h ON "
      "hhp.child_id=h.host_id OR hhp.parent_id=h.host_id WHERE "
      "h.instance_id={}",
      instance_id);
  _mysql.run_query(
      query, "conflict_manager: could not clean host parents table: ", false,
      conn);
  _add_action(conn, actions::host_parents);

  /* Remove service dependencies. */
  log_v2::sql()->debug(
      "conflict_manager: remove service dependencies (instance_id: {})",
      instance_id);
  query = fmt::format(
      "DELETE ssd FROM services_services_dependencies AS ssd"
      " INNER JOIN services as s"
      " ON ssd.service_id=s.service_id OR "
      "ssd.dependent_service_id=s.service_id"
      " INNER JOIN hosts as h"
      " ON s.host_id=h.host_id"
      " WHERE h.instance_id={}",
      instance_id);
  _mysql.run_query(
      query, "SQL: could not clean service dependencies table: ", false, conn);
  _add_action(conn, actions::service_dependencies);

  /* Remove list of modules. */
  log_v2::sql()->debug("SQL: remove list of modules (instance_id: {})",
                       instance_id);
  query = fmt::format("DELETE FROM modules WHERE instance_id={}", instance_id);
  _mysql.run_query(
      query, "conflict_manager: could not clean modules table: ", false, conn);
  _add_action(conn, actions::modules);

  // Cancellation of downtimes.
  log_v2::sql()->debug("SQL: Cancellation of downtimes (instance_id: {})",
                       instance_id);
  query = fmt::format(
      "UPDATE downtimes AS d INNER JOIN hosts AS h ON d.host_id=h.host_id "
      "SET d.cancelled=1 WHERE d.actual_end_time IS NULL AND d.cancelled=0 "
      "AND h.instance_id={}",
      instance_id);
  _mysql.run_query(query,
                   "conflict_manager: could not clean downtimes table: ", false,
                   conn);
  _add_action(conn, actions::downtimes);

  // Remove comments.
  log_v2::sql()->debug("conflict_manager: remove comments (instance_id: {})",
                       instance_id);
  query = fmt::format(
      "UPDATE comments AS c JOIN hosts AS h ON c.host_id=h.host_id SET "
      "c.deletion_time={} WHERE h.instance_id={} AND c.persistent=0 AND "
      "(c.deletion_time IS NULL OR c.deletion_time=0)",
      time(nullptr), instance_id);
  _mysql.run_query(
      query, "conflict_manager: could not clean comments table: ", false, conn);
  _add_action(conn, actions::comments);

  // Remove custom variables. No need to choose the good instance, there are
  // no constraint between custom variables and instances.
  log_v2::sql()->debug("Removing custom variables (instance_id: {})",
                       instance_id);
  query = fmt::format(
      "DELETE cv FROM customvariables AS cv INNER JOIN hosts AS h ON "
      "cv.host_id = h.host_id WHERE h.instance_id={}",
      instance_id);

  _finish_action(-1, actions::custom_variables | actions::hosts);
  _mysql.run_query(query,
                   "conflict_manager: could not clean custom variables table: ",
                   false, conn);
  _add_action(conn, actions::custom_variables);
}

/**
 *  Update all the hosts and services of unresponsive instances.
 */
void conflict_manager::_update_hosts_and_services_of_unresponsive_instances() {
  log_v2::sql()->debug("conflict_manager: checking for outdated instances");

  /* Don't do anything if timeout is deactivated. */
  if (_instance_timeout == 0)
    return;

  if (_stored_timestamps.size() == 0 ||
      std::difftime(std::time(nullptr), _oldest_timestamp) <= _instance_timeout)
    return;

  /* Update unresponsive instances which were responsive */
  for (std::unordered_map<uint32_t, stored_timestamp>::iterator
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
  for (std::unordered_map<uint32_t, stored_timestamp>::iterator
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
void conflict_manager::_update_hosts_and_services_of_instance(uint32_t id,
                                                              bool responsive) {
  int32_t conn = _mysql.choose_connection_by_instance(id);
  _finish_action(conn, actions::hosts);
  _finish_action(-1, actions::acknowledgements | actions::modules |
                         actions::downtimes | actions::comments);

  std::string query;
  if (responsive) {
    query = fmt::format(
        "UPDATE instances SET outdated=FALSE WHERE instance_id={}", id);
    _mysql.run_query(query, "SQL: could not restore outdated instance", false,
                     conn);
    _add_action(conn, actions::instances);
    query = fmt::format(
        "UPDATE hosts AS h LEFT JOIN services AS s ON h.host_id=s.host_id "
        "SET h.state=h.real_state,s.state=s.real_state WHERE h.instance_id={}",
        id);
    _mysql.run_query(query, "SQL: could not restore outdated instance", false,
                     conn);
    _add_action(conn, actions::hosts);
  } else {
    query = fmt::format(
        "UPDATE instances SET outdated=TRUE WHERE instance_id={}", id);
    _mysql.run_query(query, "SQL: could not outdate instance", false, conn);
    _add_action(conn, actions::instances);
    query = fmt::format(
        "UPDATE hosts AS h LEFT JOIN services AS s ON h.host_id=s.host_id "
        "SET h.real_state=h.state,s.real_state=s.state,h.state={},s.state={} "
        "WHERE h.instance_id={}",
        com::centreon::engine::host::state_unreachable,
        com::centreon::engine::service::state_unknown, id);
    _mysql.run_query(query, "SQL: could not outdate instance", false, conn);
    _add_action(conn, actions::hosts);
  }
  std::shared_ptr<neb::responsive_instance> ri =
      std::make_shared<neb::responsive_instance>();
  ri->poller_id = id;
  ri->responsive = responsive;
  multiplexing::publisher().write(ri);
}

/**
 *  Update the store of living instance timestamps.
 *
 *  @param instance_id The id of the instance to have its timestamp updated.
 */
void conflict_manager::_update_timestamp(uint32_t instance_id) {
  stored_timestamp::state_type s{stored_timestamp::responsive};

  // Find the state of an existing timestamp if it exists.
  std::unordered_map<uint32_t, stored_timestamp>::iterator found =
      _stored_timestamps.find(instance_id);
  if (found != _stored_timestamps.end()) {
    s = found->second.get_state();

    // Update a suddenly alive instance
    if (s == stored_timestamp::unresponsive) {
      _update_hosts_and_services_of_instance(instance_id, true);
      s = stored_timestamp::responsive;
    }
  }

  // Insert the timestamp and its state in the store.
  stored_timestamp& timestamp = _stored_timestamps[instance_id];
  timestamp = stored_timestamp(instance_id, s);
  if (_oldest_timestamp > timestamp.get_timestamp())
    _oldest_timestamp = timestamp.get_timestamp();
}

bool conflict_manager::_is_valid_poller(uint32_t instance_id) {
  /* Check if the poller of id instance_id is deleted. */
  bool deleted = false;
  if (_cache_deleted_instance_id.find(instance_id) !=
      _cache_deleted_instance_id.end()) {
    log_v2::sql()->info(
        "conflict_manager: discarding some event related to a deleted poller "
        "({})",
        instance_id);
    deleted = true;
  } else
    /* Update poller timestamp. */
    _update_timestamp(instance_id);

  return !deleted;
}

void conflict_manager::_prepare_hg_insupdate_statement() {
  if (!_host_group_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("hostgroup_id");
    query_preparator qp(neb::host_group::static_type(), unique);
    _host_group_insupdate = qp.prepare_insert_or_update(_mysql);
  }
}

void conflict_manager::_prepare_sg_insupdate_statement() {
  if (!_service_group_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("servicegroup_id");
    query_preparator qp(neb::service_group::static_type(), unique);
    _service_group_insupdate = qp.prepare_insert_or_update(_mysql);
  }
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_acknowledgement(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  // Cast object.
  neb::acknowledgement const& ack =
      *static_cast<neb::acknowledgement const*>(d.get());

  // Log message.
  log_v2::sql()->info(
      "processing acknowledgement event (poller: {}, host: {}, service: {}, "
      "entry time: {}, deletion time: {})",
      ack.poller_id, ack.service_id, ack.entry_time, ack.deletion_time);

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

    int32_t conn = _mysql.choose_connection_by_instance(ack.poller_id);
    // Process object.
    std::string msg_error(fmt::format(
        "SQL: could not store acknowledgement (poller: {}"
        ", host: {}, service: {}"
        ", entry time: {}): ",
        ack.poller_id, ack.host_id, ack.service_id, ack.entry_time));

    _acknowledgement_insupdate << ack;
    _mysql.run_statement(_acknowledgement_insupdate, msg_error, true, conn);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e  Uncasted comment.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_comment(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::hosts | actions::instances |
                         actions::host_parents | actions::host_dependencies |
                         actions::service_dependencies);

  // Cast object.
  neb::comment const& cmmnt{*static_cast<neb::comment const*>(d.get())};

  int32_t conn = _mysql.choose_connection_by_instance(cmmnt.poller_id);

  // Log message.
  log_v2::sql()->info("SQL: processing comment of poller {} on ({}, {})",
                      cmmnt.poller_id, cmmnt.host_id, cmmnt.service_id);

  // Prepare queries.
  if (!_comment_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("entry_time");
    unique.insert("instance_id");
    unique.insert("internal_id");
    query_preparator qp(neb::comment::static_type(), unique);
    _comment_insupdate = qp.prepare_insert_or_update(_mysql);
  }

  // Processing.
  std::string err_msg(
      fmt::format("SQL: could not store comment (poller: {}"
                  ", host: {}, service: {}"
                  ", entry time: {}"
                  ", internal ID: {}): ",
                  cmmnt.poller_id, cmmnt.host_id, cmmnt.service_id,
                  cmmnt.entry_time, cmmnt.internal_id));

  _comment_insupdate << cmmnt;
  _mysql.run_statement(_comment_insupdate, err_msg, true, conn);
  *std::get<2>(t) = true;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_custom_variable(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  // Cast object.
  neb::custom_variable const& cv{
      *static_cast<neb::custom_variable const*>(d.get())};

  // Prepare queries.
  if (!_custom_variable_delete.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    query_preparator qp(neb::custom_variable::static_type(), unique);
    _custom_variable_delete = qp.prepare_delete(_mysql);
  }

  // Processing.
  if (cv.enabled) {
    _cv_queue.emplace_back(std::make_pair(
        std::get<2>(t),
        fmt::format(
            "('{}',{},{},'{}',{},{},{},'{}')",
            misc::string::escape(
                cv.name, get_customvariables_col_size(customvariables_name)),
            cv.host_id, cv.service_id,
            misc::string::escape(
                cv.default_value,
                get_customvariables_col_size(customvariables_default_value)),
            cv.modified ? 1 : 0, cv.var_type, cv.update_time,
            misc::string::escape(cv.value, get_customvariables_col_size(
                                               customvariables_value)))));
    /* Here, we do not update the custom variable boolean ack flag, because
     * it will be updated later when the bulk query will be done:
     * conflict_manager::_update_customvariables() */
  } else {
    int conn =
        _mysql.choose_best_connection(neb::custom_variable::static_type());
    _finish_action(-1, actions::custom_variables);

    log_v2::sql()->info("SQL: disabling custom variable '{}' of ({}, {})",
                        cv.name, cv.host_id, cv.service_id);
    _custom_variable_delete.bind_value_as_i32(":host_id", cv.host_id);
    _custom_variable_delete.bind_value_as_i32(":service_id", cv.service_id);
    _custom_variable_delete.bind_value_as_str(":name", cv.name);

    std::string err_msg(
        fmt::format("SQL: could not remove custom variable (host: {}"
                    ", service: {}, name '{}'): ",
                    cv.host_id, cv.service_id, cv.name));
    _mysql.run_statement(_custom_variable_delete, err_msg, true, conn);
    _add_action(conn, actions::custom_variables);
    *std::get<2>(t) = true;
  }
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_custom_variable_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int conn =
      _mysql.choose_best_connection(neb::custom_variable_status::static_type());
  _finish_action(-1, actions::custom_variables);

  // Cast object.
  neb::custom_variable_status const& cv{
      *static_cast<neb::custom_variable_status const*>(d.get())};

  // Prepare queries.
  if (!_custom_variable_status_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("name");
    unique.insert("service_id");
    query_preparator qp(neb::custom_variable_status::static_type(), unique);
    _custom_variable_status_insupdate = qp.prepare_insert_or_update(_mysql);
  }

  log_v2::sql()->info("SQL: enabling custom variable '{}' of ({}, {})", cv.name,
                      cv.host_id, cv.service_id);

  std::string err_msg(
      fmt::format("SQL: could not store custom variable (name: {}"
                  ", host: {}, service: {}): ",
                  cv.name, cv.host_id, cv.service_id));

  _custom_variable_status_insupdate << cv;
  _mysql.run_statement(_custom_variable_status_insupdate, err_msg, true, conn);
  _add_action(conn, actions::custom_variables);
  *std::get<2>(t) = true;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_downtime(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int conn = _mysql.choose_best_connection(neb::downtime::static_type());
  _finish_action(-1, actions::hosts | actions::instances | actions::downtimes |
                         actions::host_parents | actions::host_dependencies |
                         actions::service_dependencies);

  // Cast object.
  neb::downtime const& dd = *static_cast<neb::downtime const*>(d.get());

  // Log message.
  log_v2::sql()->info(
      "SQL: processing downtime event (poller: {}"
      ", host: {}, service: {}, start time: {}, end_time: {}"
      ", actual start time: {}"
      ", actual end time: {}"
      ", duration: {}, entry time: {}"
      ", deletion time: {})",
      dd.poller_id, dd.host_id, dd.service_id, dd.start_time, dd.end_time,
      dd.actual_start_time, dd.actual_end_time, dd.duration, dd.entry_time,
      dd.deletion_time);

  // Check if poller is valid.
  if (_is_valid_poller(dd.poller_id)) {
    // Prepare queries.
    if (!_downtime_insupdate.prepared()) {
      _downtime_insupdate = mysql_stmt(
          "INSERT INTO downtimes (actual_end_time,actual_start_time,author,"
          "type,deletion_time,duration,end_time,entry_time,"
          "fixed,host_id,instance_id,internal_id,service_id,"
          "start_time,triggered_by,cancelled,started,comment_data) "
          "VALUES(:actual_end_time,:actual_start_time,:author,:type,"
          ":deletion_time,:duration,:end_time,:entry_time,:fixed,:host_id,"
          ":instance_id,:internal_id,:service_id,:start_time,"
          ":triggered_by,:cancelled,:started,:comment_data) "
          "ON DUPLICATE KEY UPDATE "
          "actual_end_time=GREATEST(COALESCE(actual_end_time,-1),"
          ":actual_end_time),actual_start_time=COALESCE(actual_start_time,"
          ":actual_start_time),author=:author,cancelled=:cancelled,"
          "comment_data=:comment_data,deletion_time=:deletion_time,duration="
          ":duration,end_time=:end_time,fixed=:fixed,host_id=:host_id,"
          "service_id=:service_id,start_time=:start_time,started=:started,"
          "triggered_by=:triggered_by, type=:type",
          true);
      _mysql.prepare_statement(_downtime_insupdate);
    }

    // Process object.
    std::string err_msg(
        fmt::format("SQL: could not store downtime (poller: {}"
                    ", host: {}, service: {}): ",
                    dd.poller_id, dd.host_id, dd.service_id));

    _downtime_insupdate << dd;
    _mysql.run_statement(_downtime_insupdate, err_msg, true, conn);
    _add_action(conn, actions::downtimes);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_event_handler(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  // Cast object.
  neb::event_handler const& eh =
      *static_cast<neb::event_handler const*>(d.get());

  // Log message.
  log_v2::sql()->info(
      "SQL: processing event handler event (host: {}"
      ", service: {}, start time {})",
      eh.host_id, eh.service_id, eh.start_time);

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
  std::string err_msg(
      fmt::format("SQL: could not store event handler (host: {}"
                  ", service: {}, start time: {}): ",
                  eh.host_id, eh.service_id, eh.start_time));

  _event_handler_insupdate << eh;
  _mysql.run_statement(
      _event_handler_insupdate, err_msg, true,
      _mysql.choose_connection_by_instance(_cache_host_instance[eh.host_id]));
  *std::get<2>(t) = true;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_flapping_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  // Cast object.
  neb::flapping_status const& fs(
      *static_cast<neb::flapping_status const*>(d.get()));

  // Log message.
  log_v2::sql()->info(
      "SQL: processing flapping status event (host: {}, service: {}, entry "
      "time: {})",
      fs.host_id, fs.service_id, fs.event_time);

  // Prepare queries.
  if (!_flapping_status_insupdate.prepared()) {
    query_preparator::event_unique unique;
    unique.insert("host_id");
    unique.insert("service_id");
    unique.insert("event_time");
    query_preparator qp(neb::flapping_status::static_type(), unique);
    _flapping_status_insupdate = qp.prepare_insert_or_update(_mysql);
  }

  // Processing.
  std::string err_msg(
      fmt::format("SQL: could not store flapping status (host: {}"
                  ", service: {}, event time: {}): ",
                  fs.host_id, fs.service_id, fs.event_time));

  _flapping_status_insupdate << fs;
  int32_t conn =
      _mysql.choose_connection_by_instance(_cache_host_instance[fs.host_id]);
  _mysql.run_statement(_flapping_status_insupdate, err_msg, true, conn);
  _add_action(conn, actions::hosts);
  *std::get<2>(t) = true;
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_check(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::instances | actions::downtimes |
                         actions::comments | actions::host_dependencies |
                         actions::host_parents | actions::service_dependencies);

  // Cast object.
  neb::host_check const& hc = *static_cast<neb::host_check const*>(d.get());

  time_t now = time(nullptr);
  if (hc.check_type ||                  // - passive result
      !hc.active_checks_enabled ||      // - active checks are disabled,
                                        //   status might not be updated
      hc.next_check >= now - 5 * 60 ||  // - normal case
      !hc.next_check) {                 // - initial state
    // Apply to DB.
    log_v2::sql()->info(
        "SQL: processing host check event (host: {}, command: {}", hc.host_id,
        hc.command_line);

    // Prepare queries.
    if (!_host_check_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      query_preparator qp(neb::host_check::static_type(), unique);
      _host_check_update = qp.prepare_update(_mysql);
    }

    // Processing.
    bool store;
    size_t str_hash = std::hash<std::string>{}(hc.command_line);
    // Did the command changed since last time?
    if (_cache_hst_cmd[hc.host_id] != str_hash) {
      store = true;
      _cache_hst_cmd[hc.host_id] = str_hash;
    } else
      store = false;

    if (store) {
      int32_t conn = _mysql.choose_connection_by_instance(
          _cache_host_instance[hc.host_id]);

      _host_check_update << hc;
      std::promise<int> promise;
      std::string err_msg(fmt::format(
          "SQL: could not store host check (host: {}): ", hc.host_id));
      _mysql.run_statement(_host_check_update, err_msg, true, conn);
      _add_action(conn, actions::hosts);
    }
  } else
    // Do nothing.
    log_v2::sql()->info(
        "SQL: not processing host check event (host: {}, command: {}, check "
        "type: {}, next check: {}, now: {})",
        hc.host_id, hc.command_line, hc.check_type, hc.next_check, now);
  *std::get<2>(t) = true;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_dependency(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int32_t conn =
      _mysql.choose_best_connection(neb::host_dependency::static_type());
  _finish_action(-1, actions::hosts | actions::host_parents |
                         actions::comments | actions::downtimes |
                         actions::host_dependencies |
                         actions::service_dependencies);

  // Cast object.
  neb::host_dependency const& hd =
      *static_cast<neb::host_dependency const*>(d.get());

  // Insert/Update.
  if (hd.enabled) {
    log_v2::sql()->info("SQL: enabling host dependency of {} on {}",
                        hd.dependent_host_id, hd.host_id);

    // Prepare queries.
    if (!_host_dependency_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("dependent_host_id");
      query_preparator qp(neb::host_dependency::static_type(), unique);
      _host_dependency_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    std::string err_msg(
        fmt::format("SQL: could not store host dependency (host: {}"
                    ", dependent host: {}): ",
                    hd.host_id, hd.dependent_host_id));

    _host_dependency_insupdate << hd;
    _mysql.run_statement(_host_dependency_insupdate, err_msg, true, conn);
    _add_action(conn, actions::host_dependencies);
  }
  // Delete.
  else {
    log_v2::sql()->info("SQL: removing host dependency of {} on {}",
                        hd.dependent_host_id, hd.host_id);
    std::string query(fmt::format(
        "DELETE FROM hosts_hosts_dependencies WHERE dependent_host_id={}"
        " AND host_id={}",
        hd.dependent_host_id, hd.host_id));
    _mysql.run_query(query, "SQL: ", true, conn);
    _add_action(conn, actions::host_dependencies);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_group(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int conn = _mysql.choose_best_connection(neb::host_group::static_type());
  _finish_action(-1, actions::hosts);

  // Cast object.
  neb::host_group const& hg{*static_cast<neb::host_group const*>(d.get())};

  if (hg.enabled) {
    log_v2::sql()->info("SQL: enabling host group {} ('{}' on instance {})",
                        hg.id, hg.name, hg.poller_id);
    _prepare_hg_insupdate_statement();

    std::string err_msg(
        fmt::format("SQL: could not store host group (poller: {}"
                    ", group: {}): ",
                    hg.poller_id, hg.id));

    _host_group_insupdate << hg;
    _mysql.run_statement(_host_group_insupdate, err_msg, true, conn);
    _add_action(conn, actions::hostgroups);
    _hostgroup_cache.insert(hg.id);
  }
  // Delete group.
  else {
    log_v2::sql()->info("SQL: disabling host group {} ('{}' on instance {})",
                        hg.id, hg.name, hg.poller_id);

    // Delete group members.
    {
      std::string query(fmt::format(
          "DELETE hosts_hostgroups FROM hosts_hostgroups LEFT JOIN hosts"
          " ON hosts_hostgroups.host_id=hosts.host_id"
          " WHERE hosts_hostgroups.hostgroup_id={} AND hosts.instance_id={}",
          hg.id, hg.poller_id));
      _mysql.run_query(query, "SQL: ", false, conn);
      _hostgroup_cache.erase(hg.id);
    }
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_group_member(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int conn =
      _mysql.choose_best_connection(neb::host_group_member::static_type());
  _finish_action(-1, actions::hostgroups | actions::hosts);

  // Cast object.
  neb::host_group_member const& hgm(
      *static_cast<neb::host_group_member const*>(d.get()));

  if (hgm.enabled) {
    // Log message.
    log_v2::sql()->info(
        "SQL: enabling membership of host {} to host group {} on instance {}",
        hgm.host_id, hgm.group_id, hgm.poller_id);

    // We only need to try to insert in this table as the
    // host_id/hostgroup_id should be UNIQUE.
    if (!_host_group_member_insert.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("hostgroup_id");
      unique.insert("host_id");
      query_preparator qp(neb::host_group_member::static_type(), unique);
      _host_group_member_insert = qp.prepare_insert(_mysql);
    }

    /* If the group does not exist, we create it. */
    if (_cache_host_instance[hgm.host_id]) {
      if (_hostgroup_cache.find(hgm.group_id) == _hostgroup_cache.end()) {
        logging::error(logging::low)
            << "SQL: host group " << hgm.group_id
            << " does not exist - insertion before insertion of members";
        _prepare_hg_insupdate_statement();

        neb::host_group hg;
        hg.id = hgm.group_id;
        hg.name = hgm.group_name;
        hg.enabled = true;
        hg.poller_id = _cache_host_instance[hgm.host_id];

        std::string err_msg(fmt::format(
            "SQL: could not store host group (poller: {}, group: {}): ",
            hg.poller_id, hg.id));

        _host_group_insupdate << hg;
        _mysql.run_statement(_host_group_insupdate, err_msg, false, conn);
        _add_action(conn, actions::hostgroups);
      }

      _host_group_member_insert << hgm;
      std::string err_msg(
          fmt::format("SQL: could not store host group membership (poller: "
                      "{}, host: {}, group: {}): ",
                      hgm.poller_id, hgm.host_id, hgm.group_id));
      _mysql.run_statement(_host_group_member_insert, err_msg, false, conn);
      _add_action(conn, actions::hostgroups);
    } else
      logging::error(logging::medium)
          << "SQL: host with host_id = " << hgm.host_id
          << " does not exist - unable to store "
             "unexisting host in a hostgroup. You should restart centengine.";
  }
  // Delete.
  else {
    // Log message.
    log_v2::sql()->info(
        "SQL: disabling membership of host {} to host group {} on instance {}",
        hgm.host_id, hgm.group_id, hgm.poller_id);

    if (!_host_group_member_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("hostgroup_id");
      unique.insert("host_id");
      query_preparator qp(neb::host_group_member::static_type(), unique);
      _host_group_member_delete = qp.prepare_delete(_mysql);
    }
    std::string err_msg(
        fmt::format("SQL: cannot delete membership of host {} to host group "
                    "{} on instance {}: ",
                    hgm.host_id, hgm.group_id, hgm.poller_id));

    _host_group_member_delete << hgm;
    _mysql.run_statement(_host_group_member_delete, err_msg, true, conn);
    _add_action(conn, actions::hostgroups);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::instances | actions::hostgroups |
                         actions::host_dependencies | actions::host_parents |
                         actions::custom_variables | actions::downtimes |
                         actions::comments | actions::service_dependencies);
  neb::host& h = *static_cast<neb::host*>(d.get());

  // Log message.
  log_v2::sql()->debug(
      "SQL: processing host event (poller: {}, id: {}, name: {})", h.poller_id,
      h.host_id, h.host_name);

  // Processing
  if (_is_valid_poller(h.poller_id)) {
    // FixMe BAM Generate fake host, this host
    // does not contains a display_name
    // We should not store them in db
    if (h.host_id && !h.alias.empty()) {
      int32_t conn = _mysql.choose_connection_by_instance(h.poller_id);

      // Prepare queries.
      if (!_host_insupdate.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("host_id");
        query_preparator qp(neb::host::static_type(), unique);
        _host_insupdate = qp.prepare_insert_or_update(_mysql);
      }

      // Process object.
      std::string err_msg(fmt::format(
          "SQL: could not store host (poller: {}, host: {}): ", h.poller_id,
          h.host_id));

      _host_insupdate << h;
      _mysql.run_statement(_host_insupdate, err_msg, true, conn);
      _add_action(conn, actions::hosts);

      // Fill the cache...
      if (h.enabled)
        _cache_host_instance[h.host_id] = h.poller_id;
      else
        _cache_host_instance.erase(h.host_id);
    } else
      log_v2::sql()->trace(
          "SQL: host '{0}' of poller {1} has no ID nor alias, probably bam "
          "fake "
          "host",
          h.host_name, h.poller_id);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_parent(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int32_t conn = _mysql.choose_best_connection(neb::host_parent::static_type());
  _finish_action(-1, actions::hosts | actions::host_dependencies |
                         actions::comments | actions::downtimes);

  neb::host_parent const& hp(*static_cast<neb::host_parent const*>(d.get()));

  // Enable parenting.
  if (hp.enabled) {
    // Log message.
    log_v2::sql()->info("SQL: host {} is parent of host {}", hp.parent_id,
                        hp.host_id);

    // Prepare queries.
    if (!_host_parent_insert.prepared()) {
      query_preparator qp(neb::host_parent::static_type());
      _host_parent_insert = qp.prepare_insert(_mysql, true);
    }

    // Insert.
    std::string err_msg(
        fmt::format("SQL: could not store host parentship (child host: {}, "
                    "parent host: {}): ",
                    hp.host_id, hp.parent_id));

    _host_parent_insert << hp;
    _mysql.run_statement(_host_parent_insert, err_msg, false, conn);
    _add_action(conn, actions::host_parents);
  }
  // Disable parenting.
  else {
    log_v2::sql()->info("SQL: host {} is not parent of host {} anymore",
                        hp.parent_id, hp.host_id);

    // Prepare queries.
    if (!_host_parent_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("child_id");
      unique.insert("parent_id");
      query_preparator qp(neb::host_parent::static_type(), unique);
      _host_parent_delete = qp.prepare_delete(_mysql);
    }

    // Delete.
    _host_parent_delete << hp;
    _mysql.run_statement(_host_parent_delete, "SQL: ", false, conn);
    _add_action(conn, actions::host_parents);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_host_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::instances | actions::downtimes |
                         actions::comments | actions::custom_variables |
                         actions::hostgroups | actions::host_dependencies |
                         actions::host_parents);

  // Processed object.
  neb::host_status const& hs(*static_cast<neb::host_status const*>(d.get()));

  time_t now = time(nullptr);
  if (hs.check_type ||                  // - passive result
      !hs.active_checks_enabled ||      // - active checks are disabled,
                                        //   status might not be updated
      hs.next_check >= now - 5 * 60 ||  // - normal case
      !hs.next_check) {                 // - initial state
    // Apply to DB.
    log_v2::sql()->info(
        "processing host status event (id: {}, last check: {}, state ({}, {}))",
        hs.host_id, hs.last_check, hs.current_state, hs.state_type);

    // Prepare queries.
    if (!_host_status_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      query_preparator qp(neb::host_status::static_type(), unique);
      _host_status_update = qp.prepare_update(_mysql);
    }

    // Processing.
    _host_status_update << hs;
    std::string err_msg(fmt::format(
        "SQL: could not store host status (host: {}): ", hs.host_id));
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[hs.host_id]);
    _mysql.run_statement(_host_status_update, err_msg, true, conn);
    _add_action(conn, actions::hosts);
  } else
    // Do nothing.
    log_v2::sql()->info(
        "SQL: not processing host status event (id: {}, check type: {}, last "
        "check: {}, next check: {}, now: {}, state: ({}, {}))",
        hs.host_id, hs.check_type, hs.last_check, hs.next_check, now,
        hs.current_state, hs.state_type);
  *std::get<2>(t) = true;
}

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_instance(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  neb::instance& i(*static_cast<neb::instance*>(d.get()));
  int32_t conn = _mysql.choose_connection_by_instance(i.poller_id);
  _finish_action(-1, actions::hosts | actions::acknowledgements |
                         actions::modules | actions::downtimes |
                         actions::comments | actions::servicegroups |
                         actions::hostgroups | actions::service_dependencies |
                         actions::host_dependencies);

  // Log message.
  log_v2::sql()->info(
      "SQL: processing poller event (id: {}, name: {}, running: {})",
      i.poller_id, i.name, i.is_running ? "yes" : "no");

  // Clean tables.
  _clean_tables(i.poller_id);

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
    std::string err_msg(
        fmt::format("SQL: could not store poller (poller: {}): ", i.poller_id));

    _instance_insupdate << i;
    _mysql.run_statement(_instance_insupdate, err_msg, true, conn);
    _add_action(conn, actions::instances);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process an instance status event. To work on an instance status, we must
 *  be sure the instance already exists in the database. So this query must
 *  be done by the same thread as the one that created the instance.
 *
 *  @param[in] e Uncasted instance status.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_instance_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  neb::instance_status& is = *static_cast<neb::instance_status*>(d.get());
  int32_t conn = _mysql.choose_connection_by_instance(is.poller_id);

  _finish_action(-1, actions::hosts | actions::acknowledgements |
                         actions::modules | actions::downtimes |
                         actions::comments);

  // Log message.
  log_v2::sql()->info(
      "SQL: processing poller status event (id: {}, last alive: {})",
      is.poller_id, is.last_alive);

  // Processing.
  if (_is_valid_poller(is.poller_id)) {
    // Prepare queries.
    if (!_instance_status_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("instance_id");
      query_preparator qp(neb::instance_status::static_type(), unique);
      _instance_status_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    _instance_status_insupdate << is;
    std::string err_msg(fmt::format(
        "SQL: could not update poller (poller: {}): ", is.poller_id));
    _mysql.run_statement(_instance_status_insupdate, err_msg, true, conn);
    _add_action(conn, actions::instances);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_log(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);

  // Fetch proper structure.
  neb::log_entry const& le(*static_cast<neb::log_entry const*>(d.get()));

  // Log message.
  log_v2::sql()->info(
      "SQL: processing log of poller '{}' generated at {} (type {})",
      le.poller_name, le.c_time, le.msg_type);

  // Run query.
  _log_queue.emplace_back(std::make_pair(
      std::get<2>(t),
      fmt::format(
          "({},{},{},'{}','{}',{},{},'{}','{}',{},'{}',{},'{}')", le.c_time,
          le.host_id, le.service_id,
          misc::string::escape(le.host_name, get_logs_col_size(logs_host_name)),
          misc::string::escape(le.poller_name,
                               get_logs_col_size(logs_instance_name)),
          le.log_type, le.msg_type,
          misc::string::escape(le.notification_cmd,
                               get_logs_col_size(logs_notification_cmd)),
          misc::string::escape(le.notification_contact,
                               get_logs_col_size(logs_notification_contact)),
          le.retry,
          misc::string::escape(le.service_description,
                               get_logs_col_size(logs_service_description)),
          le.status,
          misc::string::escape(le.output, get_logs_col_size(logs_output)))));
}

/**
 *  Process a module event. We must take care of the thread id sending the
 *  query because the modules table has a constraint on instances.instance_id
 *
 *  @param[in] e Uncasted module.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_module(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  // Cast object.
  neb::module const& m = *static_cast<neb::module const*>(d.get());
  int32_t conn = _mysql.choose_connection_by_instance(m.poller_id);

  // Log message.
  log_v2::sql()->info(
      "SQL: processing module event (poller: {}, filename: {}, loaded: {})",
      m.poller_id, m.filename, m.loaded ? "yes" : "no");

  // Processing.
  if (_is_valid_poller(m.poller_id)) {
    // Prepare queries.
    if (!_module_insert.prepared()) {
      query_preparator qp(neb::module::static_type());
      _module_insert = qp.prepare_insert(_mysql);
    }

    // Process object.
    if (m.enabled) {
      std::string err_msg(fmt::format(
          "SQL: could not store module (poller: {}): ", m.poller_id));
      _module_insert << m;
      _mysql.run_statement(_module_insert, err_msg, true, conn);
      _add_action(conn, actions::modules);
    } else {
      const std::string* ptr_filename;
      if (m.filename.size() > get_modules_col_size(modules_filename)) {
        std::string trunc_filename = m.filename;
        misc::string::truncate(trunc_filename,
                               get_modules_col_size(modules_filename));
        ptr_filename = &trunc_filename;
      } else
        ptr_filename = &m.filename;

      std::string query(fmt::format(
          "DELETE FROM modules WHERE instance_id={} AND filename='{}'",
          m.poller_id, *ptr_filename));
      _mysql.run_query(query, "SQL: ", false, conn);
      _add_action(conn, actions::modules);
    }
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service_check(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::downtimes | actions::comments |
                         actions::host_dependencies | actions::host_parents |
                         actions::service_dependencies);

  // Cast object.
  neb::service_check const& sc(
      *static_cast<neb::service_check const*>(d.get()));

  time_t now{time(nullptr)};
  if (sc.check_type                 // - passive result
      || !sc.active_checks_enabled  // - active checks are disabled,
                                    //   status might not be updated
                                    // - normal case
      || (sc.next_check >= now - 5 * 60) ||
      !sc.next_check) {  // - initial state
    // Apply to DB.
    log_v2::sql()->info(
        "SQL: processing service check event (host: {}, service: {}, command: "
        "{})",
        sc.host_id, sc.service_id, sc.command_line);

    // Prepare queries.
    if (!_service_check_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_check::static_type(), unique);
      _service_check_update = qp.prepare_update(_mysql);
    }

    // Processing.
    bool store = true;
    size_t str_hash = std::hash<std::string>{}(sc.command_line);
    // Did the command changed since last time?
    if (_cache_svc_cmd[{sc.host_id, sc.service_id}] != str_hash)
      _cache_svc_cmd[std::make_pair(sc.host_id, sc.service_id)] = str_hash;
    else
      store = false;

    if (store) {
      if (_cache_host_instance[sc.host_id]) {
        _service_check_update << sc;
        std::promise<int> promise;
        std::string err_msg(
            fmt::format("SQL: could not store service check command (host: {}, "
                        "service: {}): ",
                        sc.host_id, sc.service_id));
        int32_t conn = _mysql.choose_connection_by_instance(
            _cache_host_instance[sc.host_id]);
        _mysql.run_statement(_service_check_update, err_msg, false, conn);
      } else
        logging::error(logging::medium)
            << "SQL: host with host_id = " << sc.host_id
            << " does not exist - unable to store service command check of "
               "that host. You should restart centengine";
    }
  } else
    // Do nothing.
    log_v2::sql()->info(
        "SQL: not processing service check event (host: {}, service: {}, "
        "command: {}, check_type: {}, next_check: {}, now: {})",
        sc.host_id, sc.service_id, sc.command_line, sc.check_type,
        sc.next_check, now);
  *std::get<2>(t) = true;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service_dependency(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int32_t conn =
      _mysql.choose_best_connection(neb::service_dependency::static_type());
  _finish_action(-1, actions::hosts | actions::host_parents |
                         actions::downtimes | actions::comments |
                         actions::host_dependencies |
                         actions::service_dependencies);

  // Cast object.
  neb::service_dependency const& sd(
      *static_cast<neb::service_dependency const*>(d.get()));

  // Insert/Update.
  if (sd.enabled) {
    log_v2::sql()->info(
        "SQL: enabling service dependency of ({}, {}) on ({}, {})",
        sd.dependent_host_id, sd.dependent_service_id, sd.host_id,
        sd.service_id);

    // Prepare queries.
    if (!_service_dependency_insupdate.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("dependent_host_id");
      unique.insert("dependent_service_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_dependency::static_type(), unique);
      _service_dependency_insupdate = qp.prepare_insert_or_update(_mysql);
    }

    // Process object.
    std::string err_msg(fmt::format(
        "SQL: could not store service dependency (host: {}, service: {}, "
        "dependent host: {}, dependent service: {}): ",
        sd.host_id, sd.service_id, sd.dependent_host_id,
        sd.dependent_service_id));

    _service_dependency_insupdate << sd;
    _mysql.run_statement(_service_dependency_insupdate, err_msg, true, conn);
    _add_action(conn, actions::service_dependencies);
  }
  // Delete.
  else {
    log_v2::sql()->info(
        "SQL: removing service dependency of ({}, {}) on ({}, {})",
        sd.dependent_host_id, sd.dependent_service_id, sd.host_id,
        sd.service_id);
    std::string query(fmt::format(
        "DELETE FROM serivces_services_dependencies WHERE dependent_host_id={} "
        "AND dependent_service_id={} AND host_id={} AND service_id={}",
        sd.dependent_host_id, sd.dependent_service_id, sd.host_id,
        sd.service_id));
    _mysql.run_query(query, "SQL: ", false, conn);
    _add_action(conn, actions::service_dependencies);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service_group(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int32_t conn =
      _mysql.choose_best_connection(neb::service_group::static_type());
  _finish_action(-1, actions::hosts | actions::services);

  // Cast object.
  neb::service_group const& sg(
      *static_cast<neb::service_group const*>(d.get()));

  // Insert/update group.
  if (sg.enabled) {
    log_v2::sql()->info("SQL: enabling service group {} ('{}' on instance {})",
                        sg.id, sg.name, sg.poller_id);
    _prepare_sg_insupdate_statement();

    std::string err_msg(fmt::format(
        "SQL: could not store service group (poller: {}, group: {}): ",
        sg.poller_id, sg.id));

    _service_group_insupdate << sg;
    _mysql.run_statement(_service_group_insupdate, err_msg, true, conn);
    _add_action(conn, actions::servicegroups);
    _servicegroup_cache.insert(sg.id);
  }
  // Delete group.
  else {
    log_v2::sql()->info("SQL: disabling service group {} ('{}' on instance {})",
                        sg.id, sg.name, sg.poller_id);

    // Delete group members.
    {
      std::string query(fmt::format(
          "DELETE services_servicegroups FROM services_servicegroups LEFT "
          "JOIN hosts ON services_servicegroups.host_id=hosts.host_id WHERE "
          "services_servicegroups.servicegroup_id={} AND "
          "hosts.instance_id={}",
          sg.id, sg.poller_id));
      _mysql.run_query(query, "SQL: ", false, conn);
      _add_action(conn, actions::servicegroups);
      _servicegroup_cache.erase(sg.id);
    }
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service_group_member(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  int32_t conn =
      _mysql.choose_best_connection(neb::service_group_member::static_type());
  _finish_action(-1,
                 actions::hosts | actions::servicegroups | actions::services);

  // Cast object.
  neb::service_group_member const& sgm(
      *static_cast<neb::service_group_member const*>(d.get()));

  if (sgm.enabled) {
    // Log message.
    log_v2::sql()->info(
        "SQL: enabling membership of service ({}, {}) to service group {} on "
        "instance {}",
        sgm.host_id, sgm.service_id, sgm.group_id, sgm.poller_id);

    // We only need to try to insert in this table as the
    // host_id/service_id/servicegroup_id combo should be UNIQUE.
    if (!_service_group_member_insert.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("servicegroup_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_group_member::static_type(), unique);
      _service_group_member_insert = qp.prepare_insert(_mysql);
    }

    /* If the group does not exist, we create it. */
    if (_servicegroup_cache.find(sgm.group_id) == _servicegroup_cache.end()) {
      logging::error(logging::low)
          << "SQL: service group " << sgm.group_id
          << " does not exist - insertion before insertion of members";
      _prepare_sg_insupdate_statement();

      neb::service_group sg;
      sg.id = sgm.group_id;
      sg.name = sgm.group_name;
      sg.enabled = true;
      sg.poller_id = sgm.poller_id;

      std::string err_msg(fmt::format(
          "SQL: could not store service group (poller: {}, group: {}): ",
          sg.poller_id, sg.id));

      _service_group_insupdate << sg;
      _mysql.run_statement(_service_group_insupdate, err_msg, false, conn);
      _add_action(conn, actions::servicegroups);
    }

    _service_group_member_insert << sgm;
    std::string err_msg(fmt::format(
        "SQL: could not store service group membership (poller: {}, host: "
        "{}, service: {}, group: {}): ",
        sgm.poller_id, sgm.host_id, sgm.service_id, sgm.group_id));
    _mysql.run_statement(_service_group_member_insert, err_msg, false, conn);
    _add_action(conn, actions::servicegroups);
  }
  // Delete.
  else {
    // Log message.
    log_v2::sql()->info(
        "SQL: disabling membership of service ({}, {}) to service group {} on "
        "instance {}",
        sgm.host_id, sgm.service_id, sgm.group_id, sgm.poller_id);

    if (!_service_group_member_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("servicegroup_id");
      unique.insert("host_id");
      unique.insert("service_id");
      query_preparator qp(neb::service_group_member::static_type(), unique);
      _service_group_member_delete = qp.prepare_delete(_mysql);
    }
    std::string err_msg(fmt::format(
        "SQL: cannot delete membership of service (host: {}, service: {}) to "
        "service group {} on instance {}: ",
        sgm.host_id, sgm.service_id, sgm.group_id, sgm.poller_id));

    _service_group_member_delete << sgm;
    _mysql.run_statement(_service_group_member_delete, err_msg, false, conn);
    _add_action(conn, actions::servicegroups);
  }
  *std::get<2>(t) = true;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::host_parents | actions::comments |
                         actions::downtimes | actions::host_dependencies |
                         actions::service_dependencies);

  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(d.get()));
  if (_cache_host_instance[s.host_id]) {
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]);

    // Log message.
    log_v2::sql()->info(
        "SQL: processing service event (host id: {}, service id: {}, "
        "description: {})",
        s.host_id, s.service_id, s.service_description);

    // Processing.
    // FixMe BAM Generate fake services, this service
    // does not contains a display_name
    // We should not store them in db
    if (s.host_id && s.service_id && !s.host_name.empty()) {
      // Prepare queries.
      if (!_service_insupdate.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("host_id");
        unique.insert("service_id");
        query_preparator qp(neb::service::static_type(), unique);
        _service_insupdate = qp.prepare_insert_or_update(_mysql);
      }

      std::string err_msg(fmt::format(
          "SQL: could not store service (host: {}, service: {}): ", s.host_id,
          s.service_id));

      _service_insupdate << s;
      _mysql.run_statement(_service_insupdate, err_msg, true, conn);
      _add_action(conn, actions::services);
    } else
      log_v2::sql()->trace(
          "SQL: service '{}' has no host ID, service ID nor hostname, probably "
          "bam fake service",
          s.service_description);
  } else
    logging::error(logging::medium)
        << "SQL: host with host_id = " << s.host_id
        << " does not exist - unable to store "
           "service of that host. You should restart centengine";
  *std::get<2>(t) = true;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_service_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  _finish_action(-1, actions::host_parents | actions::comments |
                         actions::downtimes | actions::host_dependencies |
                         actions::service_dependencies);
  // Processed object.
  neb::service_status const& ss{
      *static_cast<neb::service_status const*>(d.get())};

  log_v2::perfdata()->info("SQL: service status output: <<{}>>", ss.output);
  log_v2::perfdata()->info("SQL: service status perfdata: <<{}>>",
                           ss.perf_data);

  time_t now = time(nullptr);
  if (ss.check_type ||           // - passive result
      !ss.active_checks_enabled  // - active checks are disabled,
                                 //   status might not be updated
      ||                         // - normal case
      ss.next_check >= now - 5 * 60 || !ss.next_check) {  // - initial state
    // Apply to DB.
    log_v2::sql()->info(
        "SQL: processing service status event (host: {}, service: {}, last "
        "check: {}, state ({}, {}))",
        ss.host_id, ss.service_id, ss.last_check, ss.current_state,
        ss.state_type);

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
    std::string err_msg(fmt::format(
        "SQL: could not store service status (host: {}, service: {}) ",
        ss.host_id, ss.service_id));
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);
    _mysql.run_statement(_service_status_update, err_msg, false, conn);
    _add_action(conn, actions::hosts);
  } else
    // Do nothing.
    log_v2::sql()->info(
        "SQL: not processing service status event (host: {}, service: {}, "
        "check type: {}, last check: {}, next check: {}, now: {}, state ({}, "
        "{}))",
        ss.host_id, ss.service_id, ss.check_type, ss.last_check, ss.next_check,
        now, ss.current_state, ss.state_type);
  *std::get<2>(t) = true;
}

/**
 *  Process an instance configuration event.
 *
 *  @param[in] e  Uncasted instance configuration.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_instance_configuration(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  *std::get<2>(t) = true;
}

/**
 *  Process a responsive instance event.
 *
 * @return The number of events that can be acknowledged.
 */
void conflict_manager::_process_responsive_instance(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  *std::get<2>(t) = true;
}

/**
 * @brief Send a big query to update/insert a bulk of custom variables. When
 * the query is done, we set the corresponding boolean of each pair to true
 * to ack each event.
 *
 * When we exit the function, the custom variables queue is empty.
 */
void conflict_manager::_update_customvariables() {
  if (_cv_queue.empty())
    return;
  int conn = _mysql.choose_best_connection(neb::custom_variable::static_type());
  _finish_action(-1, actions::custom_variables);
  auto it = _cv_queue.begin();
  std::ostringstream oss;
  oss << "INSERT INTO customvariables "
         "(name,host_id,service_id,default_value,modified,type,update_time,"
         "value) VALUES "
      << std::get<1>(*it);
  *std::get<0>(*it) = true;
  for (++it; it != _cv_queue.end(); ++it)
    oss << "," << std::get<1>(*it);

  /* Building of the query */
  oss << " ON DUPLICATE KEY UPDATE "
         "default_value=VALUES(default_VALUE),modified=VALUES(modified),type="
         "VALUES(type),update_time=VALUES(update_time),value=VALUES(value)";
  std::string query(oss.str());
  _mysql.run_query(query, "SQL: could not store custom variables correctly",
                   true, conn);
  log_v2::sql()->debug("{} new custom variables inserted", _cv_queue.size());
  log_v2::sql()->trace("sending query << {} >>", query);
  _add_action(conn, actions::custom_variables);

  /* Acknowledgement and cleanup */
  while (!_cv_queue.empty()) {
    auto it = _cv_queue.begin();
    *std::get<0>(*it) = true;
    _cv_queue.pop_front();
  }
}

/**
 * @brief Send a big query to insert a bulk of logs. When the query is done,
 * we set the corresponding boolean of each pair to true to ack each event.
 *
 * When we exit the function, the logs queue is empty.
 */
void conflict_manager::_insert_logs() {
  if (_log_queue.empty())
    return;
  int conn = _mysql.choose_best_connection(neb::log_entry::static_type());
  auto it = _log_queue.begin();
  std::ostringstream oss;

  /* Building of the query */
  oss << "INSERT INTO logs "
         "(ctime,host_id,service_id,host_name,instance_name,type,msg_type,"
         "notification_cmd,notification_contact,retry,service_description,"
         "status,output) VALUES "
      << std::get<1>(*it);
  *std::get<0>(*it) = true;
  for (++it; it != _log_queue.end(); ++it)
    oss << "," << std::get<1>(*it);

  std::string query(oss.str());
  _mysql.run_query(query, "SQL: could not store logs correctly", true, conn);
  log_v2::sql()->debug("{} new logs inserted", _log_queue.size());
  log_v2::sql()->trace("sending query << {} >>", query);

  /* Acknowledgement and cleanup */
  while (!_log_queue.empty()) {
    auto it = _log_queue.begin();
    *std::get<0>(*it) = true;
    _log_queue.pop_front();
  }
}
