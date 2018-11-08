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
  enum                   type {
                           RUN,
                           COMMIT,
                           PREPARE,
                           STATEMENT,
                           STATEMENT_ON_CONDITION,
                           LAST_INSERT_ID,
                           CHECK_AFFECTED_ROWS,
                           AFFECTED_ROWS,
                           RESULT,
                           FETCH_ROW,
                           FINISH,
  };

  enum                   condition {
                           ON_ERROR,
                           IF_PREVIOUS,
                           IF_NOT_PREVIOUS,
  };

  virtual                ~mysql_task() {}

                         mysql_task(type type)
                          : type(type) {}
  type                   type;
};

class                    mysql_task_run : public mysql_task {
 public:
                         mysql_task_run(
                           std::string const& q,
                           std::string const& error_msg, bool fatal)
                          : mysql_task(mysql_task::RUN),
                            query(q),
                            error_msg(error_msg),
                            fatal(fatal) {}
  std::string            query;
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

class                    mysql_task_result : public mysql_task {
 public:
                         mysql_task_result(mysql_result* result)
                          : mysql_task(mysql_task::RESULT),
                            result(result) {}
  mysql_result*          result;
};

class                    mysql_task_fetch : public mysql_task {
 public:
                         mysql_task_fetch(mysql_result* result)
                          : mysql_task(mysql_task::FETCH_ROW),
                            result(result) {}
  mysql_result*          result;
};

class                    mysql_task_check_affected_rows : public mysql_task {
 public:
                         mysql_task_check_affected_rows(
                             std::string const& message,
                             int statement_id = 0)
                          : mysql_task(mysql_task::CHECK_AFFECTED_ROWS),
                            message(message),
                            statement_id(statement_id) {}
  std::string            message;
  int                    statement_id;
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
                           mysql_stmt& stmt,
                           std::string const& error_msg,
                           bool fatal)
                          : mysql_task(mysql_task::STATEMENT),
                            statement_id(stmt.get_id()),
                            bind(stmt.get_bind()),
                            error_msg(error_msg),
                            fatal(fatal) {}
  int                    statement_id;
  std::shared_ptr<mysql_bind>
                         bind;
  std::string            error_msg;
  bool                   fatal;
};

class                    mysql_task_statement_on_condition : public mysql_task_statement {
 public:
                         mysql_task_statement_on_condition(
                           mysql_stmt& stmt, condition condition,
                           std::string const& error_msg,
                           bool fatal)
                          : mysql_task_statement(stmt, error_msg, fatal),
                            condition(condition) {
                             type = STATEMENT_ON_CONDITION;
                          }
  condition               condition;
};

CCB_END()

#endif  //CCB_MYSQL_TASK_HH
