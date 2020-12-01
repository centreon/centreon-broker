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
