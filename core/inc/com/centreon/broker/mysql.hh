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
#  include <future>
#  include "com/centreon/broker/mysql_thread.hh"

CCB_BEGIN()

/**
 *  @class mysql mysql.hh "com/centreon/broker/storage/mysql.hh"
 *  @brief Class managing the mysql connection
 *
 *  Here is a binding to the C MySQL connector.
 */
class                 mysql {
 public:
  enum                version {
    v2 = 2,
    v3
  };
                      mysql(database_config const& db_cfg);
                      ~mysql();
  void                prepare_statement(mysql_stmt const& stmt);
  mysql_stmt          prepare_query(std::string const& query,
                                    mysql_bind_mapping const& bind_mapping = mysql_bind_mapping());
  void                commit(int thread_id = -1);
  int                 run_query(
                        std::string const& query,
                        std::promise<mysql_result>* p = NULL,
                        std::string const& error_msg = "", bool fatal = false,
                        int thread = -1);
  int                 run_statement(
                        mysql_stmt& stmt,
                        std::promise<mysql_result>* p = NULL,
                        std::string const& error_msg = "", bool fatal = false,
                        int thread = -1);
  int                 run_statement_on_condition(
                        mysql_stmt& stmt,
                        std::promise<mysql_result>* p,
                        mysql_task::condition condition,
                        std::string const& error_msg, bool fatal,
                        int thread);
  //mysql_result        get_result(int thread_id, mysql_stmt const& stmt);
  bool                fetch_row(int thread_id, mysql_result& res);
  int                 get_last_insert_id(int thread_id);
  void                check_affected_rows(
                        int thread_id,
                        std::string const& message);
  void                check_affected_rows(
                        int thread_id,
                        mysql_stmt const& stmt,
                        std::string const& message);
  int                 get_affected_rows(int thread_id);
  int                 get_affected_rows(int thread_id, mysql_stmt const& stmt);
  bool                finish();
  version             schema_version() const;
  int                 connections_count() const;
  bool                commit_if_needed();

 private:
  static void         _initialize_mysql();
  void                _check_errors(int thread_id);
  static std::atomic_int
                      _count_ref;

  database_config     _db_cfg;
  int                 _pending_queries;

  std::vector<mysql_thread*>
                      _vector;
  version             _version;
  std::vector<mysql_thread*>
                      _thread;
  int                 _current_thread;
};

CCB_END()

#  endif  //CCB_MYSQL_HH
