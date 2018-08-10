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
#include <QMutex>
#include <QWaitCondition>
#include <mysql.h>
#include "com/centreon/broker/storage/mysql_bind.hh"
#include "com/centreon/broker/storage/mysql_task.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"

CCB_BEGIN()

namespace                  storage {

  /**
   *  @class mysql_thread mysql_thread.hh "com/centreon/broker/storage/mysql_thread.hh"
   *  @brief Class representing a thread connected to the mysql server
   *
   */
  class                    mysql_thread : public QThread {
   public:
                           mysql_thread(
                             std::string const& address,
                             std::string const& user,
                             std::string const& password,
                             std::string const& database,
                             int port);
                           ~mysql_thread();
    void                   prepare_query(std::string const& query);
    void                   run_query(std::string const& query);
    void                   run_statement(int statement_id, mysql_bind const& bind);
    void                   run_query_with_callback(
                             std::string const& query,
                             mysql_callback);
    void                   finish();

   private:
    void                   run();

    void                   _run(mysql_task_run* task);
    void                   _prepare(mysql_task_prepare* task);
    void                   _statement(mysql_task_statement* task);
    void                   _push(misc::shared_ptr<mysql_task> const& q);

    MYSQL*                 _conn;
    QMutex                 _list_mutex;
    QWaitCondition         _tasks_condition;
    bool                   _finished;

    std::list<misc::shared_ptr<mysql_task> >
                           _queries_list;
    std::vector<misc::shared_ptr<MYSQL_STMT> >
                           _stmt;
  };
}

CCB_END()

#endif  //CCB_STORAGE_MYSQL_THREAD_HH
