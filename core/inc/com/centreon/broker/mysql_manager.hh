/*
** Copyright 2018 Centreon
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
#ifndef CCB_MYSQL_MANAGER_HH
#define CCB_MYSQL_MANAGER_HH

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include "com/centreon/broker/database/mysql_error.hh"
#include "com/centreon/broker/mysql.hh"

CCB_BEGIN()

/**
 *  @class mysql_manager mysql_manager.hh
 * "com/centreon/broker/storage/mysql_manager.hh"
 *  @brief Class managing the mysql thread connections
 *
 *  Here is a mysql threads manager. It creates, destroyes, configures
 * mysql_connections.
 */
class mysql_manager {
 public:
  static mysql_manager& instance();
  std::vector<std::shared_ptr<mysql_connection>> get_connections(
      database_config const& db_cfg);
  bool commit_if_needed();
  bool is_in_error() const;
  void clear_error();
  database::mysql_error get_error();
  void set_error(std::string const& message);
  std::map<std::string, std::string> get_stats();
  void update_connections();
  void clear();

 private:
  mysql_manager();
  ~mysql_manager();
  static mysql_manager _singleton;
  mutable std::mutex _cfg_mutex;
  std::vector<std::shared_ptr<mysql_connection>> _connection;

  int _current_thread;
  std::atomic<mysql::version> _version;

  mutable std::mutex _err_mutex;
  database::mysql_error _error;

  // last stats update timestamp
  time_t _stats_connections_timestamp;
  // Number of tasks per connection
  std::vector<int> _stats_counts;
};

CCB_END()

#endif  // CCB_MYSQL_MANAGER_HH
