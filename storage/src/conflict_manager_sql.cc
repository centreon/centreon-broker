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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/broker/storage/table_max_size.hh"
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
  logging::debug(logging::low)
      << "conflict_manager: disable hosts and services (instance_id: "
      << instance_id << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove host group memberships (instance_id:"
      << instance_id << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove service group memberships (instance_id:"
      << instance_id << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove empty host groups (instance_id:"
      << instance_id << ")";
  _mysql.run_query(
      "DELETE hg FROM hostgroups AS hg LEFT JOIN hosts_hostgroups AS hhg ON "
      "hg.hostgroup_id=hhg.hostgroup_id WHERE hhg.hostgroup_id IS NULL",
      "conflict_manager: could not remove empty host groups", false, conn);
  _add_action(conn, actions::hostgroups);

  /* Remove service groups. */
  logging::debug(logging::low)
      << "conflict_manager: remove empty service groups (instance_id:"
      << instance_id << ")";

  _mysql.run_query(
      "DELETE sg FROM servicegroups AS sg LEFT JOIN services_servicegroups as "
      "ssg ON sg.servicegroup_id=ssg.servicegroup_id WHERE ssg.servicegroup_id "
      "IS NULL",
      "conflict_manager: could not remove empty service groups", false, conn);
  _add_action(conn, actions::servicegroups);

  /* Remove host dependencies. */
  logging::debug(logging::low)
      << "conflict_manager: remove host dependencies (instance_id:"
      << instance_id << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove host parents (instance_id:" << instance_id
      << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove service dependencies (instance_id:"
      << instance_id << ")";
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
  logging::debug(logging::low)
      << "SQL: remove list of modules (instance_id:" << instance_id << ")";
  query = fmt::format("DELETE FROM modules WHERE instance_id={}", instance_id);
  _mysql.run_query(
      query, "conflict_manager: could not clean modules table: ", false, conn);
  _add_action(conn, actions::modules);

  // Cancellation of downtimes.
  logging::debug(logging::low)
      << "SQL: Cancellation of downtimes (instance_id:" << instance_id << ")";
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
  logging::debug(logging::low)
      << "conflict_manager: remove comments (instance_id:" << instance_id
      << ")";
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

  _mysql.run_query(query,
                   "conflict_manager: could not clean custom variables table: ",
                   false, conn);
  _add_action(conn, actions::custom_variables);
}

/**
 *  Update all the hosts and services of unresponsive instances.
 */
void conflict_manager::_update_hosts_and_services_of_unresponsive_instances() {
  logging::debug(logging::medium)
      << "conflict_manager: checking for outdated instances";

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
    logging::info(logging::low) << "conflict_manager: discarding some event "
                                   "related to a deleted poller ("
                                << instance_id << ")";
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
int32_t conflict_manager::_process_acknowledgement() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

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

    if (ack.author.size() >
            get_acknowledgements_col_size(acknowledgements_author) ||
        ack.comment.size() >
            get_acknowledgements_col_size(acknowledgements_comment_data)) {
      neb::acknowledgement trunc_ack(ack);
      if (trunc_ack.author.size() >
          get_acknowledgements_col_size(acknowledgements_author)) {
        log_v2::sql()->warn(
            "acknowledgements author ({} instead of {}) is too long to be "
            "stored in database.",
            trunc_ack.author.size(),
            get_acknowledgements_col_size(acknowledgements_author));
        trunc_ack.author.resize(
            get_acknowledgements_col_size(acknowledgements_author));
      }
      if (trunc_ack.comment.size() >
          get_acknowledgements_col_size(acknowledgements_comment_data)) {
        log_v2::sql()->warn(
            "acknowledgements comment data ({} instead of {}) is too long to "
            "be stored in database.",
            trunc_ack.comment.size(),
            get_acknowledgements_col_size(acknowledgements_comment_data));
        trunc_ack.comment.resize(
            get_acknowledgements_col_size(acknowledgements_comment_data));
      }
      _acknowledgement_insupdate << trunc_ack;
    } else
      _acknowledgement_insupdate << ack;
    _mysql.run_statement(_acknowledgement_insupdate, msg_error, true, conn);
  }
  _pop_event(p);
  return 1;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e  Uncasted comment.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_comment() {
  _finish_action(-1, actions::hosts | actions::instances |
                         actions::host_parents | actions::host_dependencies |
                         actions::service_dependencies);
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Cast object.
  neb::comment const& cmmnt{*static_cast<neb::comment const*>(d.get())};

  int32_t conn = _mysql.choose_connection_by_instance(cmmnt.poller_id);

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

  if (cmmnt.author.size() > get_comments_col_size(comments_author) ||
      cmmnt.data.size() > get_comments_col_size(comments_data)) {
    neb::comment trunc_cmnt(cmmnt);
    if (trunc_cmnt.author.size() > get_comments_col_size(comments_author)) {
        log_v2::sql()->warn(
            "comments author ({} instead of {}) is too long to "
            "be stored in database.",
            trunc_cmnt.author.size(),
            get_comments_col_size(comments_author));
      trunc_cmnt.author.resize(get_comments_col_size(comments_author));
    }
    if (trunc_cmnt.data.size() > get_comments_col_size(comments_data)) {
      log_v2::sql()->warn(
            "comments data ({} instead of {}) is too long to "
            "be stored in database.",
            trunc_cmnt.data.size(),
            get_comments_col_size(comments_data));
      trunc_cmnt.data.resize(get_comments_col_size(comments_data));
    }
    _comment_insupdate << trunc_cmnt;
  } else
    _comment_insupdate << cmmnt;

  _comment_insupdate << cmmnt;
  _mysql.run_statement(_comment_insupdate, err_msg, true, conn);
  _pop_event(p);
  return 1;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_custom_variable() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::custom_variables);
  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::custom_variable::static_type())
      break;

    // Cast object.
    neb::custom_variable const& cv{
        *static_cast<neb::custom_variable const*>(d.get())};

    // Prepare queries.
    if (!_custom_variable_insupdate.prepared() ||
        !_custom_variable_delete.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      unique.insert("name");
      unique.insert("service_id");
      query_preparator qp(neb::custom_variable::static_type(), unique);
      _custom_variable_insupdate = qp.prepare_insert_or_update(_mysql);
      _custom_variable_delete = qp.prepare_delete(_mysql);
    }

    // Processing.
    if (cv.enabled) {
      logging::info(logging::medium)
          << "SQL: enabling custom variable '" << cv.name << "' of ("
          << cv.host_id << ", " << cv.service_id << ")";
      std::string err_msg(
          fmt::format("SQL: could not store custom variable (name: {}"
                      ", host: {}, service: {}): ",
                      cv.name, cv.host_id, cv.service_id));
      ;

      if (cv.default_value.size() >
              get_customvariables_col_size(customvariables_default_value) ||
          cv.value.size() >
              get_customvariables_col_size(customvariables_value) ||
          cv.name.size() > get_customvariables_col_size(customvariables_name)) {
        neb::custom_variable trunc_cv(cv);
        if (trunc_cv.default_value.size() >
            get_customvariables_col_size(customvariables_default_value)) {
          log_v2::sql()->warn(
              "customvariables default value ({} instead of {}) is too long to "
              "be stored in database.",
              trunc_cv.default_value.size(),
              get_customvariables_col_size(customvariables_default_value));
          trunc_cv.default_value.resize(
              get_customvariables_col_size(customvariables_default_value));
        }
        if (trunc_cv.value.size() >
            get_customvariables_col_size(customvariables_value)) {
          log_v2::sql()->warn(
              "customvariables value ({} instead of {}) is too long to "
              "be stored in database.",
              trunc_cv.value.size(),
              get_customvariables_col_size(customvariables_value));
          trunc_cv.value.resize(
              get_customvariables_col_size(customvariables_value));
        }
        if (trunc_cv.name.size() >
            get_customvariables_col_size(customvariables_name)) {
          log_v2::sql()->warn(
              "customvariables name ({} instead of {}) is too long to "
              "be stored in database.",
              trunc_cv.name.size(),
              get_customvariables_col_size(customvariables_name));
          trunc_cv.name.resize(
              get_customvariables_col_size(customvariables_name));
        }
        _custom_variable_insupdate << trunc_cv;
      } else
        _custom_variable_insupdate << cv;

      _mysql.run_statement(_custom_variable_insupdate, err_msg, true, conn);
      _add_action(conn, actions::custom_variables);
    } else {
      logging::info(logging::medium)
          << "SQL: disabling custom variable '" << cv.name << "' of ("
          << cv.host_id << ", " << cv.service_id << ")";
      _custom_variable_delete.bind_value_as_i32(":host_id", cv.host_id);
      _custom_variable_delete.bind_value_as_i32(":service_id", cv.service_id);
      _custom_variable_delete.bind_value_as_str(":name", cv.name);

      std::string err_msg(
          fmt::format("SQL: could not remove custom variable (host: {}"
                      ", service: {}, name '{}'): ",
                      cv.host_id, cv.service_id, cv.name));
      _mysql.run_statement(_custom_variable_delete, err_msg, true, conn);
      _add_action(conn, actions::custom_variables);
    }
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_custom_variable_status() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::custom_variables);
  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::custom_variable_status::static_type())
      break;

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

    logging::info(logging::medium)
        << "SQL: enabling custom variable '" << cv.name << "' of ("
        << cv.host_id << ", " << cv.service_id << ")";

    std::string err_msg(
        fmt::format("SQL: could not store custom variable (name: {}"
                    ", host: {}, service: {}): ",
                    cv.name, cv.host_id, cv.service_id));

    if (cv.value.size() > get_customvariables_col_size(customvariables_value) ||
        cv.name.size() > get_customvariables_col_size(customvariables_name)) {
      neb::custom_variable_status trunc_cv(cv);
      if (trunc_cv.value.size() >
          get_customvariables_col_size(customvariables_value)) {
        log_v2::sql()->warn(
            "customvariables value ({} instead of {}) is too long to "
            "be stored in database.",
            trunc_cv.value.size(),
            get_customvariables_col_size(customvariables_value));
        trunc_cv.value.resize(
            get_customvariables_col_size(customvariables_value));
      }
      if (trunc_cv.name.size() >
          get_customvariables_col_size(customvariables_name)) {
        log_v2::sql()->warn(
            "customvariables name ({} instead of {}) is too long to "
            "be stored in database.",
            trunc_cv.name.size(),
            get_customvariables_col_size(customvariables_name));
        trunc_cv.name.resize(
            get_customvariables_col_size(customvariables_name));
      }
      _custom_variable_status_insupdate << trunc_cv;
    } else
      _custom_variable_status_insupdate << cv;

    _mysql.run_statement(_custom_variable_status_insupdate, err_msg, true,
                         conn);
    _add_action(conn, actions::custom_variables);
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_downtime() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::instances |
                         actions::host_parents | actions::host_dependencies |
                         actions::service_dependencies);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::downtime::static_type())
      break;

    // Cast object.
    neb::downtime const& dd = *static_cast<neb::downtime const*>(d.get());

    // Log message.
    logging::info(logging::medium)
        << "SQL: processing downtime event (poller: " << dd.poller_id
        << ", host: " << dd.host_id << ", service: " << dd.service_id
        << ", start time: " << dd.start_time << ", end_time: " << dd.end_time
        << ", actual start time: " << dd.actual_start_time
        << ", actual end time: " << dd.actual_end_time
        << ", duration: " << dd.duration << ", entry time: " << dd.entry_time
        << ", deletion time: " << dd.deletion_time << ")";

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

      if (dd.author.size() > get_downtimes_col_size(downtimes_author) ||
          dd.comment.size() > get_downtimes_col_size(downtimes_comment_data)) {
        neb::downtime trunc_dd(dd);
        if (trunc_dd.author.size() > get_downtimes_col_size(downtimes_author)) {
          log_v2::sql()->warn(
              "downtimes author ({} instead of {}) is too long to "
              "be stored in database.",
              trunc_dd.author.size(),
              get_downtimes_col_size(downtimes_author));
          trunc_dd.author.resize(get_downtimes_col_size(downtimes_author));
        }
        if (trunc_dd.comment.size() > get_downtimes_col_size(downtimes_comment_data)) {
          log_v2::sql()->warn(
              "downtimes comment data ({} instead of {}) is too long to "
              "be stored in database.",
              trunc_dd.comment.size(),
              get_downtimes_col_size(downtimes_comment_data));
          trunc_dd.comment.resize(get_downtimes_col_size(downtimes_comment_data));
        }
        _downtime_insupdate << trunc_dd;
      }
      else
        _downtime_insupdate << dd;
      _mysql.run_statement(_downtime_insupdate, err_msg, true, conn);
      _add_action(conn, actions::downtimes);
    }
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_event_handler() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
  // Cast object.
  neb::event_handler const& eh =
      *static_cast<neb::event_handler const*>(d.get());

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
  std::string err_msg(
      fmt::format("SQL: could not store event handler (host: {}"
                  ", service: {}, start time: {}): ",
                  eh.host_id, eh.service_id, eh.start_time));

  if (eh.command_args.size() > get_eventhandlers_col_size(eventhandlers_command_args) ||
      eh.command_line.size() > get_eventhandlers_col_size(eventhandlers_command_line) ||
      eh.output.size() > get_eventhandlers_col_size(eventhandlers_output)) {
    neb::event_handler trunc_eh(eh);
    if (eh.command_args.size() > get_eventhandlers_col_size(eventhandlers_command_args)) {
      log_v2::sql()->warn(
          "event handler command_args ({} instead of {}) is too long to be "
          "stored in database.",
          eh.command_args.size(),
          get_eventhandlers_col_size(eventhandlers_command_args));
      trunc_eh.command_args.resize(get_eventhandlers_col_size(eventhandlers_command_args));
    }
    if (eh.command_line.size() > get_eventhandlers_col_size(eventhandlers_command_line)) {
      log_v2::sql()->warn(
          "event handler command_line ({} instead of {}) is too long to be "
          "stored in database.",
          eh.command_line.size(),
          get_eventhandlers_col_size(eventhandlers_command_line));
      trunc_eh.command_line.resize(get_eventhandlers_col_size(eventhandlers_command_line));
    }
    if (eh.output.size() > get_eventhandlers_col_size(eventhandlers_output)) {
      log_v2::sql()->warn(
          "event handler output ({} instead of {}) is too long to be "
          "stored in database.",
          eh.output.size(),
          get_eventhandlers_col_size(eventhandlers_output));
      trunc_eh.output.resize(get_eventhandlers_col_size(eventhandlers_output));
    }
    _event_handler_insupdate << trunc_eh;
  }
  else
    _event_handler_insupdate << eh;
  _mysql.run_statement(
      _event_handler_insupdate, err_msg, true,
      _mysql.choose_connection_by_instance(_cache_host_instance[eh.host_id]));
  _pop_event(p);
  return 1;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_flapping_status() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
  // Cast object.
  neb::flapping_status const& fs(
      *static_cast<neb::flapping_status const*>(d.get()));

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
  _pop_event(p);
  return 1;
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_check() {
  int32_t retval = 0;
  _finish_action(-1, actions::instances | actions::downtimes |
                         actions::comments | actions::host_dependencies |
                         actions::host_parents | actions::service_dependencies);
  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_check::static_type())
      break;

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

        if (hc.command_line.size() > get_hosts_col_size(hosts_command_line)) {
          neb::host_check trunc_hc(hc);
          log_v2::sql()->warn(
              "hosts command_line ({} instead of {}) is too long to be "
              "stored in database.",
              hc.command_line.size(), get_hosts_col_size(hosts_command_line));
          trunc_hc.command_line.resize(get_hosts_col_size(hosts_command_line));
          _host_check_update << trunc_hc;
        } else
          _host_check_update << hc;

        std::promise<int> promise;
        std::string err_msg(fmt::format(
            "SQL: could not store host check (host: {}): ", hc.host_id));
        _mysql.run_statement(_host_check_update, err_msg, true, conn);
        _add_action(conn, actions::hosts);
      }
    } else
      // Do nothing.
      logging::info(logging::medium)
          << "SQL: not processing host check event (host: " << hc.host_id
          << ", command: " << hc.command_line
          << ", check type: " << hc.check_type
          << ", next check: " << hc.next_check << ", now: " << now << ")";
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_dependency() {
  int32_t retval = 0;
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::host_parents |
                         actions::comments | actions::downtimes |
                         actions::host_dependencies |
                         actions::service_dependencies);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_dependency::static_type())
      break;

    // Cast object.
    neb::host_dependency const& hd =
        *static_cast<neb::host_dependency const*>(d.get());

    // Insert/Update.
    if (hd.enabled) {
      logging::info(logging::medium)
          << "SQL: enabling host dependency of " << hd.dependent_host_id
          << " on " << hd.host_id;

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

      if (hd.dependency_period.size() >
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_dependency_period) ||
          hd.execution_failure_options.size() >
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_execution_failure_options) ||
          hd.notification_failure_options.size() >
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_notification_failure_options)) {
        neb::host_dependency trunc_hd(hd);
        if (hd.dependency_period.size() >
            get_hosts_hosts_dependencies_col_size(
                hosts_hosts_dependencies_dependency_period)) {
          log_v2::sql()->warn(
              "hosts_hosts_dependencies dependency period ({} instead of {}) "
              "is too long to be stored in database.",
              hd.dependency_period.size(),
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_dependency_period));
          trunc_hd.dependency_period.resize(
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_dependency_period));
        }
        if (hd.execution_failure_options.size() >
            get_hosts_hosts_dependencies_col_size(
                hosts_hosts_dependencies_execution_failure_options)) {
          log_v2::sql()->warn(
              "hosts_hosts_dependencies execution failure options ({} instead "
              "of {}) is too long to be stored in database.",
              hd.execution_failure_options.size(),
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_execution_failure_options));
          trunc_hd.execution_failure_options.resize(
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_execution_failure_options));
        }
        if (hd.notification_failure_options.size() >
            get_hosts_hosts_dependencies_col_size(
                hosts_hosts_dependencies_notification_failure_options)) {
          log_v2::sql()->warn(
              "hosts_hosts_dependencies notification failure options ({} "
              "instead of {}) is too long to be stored in database.",
              hd.notification_failure_options.size(),
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_notification_failure_options));
          trunc_hd.notification_failure_options.resize(
              get_hosts_hosts_dependencies_col_size(
                  hosts_hosts_dependencies_notification_failure_options));
        }
        _host_dependency_insupdate << trunc_hd;
      } else
        _host_dependency_insupdate << hd;
      _mysql.run_statement(_host_dependency_insupdate, err_msg, true, conn);
      _add_action(conn, actions::host_dependencies);
    }
    // Delete.
    else {
      logging::info(logging::medium)
          << "SQL: removing host dependency of " << hd.dependent_host_id
          << " on " << hd.host_id;
      std::string query(fmt::format(
          "DELETE FROM hosts_hosts_dependencies WHERE dependent_host_id={}"
          " AND host_id={}",
          hd.dependent_host_id, hd.host_id));
      _mysql.run_query(query, "SQL: ", true, conn);
      _add_action(conn, actions::host_dependencies);
    }
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_group() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_group::static_type())
      break;

    // Cast object.
    neb::host_group const& hg{*static_cast<neb::host_group const*>(d.get())};

    if (hg.enabled) {
      logging::info(logging::medium)
          << "SQL: enabling host group " << hg.id << " ('" << hg.name
          << "') on instance " << hg.poller_id;
      _prepare_hg_insupdate_statement();

      std::string err_msg(
          fmt::format("SQL: could not store host group (poller: {}"
                      ", group: {}): ",
                      hg.poller_id, hg.id));

      if (hg.name.size() > get_hostgroups_col_size(hostgroups_name)) {
        neb::host_group trunc_hg(hg);
        log_v2::sql()->warn(
            "hostgroups name ({} instead of {}) is too long to be "
            "stored in database.",
            hg.name.size(), get_hostgroups_col_size(hostgroups_name));
        trunc_hg.name.resize(get_hostgroups_col_size(hostgroups_name));
        _host_group_insupdate << trunc_hg;
      } else
        _host_group_insupdate << hg;
      _mysql.run_statement(_host_group_insupdate, err_msg, true, conn);
      _add_action(conn, actions::hostgroups);
      _hostgroup_cache.insert(hg.id);
    }
    // Delete group.
    else {
      logging::info(logging::medium)
          << "SQL: disabling host group " << hg.id << " ('" << hg.name
          << "' on instance " << hg.poller_id;

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
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_group_member() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hostgroups | actions::hosts);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_group_member::static_type())
      break;

    // Cast object.
    neb::host_group_member const& hgm(
        *static_cast<neb::host_group_member const*>(d.get()));

    if (hgm.enabled) {
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

          if (hg.name.size() > get_hostgroups_col_size(hostgroups_name)) {
            neb::host_group trunc_hg(hg);
            log_v2::sql()->warn(
                "hostgroups name ({} instead of {}) is too long to be "
                "stored in database.",
                hg.name.size(), get_hostgroups_col_size(hostgroups_name));
            trunc_hg.name.resize(get_hostgroups_col_size(hostgroups_name));
            _host_group_insupdate << trunc_hg;
          } else
            _host_group_insupdate << hg;

          _mysql.run_statement(_host_group_insupdate, err_msg, false, conn);
          _add_action(conn, actions::hostgroups);
        }

        if (hgm.group_name.size() > get_hostgroups_col_size(hostgroups_name)) {
          neb::host_group_member trunc_hgm(hgm);
          log_v2::sql()->warn(
              "hostgroups name ({} instead of {}) is too long to be "
              "stored in database.",
              hgm.group_name.size(), get_hostgroups_col_size(hostgroups_name));
          trunc_hgm.group_name.resize(get_hostgroups_col_size(hostgroups_name));
          _host_group_member_insert << trunc_hgm;
        } else
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
      logging::info(logging::medium)
          << "SQL: disabling membership of host " << hgm.host_id
          << " to host group " << hgm.group_id << " on instance "
          << hgm.poller_id;

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
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host() {
  _finish_action(-1, actions::instances | actions::hostgroups |
                         actions::host_dependencies | actions::host_parents |
                         actions::custom_variables | actions::downtimes |
                         actions::comments | actions::service_dependencies);
  auto& p = _events.front();
  neb::host& h = *static_cast<neb::host*>(std::get<0>(p).get());

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

      if (h.host_name.size() > get_hosts_col_size(hosts_name) ||
          h.action_url.size() > get_hosts_col_size(hosts_action_url) ||
          h.address.size() > get_hosts_col_size(hosts_address) ||
          h.alias.size() > get_hosts_col_size(hosts_alias) ||
          h.check_command.size() > get_hosts_col_size(hosts_check_command) ||
          h.check_period.size() > get_hosts_col_size(hosts_check_period) ||
          h.display_name.size() > get_hosts_col_size(hosts_display_name) ||
          h.event_handler.size() > get_hosts_col_size(hosts_event_handler) ||
          h.icon_image.size() > get_hosts_col_size(hosts_icon_image) ||
          h.icon_image_alt.size() > get_hosts_col_size(hosts_icon_image_alt) ||
          h.notes.size() > get_hosts_col_size(hosts_notes) ||
          h.notes_url.size() > get_hosts_col_size(hosts_notes_url) ||
          h.notification_period.size() > get_hosts_col_size(hosts_notification_period) ||
          h.output.size() > get_hosts_col_size(hosts_output) ||
          h.perf_data.size() > get_hosts_col_size(hosts_perfdata) ||
          h.statusmap_image.size() > get_hosts_col_size(hosts_statusmap_image) ||
          h.timezone.size() > get_hosts_col_size(hosts_timezone)) {
        neb::host trunc_h(h);
        if (h.host_name.size() > get_hosts_col_size(hosts_name)) {
          log_v2::sql()->warn(
              "hosts name ({} instead of {}) is too long to be stored in "
              "database.",
              h.host_name.size(), get_hosts_col_size(hosts_name));
          trunc_h.host_name.resize(get_hosts_col_size(hosts_name));
        }
        if (h.action_url.size() > get_hosts_col_size(hosts_action_url)) {
          log_v2::sql()->warn(
              "hosts action_url ({} instead of {}) is too long to be stored in "
              "database.",
              h.action_url.size(), get_hosts_col_size(hosts_action_url));
          trunc_h.action_url.resize(get_hosts_col_size(hosts_action_url));
        }
        if (h.address.size() > get_hosts_col_size(hosts_address)) {
          log_v2::sql()->warn(
              "hosts address ({} instead of {}) is too long to be stored in "
              "database.",
              h.address.size(), get_hosts_col_size(hosts_address));
          trunc_h.address.resize(get_hosts_col_size(hosts_address));
        }
        if (h.alias.size() > get_hosts_col_size(hosts_alias)) {
          log_v2::sql()->warn(
              "hosts alias ({} instead of {}) is too long to be stored in "
              "database.",
              h.alias.size(), get_hosts_col_size(hosts_alias));
          trunc_h.alias.resize(get_hosts_col_size(hosts_alias));
        }
        if (h.check_command.size() > get_hosts_col_size(hosts_check_command)) {
          log_v2::sql()->warn(
              "hosts check_command ({} instead of {}) is too long to be stored in "
              "database.",
              h.check_command.size(), get_hosts_col_size(hosts_check_command));
          trunc_h.check_command.resize(get_hosts_col_size(hosts_check_command));
        }
        if (h.check_period.size() > get_hosts_col_size(hosts_check_period)) {
          log_v2::sql()->warn(
              "hosts check_period ({} instead of {}) is too long to be stored in "
              "database.",
              h.check_period.size(), get_hosts_col_size(hosts_check_period));
          trunc_h.check_period.resize(get_hosts_col_size(hosts_check_period));
        }
        if (h.display_name.size() > get_hosts_col_size(hosts_display_name)) {
          log_v2::sql()->warn(
              "hosts display_name ({} instead of {}) is too long to be stored in "
              "database.",
              h.display_name.size(), get_hosts_col_size(hosts_display_name));
          trunc_h.display_name.resize(get_hosts_col_size(hosts_display_name));
        }
        if (h.event_handler.size() > get_hosts_col_size(hosts_event_handler)) {
          log_v2::sql()->warn(
              "hosts event_handler ({} instead of {}) is too long to be stored in "
              "database.",
              h.event_handler.size(), get_hosts_col_size(hosts_event_handler));
          trunc_h.event_handler.resize(get_hosts_col_size(hosts_event_handler));
        }
        if (h.icon_image.size() > get_hosts_col_size(hosts_icon_image)) {
          log_v2::sql()->warn(
              "hosts icon_image ({} instead of {}) is too long to be stored in "
              "database.",
              h.icon_image.size(), get_hosts_col_size(hosts_icon_image));
          trunc_h.icon_image.resize(get_hosts_col_size(hosts_icon_image));
        }
        if (h.icon_image_alt.size() > get_hosts_col_size(hosts_icon_image_alt)) {
          log_v2::sql()->warn(
              "hosts icon_image_alt ({} instead of {}) is too long to be stored in "
              "database.",
              h.icon_image_alt.size(), get_hosts_col_size(hosts_icon_image_alt));
          trunc_h.icon_image_alt.resize(get_hosts_col_size(hosts_icon_image_alt));
        }
        if (h.notes.size() > get_hosts_col_size(hosts_notes)) {
          log_v2::sql()->warn(
              "hosts notes ({} instead of {}) is too long to be stored in "
              "database.",
              h.notes.size(), get_hosts_col_size(hosts_notes));
          trunc_h.notes.resize(get_hosts_col_size(hosts_notes));
        }
        if (h.notes_url.size() > get_hosts_col_size(hosts_notes_url)) {
          log_v2::sql()->warn(
              "hosts notes_url ({} instead of {}) is too long to be stored in "
              "database.",
              h.notes_url.size(), get_hosts_col_size(hosts_notes_url));
          trunc_h.notes_url.resize(get_hosts_col_size(hosts_notes_url));
        }
        if (h.notification_period.size() > get_hosts_col_size(hosts_notification_period)) {
          log_v2::sql()->warn(
              "hosts notification_period ({} instead of {}) is too long to be stored in "
              "database.",
              h.notification_period.size(), get_hosts_col_size(hosts_notification_period));
          trunc_h.notification_period.resize(get_hosts_col_size(hosts_notification_period));
        }
        if (h.output.size() > get_hosts_col_size(hosts_output)) {
          log_v2::sql()->warn(
              "hosts output ({} instead of {}) is too long to be stored in "
              "database.",
              h.output.size(), get_hosts_col_size(hosts_output));
          trunc_h.output.resize(get_hosts_col_size(hosts_output));
        }
        if (h.perf_data.size() > get_hosts_col_size(hosts_perfdata)) {
          log_v2::sql()->warn(
              "hosts perfdata ({} instead of {}) is too long to be stored in "
              "database.",
              h.perf_data.size(), get_hosts_col_size(hosts_perfdata));
          trunc_h.perf_data.resize(get_hosts_col_size(hosts_perfdata));
        }
        if (h.statusmap_image.size() > get_hosts_col_size(hosts_statusmap_image)) {
          log_v2::sql()->warn(
              "hosts statusmap_image ({} instead of {}) is too long to be stored in "
              "database.",
              h.statusmap_image.size(), get_hosts_col_size(hosts_statusmap_image));
          trunc_h.statusmap_image.resize(get_hosts_col_size(hosts_statusmap_image));
        }
        if (h.timezone.size() > get_hosts_col_size(hosts_timezone)) {
          log_v2::sql()->warn(
              "hosts timezone ({} instead of {}) is too long to be stored in "
              "database.",
              h.timezone.size(), get_hosts_col_size(hosts_timezone));
          trunc_h.timezone.resize(get_hosts_col_size(hosts_timezone));
        }
        _host_insupdate << trunc_h;
      }
      else
      _host_insupdate << h;
      log_v2::sql()->debug("insert or update host...");
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
  _pop_event(p);
  return 1;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_parent() {
  int32_t retval = 0;
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::host_dependencies |
                         actions::comments | actions::downtimes);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_parent::static_type())
      break;

    neb::host_parent const& hp(*static_cast<neb::host_parent const*>(d.get()));

    // Enable parenting.
    if (hp.enabled) {
      // Log message.
      logging::info(logging::medium) << "SQL: host " << hp.parent_id
                                     << " is parent of host " << hp.host_id;

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
      logging::info(logging::medium)
          << "SQL: host " << hp.parent_id << " is not parent of host "
          << hp.host_id << " anymore";

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
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_host_status() {
  _finish_action(-1, actions::instances | actions::downtimes |
                         actions::comments | actions::custom_variables |
                         actions::hostgroups | actions::host_dependencies |
                         actions::host_parents);
  auto& p = _events.front();

  // Processed object.
  neb::host_status const& hs(
      *static_cast<neb::host_status const*>(std::get<0>(p).get()));

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
    if (hs.check_command.size() > get_hosts_col_size(hosts_check_command) ||
        hs.check_period.size() > get_hosts_col_size(hosts_check_period) ||
        hs.event_handler.size() > get_hosts_col_size(hosts_event_handler) ||
        hs.output.size() > get_hosts_col_size(hosts_output) ||
        hs.perf_data.size() > get_hosts_col_size(hosts_perfdata)) {
      neb::host_status trunc_hs(hs);
      if (hs.check_command.size() > get_hosts_col_size(hosts_check_command)) {
        log_v2::sql()->warn(
            "hosts check_command ({} instead of {}) is too long to be stored "
            "in "
            "database.",
            hs.check_command.size(), get_hosts_col_size(hosts_check_command));
        trunc_hs.check_command.resize(get_hosts_col_size(hosts_check_command));
      }
      if (hs.check_period.size() > get_hosts_col_size(hosts_check_period)) {
        log_v2::sql()->warn(
            "hosts check_period ({} instead of {}) is too long to be stored in "
            "database.",
            hs.check_period.size(), get_hosts_col_size(hosts_check_period));
        trunc_hs.check_period.resize(get_hosts_col_size(hosts_check_period));
      }
      if (hs.event_handler.size() > get_hosts_col_size(hosts_event_handler)) {
        log_v2::sql()->warn(
            "hosts event_handler ({} instead of {}) is too long to be stored "
            "in "
            "database.",
            hs.event_handler.size(), get_hosts_col_size(hosts_event_handler));
        trunc_hs.event_handler.resize(get_hosts_col_size(hosts_event_handler));
      }
      if (hs.output.size() > get_hosts_col_size(hosts_output)) {
        log_v2::sql()->warn(
            "hosts output ({} instead of {}) is too long to be stored in "
            "database.",
            hs.output.size(), get_hosts_col_size(hosts_output));
        trunc_hs.output.resize(get_hosts_col_size(hosts_output));
      }
      if (hs.perf_data.size() > get_hosts_col_size(hosts_perfdata)) {
        log_v2::sql()->warn(
            "hosts perfdata ({} instead of {}) is too long to be stored in "
            "database.",
            hs.perf_data.size(), get_hosts_col_size(hosts_perfdata));
        trunc_hs.perf_data.resize(get_hosts_col_size(hosts_perfdata));
      }
      _host_status_update << trunc_hs;
    } else
      _host_status_update << hs;
    std::string err_msg(fmt::format(
        "SQL: could not store host status (host: {}): ", hs.host_id));
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[hs.host_id]);
    _mysql.run_statement(_host_status_update, err_msg, true, conn);
    _add_action(conn, actions::hosts);
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing host status event (id: " << hs.host_id
        << ", check type: " << hs.check_type
        << ", last check: " << hs.last_check
        << ", next check: " << hs.next_check << ", now: " << now << ", state ("
        << hs.current_state << ", " << hs.state_type << "))";
  _pop_event(p);
  return 1;
}

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_instance() {
  auto& p = _events.front();
  neb::instance& i(*static_cast<neb::instance*>(std::get<0>(p).get()));
  int32_t conn = _mysql.choose_connection_by_instance(i.poller_id);
  _finish_action(-1, actions::hosts | actions::acknowledgements |
                         actions::modules | actions::downtimes |
                         actions::comments | actions::servicegroups |
                         actions::hostgroups | actions::service_dependencies |
                         actions::host_dependencies);

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing poller event "
      << "(id: " << i.poller_id << ", name: " << i.name
      << ", running: " << (i.is_running ? "yes" : "no") << ")";

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

    if (i.name.size() > get_instances_col_size(instances_name) ||
        i.engine.size() > get_instances_col_size(instances_engine) ||
        i.version.size() > get_instances_col_size(instances_version)) {
      neb::instance trunc_i(i);
      if (i.name.size() > get_instances_col_size(instances_name)) {
        log_v2::sql()->warn(
            "instances name ({} instead of {}) is too long to be stored "
            "in "
            "database.",
            i.name.size(), get_instances_col_size(instances_name));
        trunc_i.name.resize(get_instances_col_size(instances_name));
      }
      if (i.engine.size() > get_instances_col_size(instances_engine)) {
        log_v2::sql()->warn(
            "instances engine ({} instead of {}) is too long to be stored in "
            "database.",
            i.engine.size(), get_instances_col_size(instances_engine));
        trunc_i.engine.resize(get_instances_col_size(instances_engine));
      }
      if (i.version.size() > get_instances_col_size(instances_version)) {
        log_v2::sql()->warn(
            "instances version ({} instead of {}) is too long to be stored "
            "in database.",
            i.version.size(), get_instances_col_size(instances_version));
        trunc_i.version.resize(get_instances_col_size(instances_version));
      }
      _instance_insupdate << trunc_i;
    } else
      _instance_insupdate << i;

    _mysql.run_statement(_instance_insupdate, err_msg, true, conn);
    _add_action(conn, actions::instances);
  }

  /* We just have to set the boolean */
  _pop_event(p);
  return 1;
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
int32_t conflict_manager::_process_instance_status() {
  // Cast object.
  auto& p = _events.front();
  neb::instance_status& is =
      *static_cast<neb::instance_status*>(std::get<0>(p).get());
  int32_t conn = _mysql.choose_connection_by_instance(is.poller_id);

  _finish_action(-1, actions::hosts | actions::acknowledgements |
                         actions::modules | actions::downtimes |
                         actions::comments);

  // Log message.
  logging::info(logging::medium)
      << "SQL: processing poller status event (id: " << is.poller_id
      << ", last alive: " << is.last_alive << ")";

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
    if (is.global_host_event_handler.size() >
            get_instances_col_size(instances_global_host_event_handler) ||
        is.global_service_event_handler.size() >
            get_instances_col_size(instances_global_service_event_handler)) {
      neb::instance_status trunc_is(is);
      if (is.global_host_event_handler.size() >
          get_instances_col_size(instances_global_host_event_handler)) {
        log_v2::sql()->warn(
            "instances global_host_event_handler ({} instead of {}) is too "
            "long to be stored in database.",
            is.global_host_event_handler.size(),
            get_instances_col_size(instances_global_host_event_handler));
        trunc_is.global_host_event_handler.resize(
            get_instances_col_size(instances_global_host_event_handler));
      }
      if (is.global_service_event_handler.size() >
          get_instances_col_size(instances_global_service_event_handler)) {
        log_v2::sql()->warn(
            "instances global_service_event_handler ({} instead of {}) is too "
            "long to be stored in database.",
            is.global_service_event_handler.size(),
            get_instances_col_size(instances_global_service_event_handler));
        trunc_is.global_service_event_handler.resize(
            get_instances_col_size(instances_global_service_event_handler));
      }
      _instance_status_insupdate << trunc_is;
    } else
      _instance_status_insupdate << is;

    std::string err_msg(fmt::format(
        "SQL: could not update poller (poller: {}): ", is.poller_id));
    _mysql.run_statement(_instance_status_insupdate, err_msg, true, conn);
    _add_action(conn, actions::instances);
  }
  _pop_event(p);
  return 1;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_log() {
  int32_t retval = 0;
  int conn = _mysql.choose_best_connection();

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::log_entry::static_type())
      break;

    // Fetch proper structure.
    neb::log_entry const& le(*static_cast<neb::log_entry const*>(d.get()));

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
    if (le.host_name.size() > get_logs_col_size(logs_host_name) ||
        le.poller_name.size() > get_logs_col_size(logs_instance_name) ||
        le.notification_cmd.size() > get_logs_col_size(logs_notification_cmd) ||
        le.notification_contact.size() > get_logs_col_size(logs_notification_contact) ||
        le.output.size() > get_logs_col_size(logs_output) ||
        le.service_description.size() > get_logs_col_size(logs_service_description)) {
      neb::log_entry trunc_le(le);
      if (le.host_name.size() > get_logs_col_size(logs_host_name)) {
        log_v2::sql()->warn(
            "logs host_name ({} instead of {}) is too long to be stored in "
            "database.",
            le.host_name.size(), get_logs_col_size(logs_host_name));
        trunc_le.host_name.resize(get_logs_col_size(logs_host_name));
      }
      if (le.poller_name.size() > get_logs_col_size(logs_instance_name)) {
        log_v2::sql()->warn(
            "logs instance_name ({} instead of {}) is too long to be stored in "
            "database.",
            le.poller_name.size(), get_logs_col_size(logs_instance_name));
        trunc_le.poller_name.resize(get_logs_col_size(logs_instance_name));
      }
      if (le.notification_cmd.size() > get_logs_col_size(logs_notification_cmd)) {
        log_v2::sql()->warn(
            "logs notification_cmd ({} instead of {}) is too long to be stored in "
            "database.",
            le.notification_cmd.size(), get_logs_col_size(logs_notification_cmd));
        trunc_le.notification_cmd.resize(get_logs_col_size(logs_notification_cmd));
      }
      if (le.notification_contact.size() > get_logs_col_size(logs_notification_contact)) {
        log_v2::sql()->warn(
            "logs notification_contact ({} instead of {}) is too long to be stored in "
            "database.",
            le.notification_contact.size(), get_logs_col_size(logs_notification_contact));
        trunc_le.notification_contact.resize(get_logs_col_size(logs_notification_contact));
      }
      if (le.output.size() > get_logs_col_size(logs_output)) {
        log_v2::sql()->warn(
            "logs output ({} instead of {}) is too long to be stored in "
            "database.",
            le.output.size(), get_logs_col_size(logs_output));
        trunc_le.output.resize(get_logs_col_size(logs_output));
      }
      if (le.service_description.size() > get_logs_col_size(logs_service_description)) {
        log_v2::sql()->warn(
            "logs service_description ({} instead of {}) is too long to be stored in "
            "database.",
            le.service_description.size(), get_logs_col_size(logs_service_description));
        trunc_le.service_description.resize(get_logs_col_size(logs_service_description));
      }
      _log_insert << trunc_le;
    }
    else
      _log_insert << le;

    _mysql.run_statement(_log_insert, "SQL: ", true, conn);
    /* We just have to set the boolean */
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a module event. We must take care of the thread id sending the
 *  query because the modules table has a constraint on instances.instance_id
 *
 *  @param[in] e Uncasted module.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_module() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Cast object.
  neb::module const& m = *static_cast<neb::module const*>(d.get());
  int32_t conn = _mysql.choose_connection_by_instance(m.poller_id);

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

    // Process object.
    neb::module* tmp_m;
    bool release = false;
    if (m.args.size() > get_modules_col_size(modules_args) ||
        m.filename.size() > get_modules_col_size(modules_filename)) {
      tmp_m = new neb::module(m);
      release = true;
      if (m.args.size() > get_modules_col_size(modules_args)) {
        log_v2::sql()->warn(
            "modules args ({} instead of {}) is too long to be stored in database.",
            m.args.size(), get_modules_col_size(modules_args));
        tmp_m->args.resize(get_modules_col_size(modules_args));
      }
      if (m.filename.size() > get_modules_col_size(modules_filename)) {
        log_v2::sql()->warn(
            "modules filename ({} instead of {}) is too long to be stored in database.",
            m.filename.size(), get_modules_col_size(modules_filename));
        tmp_m->filename.resize(get_modules_col_size(modules_filename));
      }
    }
    else
      tmp_m = const_cast<neb::module*>(&m);

    if (tmp_m->enabled) {
      std::string err_msg(fmt::format(
          "SQL: could not store module (poller: {}): ", tmp_m->poller_id));
      _module_insert << *tmp_m;
      _mysql.run_statement(_module_insert, err_msg, true, conn);
      _add_action(conn, actions::modules);
    } else {
      std::string query(fmt::format(
          "DELETE FROM modules WHERE instance_id={} AND filename='{}'",
          tmp_m->poller_id, tmp_m->filename));
      _mysql.run_query(query, "SQL: ", false, conn);
      _add_action(conn, actions::modules);
    }
    // the goal here is to avoid a double free corruption.
    if (release)
      delete tmp_m;
  }
  _pop_event(p);
  return 1;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service_check() {
  _finish_action(-1, actions::downtimes | actions::comments |
                         actions::host_dependencies | actions::host_parents |
                         actions::service_dependencies);
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

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
    bool store = true;
    size_t str_hash = std::hash<std::string>{}(sc.command_line);
    // Did the command changed since last time?
    if (_cache_svc_cmd[{sc.host_id, sc.service_id}] != str_hash)
      _cache_svc_cmd[std::make_pair(sc.host_id, sc.service_id)] = str_hash;
    else
      store = false;

    if (store) {
      if (_cache_host_instance[sc.service_id]) {
        if (sc.command_line.size() > get_services_col_size(services_command_line)) {
          neb::service_check trunc_sc(sc);
          log_v2::sql()->warn(
              "services command_line ({} instead of {}) is too long to be stored in database.",
              sc.command_line.size(), get_services_col_size(services_command_line));
          trunc_sc.command_line.resize(get_services_col_size(services_command_line));
          _service_check_update << trunc_sc;
        } else
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
    logging::info(logging::medium)
        << "SQL: not processing service check event (host: " << sc.host_id
        << ", service: " << sc.service_id << ", command: " << sc.command_line
        << ", check_type: " << sc.check_type
        << ", next_check: " << sc.next_check << ", now: " << now << ")";
  _pop_event(p);
  return 1;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service_dependency() {
  int32_t retval = 0;
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::host_parents |
                         actions::downtimes | actions::comments |
                         actions::host_dependencies |
                         actions::service_dependencies);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::service_dependency::static_type())
      break;

    // Cast object.
    neb::service_dependency const& sd(
        *static_cast<neb::service_dependency const*>(d.get()));

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
        _service_dependency_insupdate = qp.prepare_insert_or_update(_mysql);
      }

      // Process object.
      std::string err_msg(fmt::format(
          "SQL: could not store service dependency (host: {}, service: {}, "
          "dependent host: {}, dependent service: {}): ",
          sd.host_id, sd.service_id, sd.dependent_host_id,
          sd.dependent_service_id));

      if (sd.dependency_period.size() >
              get_services_services_dependencies_col_size(
                  services_services_dependencies_dependency_period) ||
          sd.execution_failure_options.size() >
              get_services_services_dependencies_col_size(
                  services_services_dependencies_execution_failure_options) ||
          sd.notification_failure_options.size() >
              get_services_services_dependencies_col_size(
                  services_services_dependencies_notification_failure_options)) {
        neb::service_dependency trunc_sd(sd);
        if (sd.dependency_period.size() >
            get_services_services_dependencies_col_size(
                services_services_dependencies_dependency_period)) {
          log_v2::sql()->warn(
              "services_services_dependencies dependency period ({} instead of {}) is too long to be stored in database.",
              sd.dependency_period.size(),
              get_services_services_dependencies_col_size(
                  services_services_dependencies_dependency_period));
          trunc_sd.dependency_period.resize(
              get_services_services_dependencies_col_size(
                  services_services_dependencies_dependency_period));
        }
        if (sd.execution_failure_options.size() >
            get_services_services_dependencies_col_size(
                services_services_dependencies_execution_failure_options)) {
          log_v2::sql()->warn(
              "services_services_dependencies execution failure options ({} instead of {}) is too long to be stored in database.",
              sd.execution_failure_options.size(),
              get_services_services_dependencies_col_size(
                  services_services_dependencies_execution_failure_options));
          trunc_sd.execution_failure_options.resize(
              get_services_services_dependencies_col_size(
                  services_services_dependencies_execution_failure_options));
        }
        if (sd.notification_failure_options.size() >
            get_services_services_dependencies_col_size(
                services_services_dependencies_notification_failure_options)) {
          log_v2::sql()->warn(
              "services_services_dependencies notification failure options ({} "
              "instead of {}) is too long to be stored in database.",
              sd.notification_failure_options.size(),
              get_services_services_dependencies_col_size(
                  services_services_dependencies_notification_failure_options));
          trunc_sd.notification_failure_options.resize(
              get_services_services_dependencies_col_size(
                  services_services_dependencies_notification_failure_options));
        }
        _service_dependency_insupdate << trunc_sd;
      } else
        _service_dependency_insupdate << sd;
      _service_dependency_insupdate << sd;
      _mysql.run_statement(_service_dependency_insupdate, err_msg, true, conn);
      _add_action(conn, actions::service_dependencies);
    }
    // Delete.
    else {
      logging::info(logging::medium)
          << "SQL: removing service dependency of (" << sd.dependent_host_id
          << ", " << sd.dependent_service_id << ") on (" << sd.host_id << ", "
          << sd.service_id << ")";
      std::string query(
          fmt::format("DELETE FROM serivces_services_dependencies WHERE dependent_host_id={} AND dependent_service_id={} AND host_id={} AND service_id={}",
                      sd.dependent_host_id, sd.dependent_service_id, sd.host_id,
                      sd.service_id));
      _mysql.run_query(query, "SQL: ", false, conn);
      _add_action(conn, actions::service_dependencies);
    }
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service_group() {
  int32_t retval = 0;
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::services);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (d->type() != neb::service_group::static_type())
      break;

    // Cast object.
    neb::service_group const& sg(
        *static_cast<neb::service_group const*>(d.get()));

    // Insert/update group.
    if (sg.enabled) {
      logging::info(logging::medium)
          << "SQL: enabling service group " << sg.id << " ('" << sg.name
          << "') on instance " << sg.poller_id;
      _prepare_sg_insupdate_statement();

      std::string err_msg(fmt::format(
          "SQL: could not store service group (poller: {}, group: {}): ",
          sg.poller_id, sg.id));

      if (sg.name.size() > get_servicegroups_col_size(servicegroups_name)) {
        neb::service_group trunc_sg(sg);
        log_v2::sql()->warn(
            "servicegroups name ({} instead of {}) is too long to be "
            "stored in database.",
            sg.name.size(), get_servicegroups_col_size(servicegroups_name));
        trunc_sg.name.resize(get_servicegroups_col_size(servicegroups_name));
        _service_group_insupdate << trunc_sg;
      } else
        _service_group_insupdate << sg;

      _service_group_insupdate << sg;
      _mysql.run_statement(_service_group_insupdate, err_msg, true, conn);
      _add_action(conn, actions::servicegroups);
      _servicegroup_cache.insert(sg.id);
    }
    // Delete group.
    else {
      logging::info(logging::medium)
          << "SQL: disabling service group " << sg.id << " ('" << sg.name
          << "') on instance " << sg.poller_id;

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
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service_group_member() {
  int32_t retval = 0;
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1,
                 actions::hosts | actions::servicegroups | actions::services);

  int32_t count = _get_events_size();
  while (count-- > 0) {
    auto& p = _events.front();

    if (std::get<1>(p) != stream_type::sql)
      break;

    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::service_group_member::static_type())
      break;

    // Cast object.
    neb::service_group_member const& sgm(
        *static_cast<neb::service_group_member const*>(d.get()));

    if (sgm.enabled) {
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
    _pop_event(p);
    retval++;
  }
  return retval;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service() {
  _finish_action(-1, actions::host_parents | actions::comments |
                         actions::downtimes | actions::host_dependencies |
                         actions::service_dependencies);
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(d.get()));
  if (_cache_host_instance[s.host_id]) {
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]);

    // Log message.
    logging::info(logging::medium)
        << "SQL: processing service event "
           "(host id: "
        << s.host_id << ", service_id: " << s.service_id
        << ", description: " << s.service_description << ")";

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
        logging::debug(logging::medium) << "mysql: PREPARE INSERT ON SERVICES";
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
  _pop_event(p);
  return 1;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_service_status() {
  _finish_action(-1, actions::host_parents | actions::comments |
                         actions::downtimes | actions::host_dependencies |
                         actions::service_dependencies);
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
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
    std::string err_msg(fmt::format(
        "SQL: could not store service status (host: {}, service: {}) ",
        ss.host_id, ss.service_id));
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);
    _mysql.run_statement(_service_status_update, err_msg, false, conn);
    _add_action(conn, actions::hosts);
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing service status event (host: " << ss.host_id
        << ", service: " << ss.service_id << ", check_type: " << ss.check_type
        << ", last check: " << ss.last_check
        << ", next_check: " << ss.next_check << ", now: " << now << ", state ("
        << ss.current_state << ", " << ss.state_type << "))";
  _pop_event(p);
  return 1;
}

/**
 *  Process an instance configuration event.
 *
 *  @param[in] e  Uncasted instance configuration.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_instance_configuration() {
  auto& p = _events.front();
  /* Nothing to do */
  _pop_event(p);
  return 1;
}

/**
 *  Process a responsive instance event.
 *
 * @return The number of events that can be acknowledged.
 */
int32_t conflict_manager::_process_responsive_instance() {
  auto& p = _events.front();
  /* Nothing to do */
  _pop_event(p);
  return 1;
}
