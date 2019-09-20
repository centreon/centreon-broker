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
#include <cassert>
#include <sstream>
#include "com/centreon/broker/database/mysql_result.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;
using namespace com::centreon::broker::sql;

void conflict_manager::_clean_tables(uint32_t instance_id
                                     __attribute__((unused))) {}

bool conflict_manager::_is_valid_poller(uint32_t instance_id
                                        __attribute__((unused))) {
  return true;
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
 */
void conflict_manager::_process_acknowledgement() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Cast object.
  neb::acknowledgement const& ack =
      *static_cast<neb::acknowledgement const*>(d.get());

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
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process a comment event.
 *
 *  @param[in] e  Uncasted comment.
 */
void conflict_manager::_process_comment() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Cast object.
  neb::comment const& cmmnt{*static_cast<neb::comment const*>(d.get())};
  int32_t conn = _mysql.choose_connection_by_instance(cmmnt.poller_id);

  // Log message.
  logging::info(logging::medium) << "SQL: processing comment of poller "
                                 << cmmnt.poller_id << " on (" << cmmnt.host_id
                                 << ", " << cmmnt.service_id << ")";

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
  std::ostringstream oss;
  oss << "SQL: could not store comment (poller: " << cmmnt.poller_id
      << ", host: " << cmmnt.host_id << ", service: " << cmmnt.service_id
      << ", entry time: " << cmmnt.entry_time
      << ", internal ID: " << cmmnt.internal_id << "): ";

  _comment_insupdate << cmmnt;
  _mysql.run_statement(_comment_insupdate, oss.str(), true, conn);
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void conflict_manager::_process_custom_variable() {
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::custom_variables);
  while (!_events.empty()) {
    auto& p = _events.front();
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
      logging::info(logging::medium) << "SQL: enabling custom variable '"
                                     << cv.name << "' of (" << cv.host_id
                                     << ", " << cv.service_id << ")";
      std::ostringstream oss;
      oss << "SQL: could not store custom variable (name: " << cv.name
          << ", host: " << cv.host_id << ", service: " << cv.service_id
          << "): ";

      _custom_variable_insupdate << cv;
      _mysql.run_statement(_custom_variable_insupdate, oss.str(), true, conn);
      _add_action(conn, actions::custom_variables);
    } else {
      logging::info(logging::medium) << "SQL: disabling custom variable '"
                                     << cv.name << "' of (" << cv.host_id
                                     << ", " << cv.service_id << ")";
      _custom_variable_delete.bind_value_as_i32(":host_id", cv.host_id);
      _custom_variable_delete.bind_value_as_i32(":service_id", cv.service_id);
      _custom_variable_delete.bind_value_as_str(":name", cv.name);

      std::ostringstream oss;
      oss << "SQL: could not remove custom variable (host: " << cv.host_id
          << ", service: " << cv.service_id << ", name '" << cv.name << "'): ";
      _mysql.run_statement(_custom_variable_delete,
                           oss.str(),
                           true, conn);
      _add_action(conn, actions::custom_variables);
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void conflict_manager::_process_custom_variable_status() {
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::custom_variables);
  while (!_events.empty()) {
    auto& p = _events.front();
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

    logging::info(logging::medium) << "SQL: enabling custom variable '"
                                   << cv.name << "' of (" << cv.host_id
                                   << ", " << cv.service_id << ")";
    std::ostringstream oss;
    oss << "SQL: could not store custom variable (name: " << cv.name
        << ", host: " << cv.host_id << ", service: " << cv.service_id
        << "): ";

    _custom_variable_status_insupdate << cv;
    _mysql.run_statement(_custom_variable_status_insupdate, oss.str(), true, conn);
    _add_action(conn, actions::custom_variables);
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 */
void conflict_manager::_process_downtime() {
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hosts | actions::instances);

  while (!_events.empty()) {
    auto& p = _events.front();
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
      std::ostringstream oss;
      oss << "SQL: could not store downtime (poller: " << dd.poller_id
          << ", host: " << dd.host_id << ", service: " << dd.service_id << "): ";

      _downtime_insupdate << dd;
      _mysql.run_statement(_downtime_insupdate, oss.str(), true, conn);
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void conflict_manager::_process_event_handler() {
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
  std::ostringstream oss;
  oss << "SQL: could not store event handler (host: " << eh.host_id
      << ", service: " << eh.service_id << ", start time: " << eh.start_time
      << "): ";

  _event_handler_insupdate << eh;
  _mysql.run_statement(
      _event_handler_insupdate, oss.str(), true,
      _mysql.choose_connection_by_instance(_cache_host_instance[eh.host_id]));
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void conflict_manager::_process_flapping_status() {
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
    _flapping_status_insupdate =
        qp.prepare_insert_or_update(_mysql);
  }

  // Processing.
  std::ostringstream oss;
  oss << "SQL: could not store flapping status (host: " << fs.host_id
      << ", service: " << fs.service_id << ", event time: " << fs.event_time
      << "): ";

  _flapping_status_insupdate << fs;
  _mysql.run_statement(
      _flapping_status_insupdate,
      oss.str(),
      true,
      _mysql.choose_connection_by_instance(_cache_host_instance[fs.host_id]));
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 */
void conflict_manager::_process_host_check() {
  while (!_events.empty()) {
    auto& p = _events.front();
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
      bool store;
      size_t str_hash = std::hash<std::string> {}
      (hc.command_line);
      // Did the command changed since last time?
      if (_cache_hst_cmd[hc.host_id] != str_hash) {
        store = true;
        _cache_hst_cmd[hc.host_id] = str_hash;
      }
      else
        store = false;

      if (store) {
        int32_t conn = _mysql.choose_connection_by_instance(
            _cache_host_instance[hc.host_id]);
        _host_check_update << hc;

        std::promise<int> promise;
        std::ostringstream oss;
        oss << "SQL: could not store host check (host: " << hc.host_id << "): ";
        _mysql.run_statement(_host_check_update, oss.str(), true, conn);
      }
    } else
      // Do nothing.
      logging::info(logging::medium)
          << "SQL: not processing host check event (host: " << hc.host_id
          << ", command: " << hc.command_line
          << ", check type: " << hc.check_type
          << ", next check: " << hc.next_check << ", now: " << now << ")";
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

void conflict_manager::_process_host_dependency() {}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void conflict_manager::_process_host_group() {
  int conn = _mysql.choose_best_connection();

  while (!_events.empty()) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_group::static_type())
      break;

    // Cast object.
    neb::host_group const& hg{*static_cast<neb::host_group const*>(d.get())};

    if (hg.enabled) {
      logging::info(logging::medium) << "SQL: enabling host group " << hg.id
                                     << " ('" << hg.name << "') on instance "
                                     << hg.poller_id;
      _prepare_hg_insupdate_statement();

      std::ostringstream oss;
      oss << "SQL: could not store host group (poller: " << hg.poller_id
          << ", group: " << hg.id << "): ";

      _host_group_insupdate << hg;
      _mysql.run_statement(_host_group_insupdate, oss.str(), true, conn);
      _add_action(conn, actions::hostgroups);
      _hostgroup_cache.insert(hg.id);
    }
    // Delete group.
    else {
      logging::info(logging::medium) << "SQL: disabling host group " << hg.id
                                     << " ('" << hg.name << "' on instance "
                                     << hg.poller_id;

      // Delete group members.
      {
        std::ostringstream oss;
        oss << "DELETE hosts_hostgroups"
            << "  FROM hosts_hostgroups"
            << "  LEFT JOIN hosts"
            << "    ON hosts_hostgroups.host_id=hosts.host_id"
            << "  WHERE hosts_hostgroups.hostgroup_id=" << hg.id
            << "    AND hosts.instance_id=" << hg.poller_id;
        _mysql.run_query(oss.str(), "SQL: ", false, conn);
        _hostgroup_cache.erase(hg.id);
      }
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void conflict_manager::_process_host_group_member() {
  int conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::hostgroups | actions::hosts);

  while (!_events.empty()) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::host_group_member::static_type())
      break;

    // Cast object.
    neb::host_group_member const& hgm(
        *static_cast<neb::host_group_member const*>(d.get()));

    if (hgm.enabled) {
      // Log message.
      logging::info(logging::medium) << "SQL: enabling membership of host "
                                     << hgm.host_id << " to host group "
                                     << hgm.group_id << " on instance "
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
      if (_hostgroup_cache.find(hgm.group_id) == _hostgroup_cache.end()) {
        logging::error(logging::low)
            << "SQL: host group " << hgm.group_id
            << " does not exist - insertion before insertion of members";
        _prepare_hg_insupdate_statement();

        neb::host_group hg;
        hg.id = hgm.group_id;
        hg.name = hgm.group_name;
        hg.enabled = true;
        assert(_cache_host_instance[hgm.host_id]);
        hg.poller_id = _cache_host_instance[hgm.host_id];
        assert(hg.poller_id);

        std::ostringstream oss;
        oss << "SQL: could not store host group (poller: " << hg.poller_id
            << ", group: " << hg.id << "): ";

        _host_group_insupdate << hg;
        _mysql.run_statement(_host_group_insupdate, oss.str(), false, conn);
      }

      _host_group_member_insert << hgm;
      std::ostringstream oss;
      oss << "SQL: could not store host group membership (poller: "
          << hgm.poller_id << ", host: " << hgm.host_id
          << ", group: " << hgm.group_id << "): ";
      _mysql.run_statement(_host_group_member_insert, oss.str(), false, conn);
    }
    // Delete.
    else {
      // Log message.
      logging::info(logging::medium) << "SQL: disabling membership of host "
                                     << hgm.host_id << " to host group "
                                     << hgm.group_id << " on instance "
                                     << hgm.poller_id;

      if (!_host_group_member_delete.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("hostgroup_id");
        unique.insert("host_id");
        query_preparator qp(neb::host_group_member::static_type(), unique);
        _host_group_member_delete = qp.prepare_delete(_mysql);
      }
      std::ostringstream oss;
      oss << "SQL: cannot delete membership of host " << hgm.host_id
          << " to host group " << hgm.group_id << " on instance "
          << hgm.poller_id << ": ";

      _host_group_member_delete << hgm;
      _mysql.run_statement(_host_group_member_delete, oss.str(), true, conn);
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void conflict_manager::_process_host() {
  auto& p = _events.front();
  neb::host& h = *static_cast<neb::host*>(std::get<0>(p).get());

  // Log message.
  logging::info(logging::medium) << "SQL: processing host event"
                                    " (poller: " << h.poller_id
                                 << ", id: " << h.host_id
                                 << ", name: " << h.host_name << ")";

  // Processing
  if (_is_valid_poller(h.poller_id)) {
    if (h.host_id) {
      int32_t conn = _mysql.choose_connection_by_instance(h.poller_id);

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
      _mysql.run_statement(_host_insupdate,
                           oss.str(),
                           true, conn);
      _add_action(conn, actions::hosts);

      // Fill the cache...
      _cache_host_instance[h.host_id] = h.poller_id;
    } else
      logging::error(logging::high) << "SQL: host '" << h.host_name
                                    << "' of poller " << h.poller_id
                                    << " has no ID";
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

void conflict_manager::_process_host_parent() {}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void conflict_manager::_process_host_status() {
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
    logging::info(logging::medium) << "SQL: processing host status event (id: "
                                   << hs.host_id
                                   << ", last check: " << hs.last_check
                                   << ", state (" << hs.current_state << ", "
                                   << hs.state_type << "))";

    // Prepare queries.
    if (!_host_status_update.prepared()) {
      query_preparator::event_unique unique;
      unique.insert("host_id");
      query_preparator qp(neb::host_status::static_type(), unique);
      _host_status_update = qp.prepare_update(_mysql);
    }

    // Processing.
    _host_status_update << hs;
    std::ostringstream oss;
    oss << "SQL: could not store host status (host: " << hs.host_id << "): ";
    _mysql.run_statement(
        _host_status_update,
        oss.str(),
        true,
        _mysql.choose_connection_by_instance(_cache_host_instance[hs.host_id]));
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing host status event (id: " << hs.host_id
        << ", check type: " << hs.check_type
        << ", last check: " << hs.last_check
        << ", next check: " << hs.next_check << ", now: " << now << ", state ("
        << hs.current_state << ", " << hs.state_type << "))";
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 */
void conflict_manager::_process_instance() {
  auto& p = _events.front();
  neb::instance& i(*static_cast<neb::instance*>(std::get<0>(p).get()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing poller event "
                                 << "(id: " << i.poller_id
                                 << ", name: " << i.name << ", running: "
                                 << (i.is_running ? "yes" : "no") << ")";

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

    int32_t conn = _mysql.choose_connection_by_instance(i.poller_id);
    _mysql.run_statement(_instance_insupdate,
                         oss.str(),
                         true, conn);
    _add_action(conn, actions::instances);
  }

  /* We just have to set the boolean */
  *std::get<2>(p) = true;
  _events.pop_front();
}

void conflict_manager::_process_instance_status() {}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void conflict_manager::_process_log() {
  int conn = _mysql.choose_best_connection();

  while (!_events.empty()) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::log_entry::static_type())
      break;

    // Fetch proper structure.
    neb::log_entry const& le(*static_cast<neb::log_entry const*>(d.get()));

    // Log message.
    logging::info(logging::medium) << "SQL: processing log of poller '"
                                   << le.poller_name << "' generated at "
                                   << le.c_time << " (type " << le.msg_type
                                   << ")";

    // Prepare query.
    if (!_log_insert.prepared()) {
      query_preparator qp(neb::log_entry::static_type());
      _log_insert = qp.prepare_insert(_mysql);
    }

    // Run query.
    _log_insert << le;
    _mysql.run_statement(_log_insert, "SQL: ", conn);
    /* We just have to set the boolean */
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

void conflict_manager::_process_module() {}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void conflict_manager::_process_service_check() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Cast object.
  neb::service_check const& sc(
      *static_cast<neb::service_check const*>(d.get()));

  time_t now{time(nullptr)};
  if (sc.check_type  // - passive result
      ||
      !sc.active_checks_enabled  // - active checks are disabled,
                                 //   status might not be updated
                                 // - normal case
      ||
      (sc.next_check >= now - 5 * 60) || !sc.next_check) {  // - initial state
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
    size_t str_hash = std::hash<std::string> {} (sc.command_line);
    // Did the command changed since last time?
    if (_cache_svc_cmd[{sc.host_id, sc.service_id}] != str_hash)
      _cache_svc_cmd[std::make_pair(sc.host_id, sc.service_id)] = str_hash;
    else
      store = false;

    if (store) {
      _service_check_update << sc;
      std::promise<int> promise;
      std::stringstream oss;
      oss << "SQL: could not store service check command (host: " << sc.host_id
          << ", service: " << sc.service_id << "): ";
      assert(_cache_host_instance[sc.host_id]);
      int32_t conn = _mysql.choose_connection_by_instance(_cache_host_instance[sc.host_id]);
      _mysql.run_statement(_service_check_update,
                           oss.str(),
                           true, conn);
    }
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing service check event (host: " << sc.host_id
        << ", service: " << sc.service_id << ", command: " << sc.command_line
        << ", check_type: " << sc.check_type
        << ", next_check: " << sc.next_check << ", now: " << now << ")";
  *std::get<2>(p) = true;
  _events.pop_front();
}

void conflict_manager::_process_service_dependency() {}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void conflict_manager::_process_service_group() {
  int32_t conn = _mysql.choose_best_connection();

  while (!_events.empty()) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (d->type() != neb::service_group::static_type())
      break;

    // Cast object.
    neb::service_group const& sg(
        *static_cast<neb::service_group const*>(d.get()));

    // Insert/update group.
    if (sg.enabled) {
      logging::info(logging::medium) << "SQL: enabling service group " << sg.id
                                     << " ('" << sg.name << "') on instance "
                                     << sg.poller_id;
      _prepare_sg_insupdate_statement();

      std::stringstream oss;
      oss << "SQL: could not store service group (poller: " << sg.poller_id
          << ", group: " << sg.id << "): ";

      _service_group_insupdate << sg;
      _mysql.run_statement(_service_group_insupdate, oss.str(), true, conn);
      _add_action(conn, actions::servicegroups);
      _servicegroup_cache.insert(sg.id);
    }
    // Delete group.
    else {
      logging::info(logging::medium) << "SQL: disabling service group " << sg.id
                                     << " ('" << sg.name << "') on instance "
                                     << sg.poller_id;

      // Delete group members.
      {
        std::ostringstream oss;
        oss << "DELETE services_servicegroups"
            << "  FROM services_servicegroups"
            << "  LEFT JOIN hosts"
            << "    ON services_servicegroups.host_id=hosts.host_id"
            << "  WHERE services_servicegroups.servicegroup_id=" << sg.id
            << "    AND hosts.instance_id=" << sg.poller_id;
        _mysql.run_query(oss.str(), "SQL: ", false, conn);
        _servicegroup_cache.erase(sg.id);
      }
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void conflict_manager::_process_service_group_member() {
  int32_t conn = _mysql.choose_best_connection();
  _finish_action(-1, actions::servicegroups | actions::services);

  while (!_events.empty()) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{std::get<0>(p)};

    if (!d || d->type() != neb::service_group_member::static_type())
      break;

    // Cast object.
    neb::service_group_member const& sgm(
        *static_cast<neb::service_group_member const*>(d.get()));

    if (sgm.enabled) {
      // Log message.
      logging::info(logging::medium) << "SQL: enabling membership of service ("
                                     << sgm.host_id << ", " << sgm.service_id
                                     << ") to service group " << sgm.group_id
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

        std::ostringstream oss;
        oss << "SQL: could not store service group (poller: " << sg.poller_id
            << ", group: " << sg.id << "): ";

        _service_group_insupdate << sg;
        _mysql.run_statement(_service_group_insupdate, oss.str(), false, conn);
      }

      _service_group_member_insert << sgm;
      std::ostringstream oss;
      oss << "SQL: could not store service group membership (poller: "
          << sgm.poller_id << ", host: " << sgm.host_id
          << ", service: " << sgm.service_id << ", group: " << sgm.group_id
          << "): ";
      _mysql.run_statement(
          _service_group_member_insert, oss.str(), false, conn);
    }
    // Delete.
    else {
      // Log message.
      logging::info(logging::medium) << "SQL: disabling membership of service ("
                                     << sgm.host_id << ", " << sgm.service_id
                                     << ") to service group " << sgm.group_id
                                     << " on instance " << sgm.poller_id;

      if (!_service_group_member_delete.prepared()) {
        query_preparator::event_unique unique;
        unique.insert("servicegroup_id");
        unique.insert("host_id");
        unique.insert("service_id");
        query_preparator qp(neb::service_group_member::static_type(), unique);
        _service_group_member_delete = qp.prepare_delete(_mysql);
      }
      std::ostringstream oss;
      oss << "SQL: cannot delete membership of service (" << sgm.host_id << ", "
          << sgm.service_id << ") to service group " << sgm.group_id
          << " on instance " << sgm.poller_id << ": ";

      _service_group_member_delete << sgm;
      _mysql.run_statement(_service_group_member_delete, oss.str(), true, conn);
    }
    *std::get<2>(p) = true;
    _events.pop_front();
  }
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void conflict_manager::_process_service() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};

  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(d.get()));
  assert(_cache_host_instance[s.host_id]);
  int32_t conn =
      _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]);

  // Log message.
  logging::info(logging::medium) << "SQL: processing service event "
                                    "(host id: " << s.host_id
                                 << ", service_id: " << s.service_id
                                 << ", description: " << s.service_description
                                 << ")";

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
        _service_insupdate, oss.str(), true, conn);
  } else
    logging::error(logging::high) << "SQL: service '" << s.service_description
                                  << "' has no host ID or no service ID";
  *std::get<2>(p) = true;
  _events.pop_front();
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void conflict_manager::_process_service_status() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
  // Processed object.
  neb::service_status const& ss{
      *static_cast<neb::service_status const*>(d.get())};

  time_t now = time(nullptr);
  if (ss.check_type ||           // - passive result
      !ss.active_checks_enabled  // - active checks are disabled,
                                 //   status might not be updated
      ||                         // - normal case
      ss.next_check >= now - 5 * 60 ||
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
    std::ostringstream oss;
    oss << "SQL: could not store service status (host: " << ss.host_id << ", "
        << ss.service_id << ") ";
    int32_t conn =
        _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);
    _mysql.run_statement(
        _service_status_update,
        oss.str(),
        true, conn);
  } else
    // Do nothing.
    logging::info(logging::medium)
        << "SQL: not processing service status event (host: " << ss.host_id
        << ", service: " << ss.service_id << ", check_type: " << ss.check_type
        << ", last check: " << ss.last_check
        << ", next_check: " << ss.next_check << ", now: " << now << ", state ("
        << ss.current_state << ", " << ss.state_type << "))";
  *std::get<2>(p) = true;
  _events.pop_front();
}

void conflict_manager::_process_instance_configuration() {}
void conflict_manager::_process_responsive_instance() {}
