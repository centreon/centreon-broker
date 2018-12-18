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

#include <atomic>
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
                           LAST_INSERT_ID,
                           CHECK_AFFECTED_ROWS,
                           AFFECTED_ROWS,
                           FETCH_ROW,
                           FINISH,
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
                            fatal(fatal),
                            promise(NULL) {}
                         mysql_task_run(
                           std::string const& q,
                           std::string const& error_msg, bool fatal,
                           std::promise<mysql_result>* p)
                          : mysql_task(mysql_task::RUN),
                            query(q),
                            error_msg(error_msg),
                            fatal(fatal),
                            promise(p) {}
  std::string            query;
  std::string            error_msg;
  bool                   fatal;
  std::promise<mysql_result>*
                         promise;
};

class                    mysql_task_commit : public mysql_task {
 public:
                         mysql_task_commit(
                           std::promise<bool>* promise,
                           std::atomic_int& count)
                          : mysql_task(mysql_task::COMMIT),
                            promise(promise),
                            count(count) {}
  std::promise<bool>*    promise;
  std::atomic_int&       count;
};

class                    mysql_task_last_insert_id : public mysql_task {
 public:
                         mysql_task_last_insert_id(std::promise<int>* promise)
                          : mysql_task(mysql_task::LAST_INSERT_ID),
                            promise(promise) {}
  std::promise<int>*     promise;
};

class                    mysql_task_fetch : public mysql_task {
 public:
                         mysql_task_fetch(mysql_result* result, std::promise<bool>* promise)
                          : mysql_task(mysql_task::FETCH_ROW),
                            result(result),
                            promise(promise) {}
  mysql_result*          result;
  std::promise<bool>*    promise;
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
                             std::promise<int>* promise,
                             int statement_id = 0)
                          : mysql_task(mysql_task::AFFECTED_ROWS),
                            promise(promise),
                            statement_id(statement_id) {}
  std::promise<int>*     promise;
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
                           std::promise<mysql_result>* promise,
                           std::string const& error_msg,
                           bool fatal)
                          : mysql_task(mysql_task::STATEMENT),
                            promise(promise),
                            statement_id(stmt.get_id()),
                            param_count(stmt.get_param_count()),
                            array_size(stmt.get_array_size()),
                            bind(stmt.get_bind()),
                            error_msg(error_msg),
                            fatal(fatal) {}
  int                    statement_id;
  std::promise<mysql_result>*
                         promise;
  int                    param_count;
  int                    array_size;
  std::unique_ptr<database::mysql_bind>
                         bind;
  std::string            error_msg;
  bool                   fatal;
};

CCB_END()

#endif  //CCB_MYSQL_TASK_HH
