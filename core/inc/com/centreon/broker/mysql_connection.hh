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

#ifndef CCB_MYSQL_CONNECTION_HH
#  define CCB_MYSQL_CONNECTION_HH

#  include <atomic>
#  include <condition_variable>
#  include <future>
#  include <list>
#  include <mutex>
#  include "com/centreon/broker/database_config.hh"
//#  include "com/centreon/broker/mysql_error.hh"
#  include "com/centreon/broker/mysql_result.hh"
#  include "com/centreon/broker/mysql_stmt.hh"
#  include "com/centreon/broker/mysql_task.hh"

CCB_BEGIN()

/**
 *  @class mysql_connection mysql_connection.hh "com/centreon/broker/mysql_connection.hh"
 *  @brief Class representing a thread connected to the mysql server
 *
 */
class                    mysql_connection {
 public:

  /**************************************************************************/
  /*                  Methods executed by the main thread                   */
  /**************************************************************************/

                          mysql_connection(database_config const& db_cfg);
                          ~mysql_connection();

  void                    prepare_query(int id, std::string const& query);
  void                    commit(
                            QSemaphore& sem,
                            std::atomic_int& count);
  void                    run_query(
                            std::string const& query,
                            std::promise<mysql_result>* p,
                            std::string const& error_msg, bool fatal);
  void                    run_statement(
                            mysql_stmt& stmt,
                            std::promise<mysql_result>* p,
                            std::string const& error_msg, bool fatal);
  void                    run_statement_on_condition(
                            mysql_stmt& stmt,
                            std::promise<mysql_result>* p,
                            mysql_task::condition conditiond,
                            std::string const& error_msg, bool fatal);
  void                    finish();
  int                     get_last_insert_id();
  bool                    fetch_row(mysql_result& result);
  int                     get_affected_rows(int statement_id = 0);
  void                    check_affected_rows(
                            std::string const& message,
                            int statement_id = 0);
  mysql_bind_mapping      get_stmt_mapping(int stmt_id) const;
  int                     get_stmt_size() const;
  bool                    match_config(database_config const& db_cfg) const;

 private:

  /**************************************************************************/
  /*                    Methods executed by this thread                     */
  /**************************************************************************/

  void                    _run();
  void                    _query(mysql_task* t);
  void                    _commit(mysql_task* t);
  void                    _prepare(mysql_task* t);
  void                    _statement(mysql_task* t);
  void                    _statement_on_condition(mysql_task* t);
  void                    _get_last_insert_id_sync(mysql_task* task);
  void                    _check_affected_rows(mysql_task* task);
  void                    _get_affected_rows_sync(mysql_task* task);
  void                    _get_result_sync(mysql_task* task);
  void                    _fetch_row_sync(mysql_task* task);
  void                    _finish(mysql_task* task);
  void                    _push(std::shared_ptr<mysql_task> const& q);

  static void (mysql_connection::* const _task_processing_table[])(mysql_task* task);

  std::unique_ptr<std::thread>
                          _thread;
  MYSQL*                  _conn;

  // Mutex and condition working on _tasks_list.
  std::mutex              _list_mutex;
  std::condition_variable _tasks_condition;
  bool                    _finished;
  std::list<std::shared_ptr<mysql_task> >
                          _tasks_list;
  std::unordered_map<unsigned int, MYSQL_STMT*>
                          _stmt;

  // Mutex and condition working on result and error_msg.
  std::mutex              _result_mutex;
  std::condition_variable _result_condition;

  bool                    _previous;

  // Mutex to access the configuration
  mutable std::mutex      _cfg_mutex;
  std::string             _host;
  std::string             _user;
  std::string             _pwd;
  std::string             _name;
  int                     _port;
  int                     _qps;
};

CCB_END()

#endif  //CCB_MYSQL_CONNECTION_HH
