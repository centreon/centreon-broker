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
#define CCB_MYSQL_CONNECTION_HH

#include <atomic>
#include <condition_variable>
#include <future>
#include <list>
#include <mutex>
#include <unordered_map>
#include "com/centreon/broker/database/mysql_result.hh"
#include "com/centreon/broker/database/mysql_stmt.hh"
#include "com/centreon/broker/database/mysql_task.hh"
#include "com/centreon/broker/database_config.hh"

CCB_BEGIN()

/**
 *  @class mysql_connection mysql_connection.hh
 * "com/centreon/broker/mysql_connection.hh"
 *  @brief Class representing a thread connected to the mysql server
 *
 */
class mysql_connection {
 public:
  /**************************************************************************/
  /*                  Methods executed by the main thread                   */
  /**************************************************************************/

  mysql_connection(database_config const& db_cfg);
  ~mysql_connection();

  void prepare_query(int id, std::string const& query);
  void commit(std::promise<bool>* promise, std::atomic_int& count);
  void run_query(std::string const& query,
                 std::string const& error_msg,
                 bool fatal);
  void run_query_and_get_result(std::string const& query,
                                std::promise<database::mysql_result>* promise);
  void run_query_and_get_int(std::string const& query,
                             std::promise<int>* promise,
                             database::mysql_task::int_type type);

  void run_statement(database::mysql_stmt& stmt,
                     std::string const& error_msg,
                     bool fatal);
  void run_statement_and_get_result(
      database::mysql_stmt& stmt,
      std::promise<database::mysql_result>* promise);

  template <typename T>
  void run_statement_and_get_int(database::mysql_stmt& stmt,
                                 std::promise<T>* promise,
                                 database::mysql_task::int_type type) {
    _push(std::make_shared<database::mysql_task_statement_int<T>>(stmt, promise, type));
  }

  void finish();
  bool fetch_row(database::mysql_result& result);
  mysql_bind_mapping get_stmt_mapping(int stmt_id) const;
  int get_stmt_size() const;
  bool match_config(database_config const& db_cfg) const;
  int get_tasks_count() const;
  bool is_finished() const;

 private:
  /**************************************************************************/
  /*                    Methods executed by this thread                     */
  /**************************************************************************/

  void _run();
  std::string _get_stack();
  void _query(database::mysql_task* t);
  void _query_res(database::mysql_task* t);
  void _query_int(database::mysql_task* t);
  void _commit(database::mysql_task* t);
  void _prepare(database::mysql_task* t);
  void _statement(database::mysql_task* t);
  void _statement_res(database::mysql_task* t);
  template <typename T>
  void _statement_int(database::mysql_task* t);
  void _get_result_sync(database::mysql_task* task);
  void _fetch_row_sync(database::mysql_task* task);
  void _finish(database::mysql_task* task);
  void _push(std::shared_ptr<database::mysql_task> const& q);
  void _debug(MYSQL_BIND* bind, uint32_t size);

  static void (mysql_connection::*const _task_processing_table[])(
      database::mysql_task* task);

  std::unique_ptr<std::thread> _thread;
  MYSQL* _conn;

  // Mutex and condition working on _tasks_list.
  std::mutex _list_mutex;
  std::condition_variable _tasks_condition;
  std::atomic<bool> _finished;
  std::list<std::shared_ptr<database::mysql_task> > _tasks_list;
  std::atomic_int _tasks_count;

  std::unordered_map<uint32_t, MYSQL_STMT*> _stmt;

  // FIXME DBR: to debug: Logs must be well implemented
  std::unordered_map<uint32_t, std::string> _stmt_query;

  // Mutex and condition working on result and error_msg.
  std::mutex _result_mutex;
  std::condition_variable _result_condition;

  // Mutex to access the configuration
  mutable std::mutex _cfg_mutex;
  std::string _host;
  std::string _user;
  std::string _pwd;
  std::string _name;
  int _port;
  bool _started;
  uint32_t _qps;
  bool _need_commit;
};

CCB_END()

#endif  // CCB_MYSQL_CONNECTION_HH
