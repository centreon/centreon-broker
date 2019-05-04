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
#  define CCB_MYSQL_HH

#  include <atomic>
#  include "com/centreon/broker/mysql_connection.hh"

CCB_BEGIN()

/**
 *  @class mysql mysql.hh "com/centreon/broker/storage/mysql.hh"
 *  @brief Class managing the mysql connection
 *
 *  Here is a binding to the C MySQL connector.
 */
class                   mysql {
 public:
  enum                  version {
    v2 = 2,
    v3
  };
                        mysql(database_config const& db_cfg);
                        ~mysql();
  void                  prepare_statement(database::mysql_stmt const& stmt);
  database::mysql_stmt  prepare_query(std::string const& query,
                          mysql_bind_mapping const& bind_mapping = mysql_bind_mapping());
  void                  commit(int thread_id = -1);
  int                   run_query(
                          std::string const& query,
                          std::string const& error_msg = "", bool fatal = false,
                          int thread = -1);
  int                   run_query_and_get_result(
                          std::string const& query,
                          std::promise<database::mysql_result>* promise,
                          int thread = -1);
  int                   run_query_and_get_int(
                          std::string const& query,
                          std::promise<int>* promise, database::mysql_task::int_type type,
                          int thread = -1);

  int                   run_statement(
                          database::mysql_stmt& stmt,
                          std::string const& error_msg = "", bool fatal = false,
                          int thread_id = -1);

  int                   run_statement_and_get_result(
                          database::mysql_stmt& stmt,
                          std::promise<database::mysql_result>* promise,
                          int thread_id = -1);

  int                   run_statement_and_get_int(
                          database::mysql_stmt& stmt,
                          std::promise<int>* promise, database::mysql_task::int_type type,
                          int thread_id = -1);

  bool                  fetch_row(database::mysql_result& res);
  int                   get_last_insert_id(int thread_id);
  version               schema_version() const;
  int                   connections_count() const;
  bool                  commit_if_needed();
  int                   choose_connection_by_name(std::string const& name);
  int                   choose_connection_by_instance(int instance_id) const;

 private:
  static void           _initialize_mysql();
  void                  _check_errors();
  int                   _choose_best_connection();

  static std::atomic_int
                        _count_ref;

  database_config       _db_cfg;
  int                   _pending_queries;

  std::vector<std::shared_ptr<mysql_connection>>
                        _connection;
  version               _version;
  int                   _current_connection;
  std::unordered_map<std::string, int>
                        _connection_by_name;
};

CCB_END()

#  endif  //CCB_MYSQL_HH
