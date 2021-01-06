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
 *  Here is the mysql connection manager. It creates, destroyes, configures
 * mysql_connections. It is used as a unique instance transparently through
 * the mysql class.
 *
 * The developer creates a mysql object with a little configuration. Through
 * this object, he can get connections to the database and then send queries
 * always through the mysql object.
 *
 * So from the developer point of view, only the mysql object is seen.
 *
 * A mysql object contains a configuration with user/password, database host,
 * a port, a number of queries per transaction. When it is constructed, it asks
 * the manager to get the good number of connections following its
 * configuration. The manager owns all the databases connections, if it already
 * has connections matching the mysql object, they are given to it. If some
 * miss, the manager creates them and keeps them in a vector _connection.
 *
 * The mysql object asks for its connections using the internal function
 * get_connections(const database_config& db_cfg) that returns a vector
 * containing the asked connections. Connections can be shared between several
 * mysql objects.
 *
 * When a connection is no more used, it is the manager work to destroy it.
 * This work of cleanup is done when a new mysql object is created, or when
 * a mysql object is destroyed.
 *
 * The manager works on the connections statistics. It centralizes the number of
 * tasks waiting on each connection.
 *
 */
class mysql_manager {
  mutable std::mutex _cfg_mutex;
  std::vector<std::shared_ptr<mysql_connection>> _connection;

  // last stats update timestamp
  time_t _stats_connections_timestamp;
  // Number of tasks per connection
  std::vector<int> _stats_counts;

  mysql_manager();

 public:
  ~mysql_manager();
  static mysql_manager& instance();
  std::vector<std::shared_ptr<mysql_connection>> get_connections(
      database_config const& db_cfg);
  std::map<std::string, std::string> get_stats();
  void update_connections();
  void clear();
};

CCB_END()

#endif  // CCB_MYSQL_MANAGER_HH
