/*
** Copyright 2018-2020 Centreon
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

#ifndef CCB_MYSQL_ERROR_HH
#define CCB_MYSQL_ERROR_HH

#include <fmt/format.h>

#include <atomic>
#include <string>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace database {
/**
 *  @class mysql_thread mysql_thread.hh
 * "com/centreon/broker/storage/mysql_thread.hh"
 *  @brief Class representing a thread connected to the mysql server
 *
 */
class mysql_error {
 public:
  enum code {
    empty,
    clean_hosts_services,
    clean_hostgroup_members,
    clean_servicegroup_members,
    clean_empty_hostgroups,
    clean_empty_servicegroups,
    clean_host_dependencies,
    clean_service_dependencies,
    clean_host_parents,
    clean_modules,
    clean_downtimes,
    clean_comments,
    clean_customvariables,
    restore_instances,
    update_customvariables,
    update_logs,
    update_metrics,
    insert_data,
    delete_metric,
    delete_index,
    flag_index_data,
    delete_hosts,
    delete_modules,
    update_index_state,
    delete_availabilities,
    insert_availability,
    rebuild_ba,
    close_event,
    close_ba_events,
    close_kpi_events,
    delete_ba_durations,
    store_host_state,
    store_acknowledgement,
    store_comment,
    remove_customvariable,
    store_customvariable,
    store_downtime,
    store_eventhandler,
    store_flapping,
    store_host_check,
    store_host_dependency,
    store_host_group,
    store_host_group_member,
    delete_host_group_member,
    store_host,
    store_host_parentship,
    store_host_status,
    store_poller,
    update_poller,
    store_module,
    store_service_check_command,
    store_service_dependency,
    store_service_group,
    store_service_group_member,
    delete_service_group_member,
    store_service,
    store_service_status,
    update_ba,
    update_kpi,
    update_kpi_event,
    insert_kpi_event,
    insert_ba,
    insert_bv,
    insert_dimension_ba_bv,
    truncate_dimension_table,
    insert_dimension_kpi,
    insert_timeperiod,
    insert_timeperiod_exception,
    insert_exclusion_timeperiod,
    insert_relation_ba_timeperiod,
  };
  static constexpr const char* msg[]{
      "error: ",
      "could not clean hosts and services tables: ",
      "could not clean host groups memberships table: ",
      "could not clean service groups memberships table: ",
      "could not remove empty host groups: ",
      "could not remove empty service groups: ",
      "could not clean host dependencies table: ",
      "could not clean service dependencies table: ",
      "could not clean host parents table: ",
      "could not clean modules table: ",
      "could not clean downtimes table: ",
      "could not clean comments table: ",
      "could not clean custom variables table: ",
      "could not restore outdated instance: ",
      "could not store custom variables correctly: ",
      "could not store logs correctly: ",
      "could not update metrics: ",
      "could not insert data in data_bin: ",
      "could not delete metric: ",
      "could not delete index: ",
      "could not flag the index_data table to delete outdated entries: ",
      "could not delete outdated entries from the hosts table: ",
      "could not delete outdated entries from the modules table: ",
      "cannot update state of index: ",
      "availability thread could not delete the BA availabilities from the "
      "reporting database: ",
      "availability thread could not insert an availability: ",
      "could not update the list of BAs to rebuild: ",
      "could not close inconsistent event: ",
      "could not close all ba events: ",
      "could not close all kpi events: ",
      "could not delete BA durations: ",
      "could not store host state event: ",
      "could not store acknowledgement: ",
      "could not store comment: ",
      "could not remove custom variable: ",
      "could not store custom variable: ",
      "could not store downtime: ",
      "could not store event handler: ",
      "could not store flapping status: ",
      "could not store host check: ",
      "could not store host dependency: ",
      "could not store host group: ",
      "could not store host group membership: ",
      "could not delete membership of host to host group: ",
      "could not store host: ",
      "could not store host parentship: ",
      "could not store host status: ",
      "could not store poller: ",
      "could not update poller: ",
      "could not store module: ",
      "could not store service check command: ",
      "could not store service dependency: ",
      "could not store service group: ",
      "could not store service group membership: ",
      "could not delete membersjip of service to service group: ",
      "could not store service: ",
      "could not store service status: ",
      "could not update BA: ",
      "could not update KPI: ",
      "could not update kpi event: ",
      "could not insert kpi event: ",
      "could not insert BA: ",
      "could not insert BV: ",
      "could not insert dimension of BA-BV relation: ",
      "could not truncate some dimension table: ",
      "could not insert dimension of KPI: ",
      "could not insert timeperiod: ",
      "could not insert exception of timeperiod: ",
      "could not insert exclusion of timeperiod: ",
      "could not insert relation of BA to timeperiod: ",
  };

  mysql_error() : _active(false) {}
  mysql_error(mysql_error const& other) = delete;
  mysql_error(mysql_error&& other) = delete;
  mysql_error(char const* message) : _message(message), _active(true) {}
  mysql_error& operator=(mysql_error const& other) = delete;
  std::string get_message() { return std::move(_message); }

  template <typename... Args>
  void set_message(std::string const& format, const Args&... args) {
    _message = fmt::format(format, args...);
    _active = true;
  }
  void clear() { _active = false; }
  bool is_active() const { return _active; }

 private:
  std::string _message;
  std::atomic<bool> _active;
};

}  // namespace database

CCB_END()

#endif  // CCB_MYSQL_ERROR_HH
