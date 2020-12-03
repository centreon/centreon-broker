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
#ifndef CCB_MYSQL_HH
#define CCB_MYSQL_HH

#include <atomic>

#include "com/centreon/broker/database/mysql_error.hh"
#include "com/centreon/broker/mysql_connection.hh"

CCB_BEGIN()

using my_error = database::mysql_error;
/**
 *  @class mysql mysql.hh "com/centreon/broker/storage/mysql.hh"
 *  @brief Class managing the mysql connection
 *
 *  Here is a binding to the C MySQL connector.
 */
class mysql {
 public:
  mysql(database_config const& db_cfg);
  ~mysql();
  void prepare_statement(database::mysql_stmt const& stmt);
  database::mysql_stmt prepare_query(
      std::string const& query,
      mysql_bind_mapping const& bind_mapping = mysql_bind_mapping());
  void commit(int thread_id = -1);
  int run_query(std::string const& query,
                my_error::code ec = my_error::empty,
                bool fatal = false,
                int thread = -1);
  int run_query_and_get_result(std::string const& query,
                               std::promise<database::mysql_result>* promise,
                               int thread = -1);
  int run_query_and_get_int(std::string const& query,
                            std::promise<int>* promise,
                            database::mysql_task::int_type type,
                            int thread = -1);

  int run_statement(database::mysql_stmt& stmt,
                    my_error::code ec = my_error::empty,
                    bool fatal = false,
                    int thread_id = -1);

  int run_statement_and_get_result(
      database::mysql_stmt& stmt,
      std::promise<database::mysql_result>* promise,
      int thread_id = -1);

  template <typename T>
  int run_statement_and_get_int(database::mysql_stmt& stmt,
                                std::promise<T>* promise,
                                database::mysql_task::int_type type,
                                int thread_id = -1) {
    _check_errors();
    if (thread_id < 0)
      // Here, we use _current_thread
      thread_id = choose_best_connection(-1);

    _connection[thread_id]->run_statement_and_get_int<T>(stmt, promise, type);
    return thread_id;
  }

  bool fetch_row(database::mysql_result& res);
  int get_last_insert_id(int thread_id);
  int connections_count() const;
  bool commit_if_needed();
  int choose_connection_by_name(std::string const& name);
  int choose_connection_by_instance(int instance_id) const;
  int choose_best_connection(int32_t type);

 private:
  static void _initialize_mysql();
  void _check_errors();

  static std::atomic_int _count_ref;

  database_config _db_cfg;
  int _pending_queries;

  std::vector<std::shared_ptr<mysql_connection>> _connection;
  int _current_connection;
  std::unordered_map<std::string, int> _connection_by_name;
};

CCB_END()

#endif  // CCB_MYSQL_HH
