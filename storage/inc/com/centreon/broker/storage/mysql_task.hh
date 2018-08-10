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

#include <mysql.h>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

typedef int (*mysql_callback)(MYSQL* conn);

namespace                  storage {
  class                    mysql_task {
   public:
    enum                   mysql_type {
                             RUN,
                             PREPARE,
                             STATEMENT,
                             FINISH,
    };

                           mysql_task(mysql_type type)
                            : type(type) {}
    mysql_type             type;
  };

  class                    mysql_task_run : public mysql_task {
   public:
                           mysql_task_run(
                             std::string const& q,
                             mysql_callback fn = static_cast<mysql_callback>(0))
                            : mysql_task(mysql_task::RUN), query(q), fn(fn) {}
    std::string            query;
    mysql_callback         fn;
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
                           mysql_task_statement(int statement_id, mysql_bind const& bind)
                            : mysql_task(mysql_task::STATEMENT), statement_id(statement_id), bind(bind) {}
    int                    statement_id;
    mysql_bind             bind;
  };
}

CCB_END()

#endif  //CCB_STORAGE_MYSQL_TASK_HH
