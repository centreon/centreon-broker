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

#ifndef CCB_STORAGE_MYSQL_TASK_HH
#  define CCB_STORAGE_MYSQL_TASK_HH

#include <QAtomicInt>
#include <QSemaphore>
#include <mysql.h>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

typedef int (*mysql_callback)(MYSQL* conn, void* data);

namespace                  storage {
  class                    mysql_task {
   public:
    enum                   mysql_type {
                             RUN,
                             RUN_SYNC,
                             COMMIT,
                             PREPARE,
                             STATEMENT,
                             FINISH,
    };

                           mysql_task(mysql_type type)
                            : type(type) {}
    mysql_type             type;
  };

  class                    mysql_task_run_sync : public mysql_task {
   public:
                           mysql_task_run_sync(std::string const& q)
                            : mysql_task(mysql_task::RUN_SYNC), query(q) {}
    std::string            query;
  };

  class                    mysql_task_run : public mysql_task {
   public:
                           mysql_task_run(
                             std::string const& q,
                             std::string const& error_msg,
                             mysql_callback fn = static_cast<mysql_callback>(0))
                            : mysql_task(mysql_task::RUN),
                              query(q),
                              error_msg(error_msg),
                              fn(fn) {}
    std::string            query;
    std::string            error_msg;
    mysql_callback         fn;
    void*                  data;
  };

  class                    mysql_task_commit : public mysql_task {
   public:
                           mysql_task_commit(
                             QSemaphore& sem,
                             QAtomicInt& count)
                            : mysql_task(mysql_task::COMMIT),
                              sem(sem),
                              count(count) {}
    QSemaphore&            sem;
    QAtomicInt&            count;
  };

  class                    mysql_task_finish : public mysql_task {
   public:
                           mysql_task_finish()
                            : mysql_task(mysql_task::FINISH) {}
  };

  class                    mysql_task_prepare : public mysql_task {
   public:
                           mysql_task_prepare(std::string const& q)
                            : mysql_task(mysql_task::PREPARE), query(q) {}
    std::string            query;
  };

  class                    mysql_task_statement : public mysql_task {
   public:
                           mysql_task_statement(
                             int statement_id,
                             mysql_bind const& bind,
                             mysql_callback fn,
                             void* data)
                            : mysql_task(mysql_task::STATEMENT),
                              statement_id(statement_id),
                              bind(bind),
                              fn(fn),
                              data(data) {}
    int                    statement_id;
    mysql_bind             bind;
    mysql_callback         fn;
    void*                  data;
  };
}

CCB_END()

#endif  //CCB_STORAGE_MYSQL_TASK_HH
