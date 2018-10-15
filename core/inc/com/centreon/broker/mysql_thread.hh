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

#ifndef CCB_MYSQL_THREAD_HH
#  define CCB_MYSQL_THREAD_HH

#include <QThread>
#include <QWaitCondition>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/mysql_error.hh"
#include "com/centreon/broker/mysql_result.hh"
#include "com/centreon/broker/mysql_stmt.hh"
#include "com/centreon/broker/mysql_task.hh"

CCB_BEGIN()

/**
 *  @class mysql_thread mysql_thread.hh "com/centreon/broker/mysql_thread.hh"
 *  @brief Class representing a thread connected to the mysql server
 *
 */
class                    mysql_thread : public QThread {
 public:

  /**************************************************************************/
  /*                  Methods executed by the main thread                   */
  /**************************************************************************/

                         mysql_thread(database_config const& db_cfg);
                         ~mysql_thread();

  void                   prepare_query(int id, std::string const& query);
  void                   commit(
                           QSemaphore& sem,
                           QAtomicInt& count);
  void                   run_query(
                           std::string const& query,
                           std::string const& error_msg, bool fatal);
  void                   run_statement(
                           mysql_stmt& stmt,
                           std::string const& error_msg, bool fatal);
  void                   run_statement_on_condition(
                           mysql_stmt& stmt, mysql_task::condition conditiond,
                           std::string const& error_msg, bool fatal);
  void                   finish();
  mysql_error            get_error();
  int                    get_last_insert_id();
  mysql_result           get_result(int statement_id = 0);
  bool                   fetch_row(mysql_result& result);
  int                    get_affected_rows(int statement_id = 0);
  void                   check_affected_rows(
                           std::string const& message,
                           int statement_id = 0);
  mysql_bind_mapping     get_stmt_mapping(int stmt_id) const;
  int                    get_stmt_size() const;

 private:

  /**************************************************************************/
  /*                    Methods executed by this thread                     */
  /**************************************************************************/

  void                   run();

  void                   _run(mysql_task* t);
  void                   _commit(mysql_task* t);
  void                   _prepare(mysql_task* t);
  void                   _statement(mysql_task* t);
  void                   _statement_on_condition(mysql_task* t);
  void                   _get_last_insert_id_sync(mysql_task* task);
  void                   _check_affected_rows(mysql_task* task);
  void                   _get_affected_rows_sync(mysql_task* task);
  void                   _get_result_sync(mysql_task* task);
  void                   _fetch_row_sync(mysql_task* task);
  void                   _finish(mysql_task* task);
  void                   _push(misc::shared_ptr<mysql_task> const& q);

  static void (mysql_thread::* const _task_processing_table[])(mysql_task* task);

  MYSQL*                 _conn;

  // Mutex and condition working on _tasks_list.
  QMutex                 _list_mutex;
  QWaitCondition         _tasks_condition;
  bool                   _finished;
  std::list<misc::shared_ptr<mysql_task> >
                         _tasks_list;
  umap<unsigned int, MYSQL_STMT*>
                         _stmt;

  // Mutex and condition working on result and error_msg.
  QMutex                 _result_mutex;
  QWaitCondition         _result_condition;

  mysql_error            _error;
  bool                   _previous;
  std::string            _host;
  std::string            _user;
  std::string            _pwd;
  std::string            _name;
  int                    _port;
  int                    _qps;
};

CCB_END()

#endif  //CCB_MYSQL_THREAD_HH
