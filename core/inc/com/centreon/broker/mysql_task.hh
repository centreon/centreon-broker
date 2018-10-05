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

#ifndef CCB_MYSQL_TASK_HH
#  define CCB_MYSQL_TASK_HH

#include <QAtomicInt>
#include <QSemaphore>
#include <mysql.h>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

typedef int (*mysql_callback)(MYSQL* conn, void* data);

class                    mysql_task {
 public:
  enum                   mysql_type {
                           RUN,
                           RUN_SYNC,
                           COMMIT,
                           PREPARE,
                           STATEMENT,
                           STATEMENT_SYNC,
                           FINISH,
                           LAST_INSERT_ID,
                           AFFECTED_ROWS,
  };
  virtual                ~mysql_task() {}

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
                           mysql_callback fn, void* data,
                           std::string const& error_msg, bool fatal)
                          : mysql_task(mysql_task::RUN),
                            query(q),
                            error_msg(error_msg),
                            fatal(fatal),
                            fn(fn) {}
  std::string            query;
  mysql_callback         fn;
  void*                  data;
  std::string            error_msg;
  bool                   fatal;
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

class                    mysql_task_last_insert_id : public mysql_task {
 public:
                         mysql_task_last_insert_id(int* id)
                          : mysql_task(mysql_task::LAST_INSERT_ID),
                            id(id) {}
  int*                   id;
};

class                    mysql_task_affected_rows : public mysql_task {
 public:
                         mysql_task_affected_rows(
                             int* count,
                             int statement_id = 0)
                          : mysql_task(mysql_task::AFFECTED_ROWS),
                            count(count),
                            statement_id(statement_id) {}
  int*                   count;
  int                    statement_id;
};

class                    mysql_task_finish : public mysql_task {
 public:
                         mysql_task_finish()
                          : mysql_task(mysql_task::FINISH) {}
};

class                    mysql_task_prepare : public mysql_task {
 public:
                         mysql_task_prepare(
                           unsigned int id,
                           std::string const& q)
                          : mysql_task(mysql_task::PREPARE),
                            id(id),
                            query(q) {}
  unsigned int           id;
  std::string            query;
};

class                    mysql_task_statement : public mysql_task {
 public:
                         mysql_task_statement(
                           int statement_id,
                           std::auto_ptr<mysql_bind> bind,
                           mysql_callback fn,
                           void* data,
                           std::string const& error_msg,
                           bool fatal)
                          : mysql_task(mysql_task::STATEMENT),
                            statement_id(statement_id),
                            bind(bind),
                            fn(fn),
                            data(data),
                            error_msg(error_msg),
                            fatal(fatal) {}
  int                    statement_id;
  std::auto_ptr<mysql_bind>
                         bind;
  mysql_callback         fn;
  void*                  data;
  std::string            error_msg;
  bool                   fatal;
};

class                    mysql_task_statement_sync : public mysql_task {
 public:
                         mysql_task_statement_sync(
                           int statement_id,
                           std::auto_ptr<mysql_bind> bind,
                           std::string const& error_msg)
                          : mysql_task(mysql_task::STATEMENT_SYNC),
                            statement_id(statement_id),
                            bind(bind),
                            error_msg(error_msg) {}
  int                    statement_id;
  std::auto_ptr<mysql_bind>
                         bind;
  std::string            error_msg;
};

CCB_END()

#endif  //CCB_MYSQL_TASK_HH
