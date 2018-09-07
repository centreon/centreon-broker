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

#ifndef CCB_STORAGE_MYSQL_THREAD_HH
#  define CCB_STORAGE_MYSQL_THREAD_HH

#include <QThread>
#include <QWaitCondition>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/storage/mysql_bind.hh"
#include "com/centreon/broker/storage/mysql_error.hh"
#include "com/centreon/broker/storage/mysql_result.hh"
#include "com/centreon/broker/storage/mysql_task.hh"

CCB_BEGIN()

namespace                  storage {

  /**
   *  @class mysql_thread mysql_thread.hh "com/centreon/broker/storage/mysql_thread.hh"
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

    void                   prepare_query(std::string const& query);
    void                   commit(
                             QSemaphore& sem,
                             QAtomicInt& count);
    void                   run_query(
                             std::string const& query,
                             mysql_callback fn, void* data,
                             std::string const& error_msg, bool fatal);
    void                   run_query_sync(
                             std::string const& query,
                             std::string const& error_msg);
    void                   run_statement(
                             int statement_id, mysql_bind const& bind,
                             mysql_callback fn, void* data,
                             std::string const& error_msg, bool fatal);
    void                   finish();
    mysql_result           get_result();
    mysql_error            get_error();

   private:

    /**************************************************************************/
    /*                    Methods executed by this thread                     */
    /**************************************************************************/

    void                   run();

    void                   _commit(mysql_task_commit* task);
    void                   _run(mysql_task_run* task);
    void                   _run_sync(mysql_task_run_sync* task);
    void                   _prepare(mysql_task_prepare* task);
    void                   _statement(mysql_task_statement* task);
    void                   _push(misc::shared_ptr<mysql_task> const& q);

    MYSQL*                 _conn;

    // Mutex and condition working on _tasks_list.
    QMutex                 _list_mutex;
    QWaitCondition         _tasks_condition;
    bool                   _finished;
    std::list<misc::shared_ptr<mysql_task> >
                           _tasks_list;
    std::vector<MYSQL_STMT*>
                           _stmt;

    // Mutex and condition working on _result and _error_msg.
    QMutex                 _result_mutex;
    QWaitCondition         _result_condition;

    // Result of a mysql query. It is used in the case of _run_sync() calls.
    MYSQL_RES*             _result;

    // Error message returned when the call to _run_sync() fails.
    mysql_error            _error;
  };
}

CCB_END()

#endif  //CCB_STORAGE_MYSQL_THREAD_HH
