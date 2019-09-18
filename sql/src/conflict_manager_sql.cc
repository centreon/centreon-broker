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

void conflict_manager::_clean_tables(uint32_t instance_id) {}

bool conflict_manager::_is_valid_poller(uint32_t instance_id) { return true; }

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

void conflict_manager::_process_acknowledgement() {}

/**
 *  Process a comment event.
 *
 *  @param[in] e  Uncasted comment.
 */
void conflict_manager::_process_comment() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{p.first};

  // Cast object.
  neb::comment const& cmmnt{*static_cast<neb::comment const*>(d.get())};

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
  std::ostringstream oss;
  oss << "SQL: could not store comment (poller: " << cmmnt.poller_id
      << ", host: " << cmmnt.host_id << ", service: " << cmmnt.service_id
      << ", entry time: " << cmmnt.entry_time
      << ", internal ID: " << cmmnt.internal_id << "): ";

  _comment_insupdate << cmmnt;
  _mysql.run_statement(_comment_insupdate, oss.str(), true);
  *p.second = true;
  _events.pop_front();
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void conflict_manager::_process_custom_variable() {
  std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
  std::shared_ptr<io::data> d{p.first};

  // Cast object.
  neb::custom_variable const& cv(
      *static_cast<neb::custom_variable const*>(d.get()));

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
                                   << cv.name << "' of (" << cv.host_id << ", "
                                   << cv.service_id << ")";
    std::ostringstream oss;
    oss << "SQL: could not store custom variable (name: " << cv.name
        << ", host: " << cv.host_id << ", service: " << cv.service_id << "): ";

    _custom_variable_insupdate << cv;
    _mysql.run_statement(_custom_variable_insupdate, oss.str(), true);
  } else {
    logging::info(logging::medium) << "SQL: disabling custom variable '"
                                   << cv.name << "' of (" << cv.host_id << ", "
                                   << cv.service_id << ")";
    _custom_variable_delete.bind_value_as_i32(":host_id", cv.host_id);
    _custom_variable_delete.bind_value_as_i32(":service_id", cv.service_id);
    _custom_variable_delete.bind_value_as_str(":name", cv.name);

    std::ostringstream oss;
    oss << "SQL: could not remove custom variable (host: " << cv.host_id
        << ", service: " << cv.service_id << ", name '" << cv.name << "'): ";
    _mysql.run_statement(
        _custom_variable_delete,
        oss.str(),
        true,
        _mysql.choose_connection_by_instance(_cache_host_instance[cv.host_id]));
  }
  *p.second = true;
  _events.pop_front();
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void conflict_manager::_process_custom_variable_status() {
  std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
  std::shared_ptr<io::data> d{p.first};
  // Cast object.
  neb::custom_variable_status const& cvs(
      *static_cast<neb::custom_variable_status const*>(d.get()));

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
    _custom_variable_status_update = qp.prepare_update(_mysql);
  }

  // Processing.
  _custom_variable_status_update << cvs;
  std::promise<int> promise;
  _mysql.run_statement_and_get_int(
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
  *p.second = true;
  _events.pop_front();
}

void conflict_manager::_process_downtime() {}
void conflict_manager::_process_event_handler() {}
void conflict_manager::_process_flapping_status() {}
void conflict_manager::_process_host_check() {}
void conflict_manager::_process_host_dependency() {}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void conflict_manager::_process_host_group() {
  int conn = _mysql.choose_best_connection();

  while (true) {
    std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
    std::shared_ptr<io::data> d{p.first};
    uint32_t type{d->type()};

    if (type != neb::host_group::static_type())
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
      }
    }
    *p.second = true;
    _events.pop_front();
  }
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void conflict_manager::_process_host_group_member() {
  std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
  std::shared_ptr<io::data> d{p.first};

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
    _host_group_member_insert << hgm;

    std::promise<mysql_result> promise;
    _mysql.run_statement_and_get_result(_host_group_member_insert, &promise);
    try {
      promise.get_future().get();
    }
    catch (std::exception const& e) {
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
      _mysql.run_statement(_host_group_insupdate, oss.str(), false);

      oss.str("");
      oss << "SQL: could not store host group membership (poller: "
          << hgm.poller_id << ", host: " << hgm.host_id
          << ", group: " << hgm.group_id << "): ";
      _host_group_member_insert << hgm;
      _mysql.run_statement(_host_group_member_insert, oss.str(), false);
    }
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
        << " to host group " << hgm.group_id << " on instance " << hgm.poller_id
        << ": ";

    _host_group_member_delete << hgm;
    _mysql.run_statement(_host_group_member_delete, oss.str(), true);
  }
  *p.second = true;
  _events.pop_front();
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void conflict_manager::_process_host() {
  std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
  neb::host& h = *static_cast<neb::host*>(p.first.get());

  // Log message.
  logging::info(logging::medium) << "SQL: processing host event"
                                    " (poller: " << h.poller_id
                                 << ", id: " << h.host_id
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
      _mysql.run_statement(_host_insupdate,
                           oss.str(),
                           true,
                           _mysql.choose_connection_by_instance(h.poller_id));

      // Fill the cache...
      _cache_host_instance[h.host_id] = h.poller_id;
    } else
      logging::error(logging::high) << "SQL: host '" << h.host_name
                                    << "' of poller " << h.poller_id
                                    << " has no ID";
    *p.second = true;
    _events.pop_front();
  }
}

void conflict_manager::_process_host_parent() {}
void conflict_manager::_process_host_status() {}

/**
 *  Process an instance event. The thread executing the command is controlled
 *  so that queries depending on this one will be made by the same thread.
 *
 *  @param[in] e Uncasted instance.
 */
void conflict_manager::_process_instance() {
  std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
  neb::instance& i(*static_cast<neb::instance*>(p.first.get()));

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

    _mysql.run_statement(_instance_insupdate,
                         oss.str(),
                         true,
                         _mysql.choose_connection_by_instance(i.poller_id));
  }

  /* We just have to set the boolean */
  *p.second = true;
  _events.pop_front();
}

void conflict_manager::_process_instance_status() {}
void conflict_manager::_process_log() {}
void conflict_manager::_process_module() {}
void conflict_manager::_process_service_check() {}
void conflict_manager::_process_service_dependency() {}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void conflict_manager::_process_service_group() {
  int conn = _mysql.choose_best_connection();

  while (true) {
    auto& p = _events.front();
    std::shared_ptr<io::data> d{p.first};

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
      }
    }
    *p.second = true;
    _events.pop_front();
  }
}

void conflict_manager::_process_service_group_member() {}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void conflict_manager::_process_service() {
  auto& p = _events.front();
  std::shared_ptr<io::data> d{p.first};

  // Processed object.
  neb::service const& s(*static_cast<neb::service const*>(d.get()));

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
  *p.second = true;
  _events.pop_front();
}

void conflict_manager::_process_service_status() {
  while (true) {
    std::pair<std::shared_ptr<io::data>, bool*>& p = _events.front();
    std::shared_ptr<io::data> d{p.first};
    uint32_t type{d->type()};

    if (type != neb::host_group::static_type())
      break;

  //neb::service_status& ss = *static_cast<neb::service_status*>(p.first.get());
  break;
  }
}

void conflict_manager::_process_instance_configuration() {}
void conflict_manager::_process_responsive_instance() {}
