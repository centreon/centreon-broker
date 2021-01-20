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
#include <errmsg.h>

#include <cstring>
#include <sstream>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/exceptions/msg_fmt.hh"
#include "com/centreon/broker/config/applier/init.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

constexpr const char* mysql_error::msg[];

const int STR_SIZE = 200;
const int MAX_ATTEMPTS = 10;

void (mysql_connection::*const mysql_connection::_task_processing_table[])(
    mysql_task* task) = {
    &mysql_connection::_query,
    &mysql_connection::_query_res,
    &mysql_connection::_query_int,
    &mysql_connection::_commit,
    &mysql_connection::_prepare,
    &mysql_connection::_statement,
    &mysql_connection::_statement_res,
    &mysql_connection::_statement_int<int>,
    &mysql_connection::_statement_int<int64_t>,
    &mysql_connection::_statement_int<uint32_t>,
    &mysql_connection::_statement_int<uint64_t>,
    &mysql_connection::_fetch_row_sync,
    &mysql_connection::_finish,
};

/******************************************************************************/
/*                      Methods executed by this thread                       */
/******************************************************************************/

/**
 * @brief check if the error code is a server error. At the moment, we only
 * check two errors. Maybe we will need to add some.
 *
 * @param code the code to check
 *
 * @return a boolean telling if the error is fatal (a server error).
 */
bool mysql_connection::_server_error(int code) const {
  switch (code) {
    case CR_SERVER_GONE_ERROR:
    case CR_SERVER_LOST:
      return true;
    default:
      return false;
  }
}

void mysql_connection::_query(mysql_task* t) {
  mysql_task_run* task(static_cast<mysql_task_run*>(t));
  log_v2::sql()->debug("mysql_connection: run query: {}", task->query);
  if (mysql_query(_conn, task->query.c_str())) {
    const char* m = mysql_error::msg[task->error_code];
    std::string err_msg(fmt::format("{} {}", m, ::mysql_error(_conn)));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    if (task->fatal || _server_error(::mysql_errno(_conn)))
      set_error_message(err_msg);
  } else
    _need_commit = true;
}

void mysql_connection::_query_res(mysql_task* t) {
  mysql_task_run_res* task(static_cast<mysql_task_run_res*>(t));
  log_v2::sql()->debug("mysql_connection: run query: {}", task->query);
  if (mysql_query(_conn, task->query.c_str())) {
    std::string err_msg(::mysql_error(_conn));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    if (_server_error(mysql_errno(_conn)))
      set_error_message(err_msg);

    msg_fmt e(err_msg);
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
  } else {
    /* All is good here */
    _need_commit = true;
    task->promise->set_value(mysql_result(this, mysql_store_result(_conn)));
  }
}

void mysql_connection::_query_int(mysql_task* t) {
  mysql_task_run_int* task(static_cast<mysql_task_run_int*>(t));
  log_v2::sql()->debug("mysql_connection: run query: {}", task->query);
  if (mysql_query(_conn, task->query.c_str())) {
    std::string err_msg(::mysql_error(_conn));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    if (_server_error(::mysql_errno(_conn)))
      set_error_message(err_msg);

    msg_fmt e(err_msg);
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
  } else {
    /* All is good here */
    _need_commit = true;
    if (task->return_type == mysql_task::AFFECTED_ROWS)
      task->promise->set_value(mysql_affected_rows(_conn));
    else /* LAST_INSERT_ID */
      task->promise->set_value(mysql_insert_id(_conn));
  }
}

void mysql_connection::_commit(mysql_task* t) {
  mysql_task_commit* task(static_cast<mysql_task_commit*>(t));
  int32_t attempts = 0;
  int res;
  std::string err_msg;
  if (_need_commit) {
    log_v2::sql()->debug("mysql_connection: commit");
    while (attempts++ < MAX_ATTEMPTS && (res = mysql_commit(_conn))) {
      err_msg = ::mysql_error(_conn);
      if (_server_error(::mysql_errno(_conn))) {
        set_error_message(err_msg);
        break;
      }
      log_v2::sql()->error("mysql_connection: {}", err_msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  } else
    res = 0;

  if (res) {
    std::string err_msg(
        fmt::format("Error during commit: {}", ::mysql_error(_conn)));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    set_error_message(err_msg);
    if (--task->count == 0)
      task->promise->set_value(true);
  } else {
    /* No more queries are waiting for a commit now. */
    _need_commit = false;

    /* Commit is done on each connection. If task->count is 0, then we are on
     * the last one. It's time to release the future boolean. */
    if (--task->count == 0)
      task->promise->set_value(true);
  }
}

void mysql_connection::_prepare(mysql_task* t) {
  mysql_task_prepare* task(static_cast<mysql_task_prepare*>(t));
  if (_stmt[task->id]) {
    log_v2::sql()->info("mysql_connection: Statement already prepared: {} ({})",
                        task->id, task->query);
    return;
  }

  _stmt_query[task->id] = task->query;
  log_v2::sql()->debug("mysql_connection: prepare statement {}: {}", task->id,
                       task->query);
  MYSQL_STMT* stmt(mysql_stmt_init(_conn));
  if (!stmt)
    set_error_message("statement initialization failed: insuffisant memory");
  else {
    if (mysql_stmt_prepare(stmt, task->query.c_str(), task->query.size())) {
      std::string err_msg(::mysql_stmt_error(stmt));
      log_v2::sql()->error("mysql_connection: {}", err_msg);
      set_error_message(err_msg);
    } else
      _stmt[task->id] = stmt;
  }
}

void mysql_connection::_statement(mysql_task* t) {
  mysql_task_statement* task(static_cast<mysql_task_statement*>(t));
  log_v2::sql()->debug("mysql_connection: execute statement {}: {}",
                       task->statement_id, _stmt_query[task->statement_id]);
  MYSQL_STMT* stmt(_stmt[task->statement_id]);
  if (!stmt) {
    log_v2::sql()->error("mysql_connection: no statement to execute");
    set_error_message("statement {} not prepared", task->statement_id);
    return;
  }
  MYSQL_BIND* bb = nullptr;
  if (task->bind)
    bb = const_cast<MYSQL_BIND*>(task->bind->get_bind());

  if (bb && mysql_stmt_bind_param(stmt, bb)) {
    std::string err_msg(::mysql_stmt_error(stmt));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    if (task->fatal || _server_error(::mysql_stmt_errno(stmt)))
      set_error_message(err_msg);
    else {
      logging::error(logging::medium)
          << "mysql_connection: Error while binding values in statement: "
          << err_msg;
    }
  } else {
    int32_t attempts = 0;
    for (;;) {
      if (mysql_stmt_execute(stmt)) {
        std::string err_msg(fmt::format("{} {}",
                                        mysql_error::msg[task->error_code],
                                        ::mysql_stmt_error(stmt)));
        if (_server_error(::mysql_stmt_errno(stmt))) {
          set_error_message(err_msg);
          break;
        }
        if (mysql_stmt_errno(stmt) != 1213 &&
            mysql_stmt_errno(stmt) != 1205)  // Dead Lock error
          attempts = MAX_ATTEMPTS;

        mysql_commit(_conn);

        log_v2::sql()->error("mysql_connection: {}", err_msg);
        logging::error(logging::medium) << "mysql_connection: " << err_msg;
        if (++attempts >= MAX_ATTEMPTS) {
          if (task->fatal || _server_error(::mysql_stmt_errno(stmt)))
            set_error_message("{} {}", mysql_error::msg[task->error_code],
                              ::mysql_stmt_error(stmt));
          break;
        }
      } else {
        _need_commit = true;
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

void mysql_connection::_statement_res(mysql_task* t) {
  mysql_task_statement_res* task(static_cast<mysql_task_statement_res*>(t));
  log_v2::sql()->debug("mysql_connection: execute statement {}: {}",
                       task->statement_id, _stmt_query[task->statement_id]);
  MYSQL_STMT* stmt(_stmt[task->statement_id]);
  if (!stmt) {
    log_v2::sql()->error("mysql_connection: no statement to execute");
    msg_fmt e("statement not prepared");
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
    return;
  }
  MYSQL_BIND* bb(nullptr);
  if (task->bind)
    bb = const_cast<MYSQL_BIND*>(task->bind->get_bind());

  if (bb && mysql_stmt_bind_param(stmt, bb)) {
    std::string err_msg(::mysql_stmt_error(stmt));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    msg_fmt e(err_msg);
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
  } else {
    int32_t attempts = 0;
    for (;;) {
      if (mysql_stmt_execute(stmt)) {
        std::string err_msg(::mysql_stmt_error(stmt));
        if (_server_error(mysql_stmt_errno(stmt))) {
          set_error_message(err_msg);
          msg_fmt e(err_msg);
          task->promise->set_exception(
              std::make_exception_ptr<msg_fmt>(e));
          break;
        }
        if (mysql_stmt_errno(stmt) != 1213 &&
            mysql_stmt_errno(stmt) != 1205)  // Dead Lock error
          attempts = MAX_ATTEMPTS;

        mysql_commit(_conn);

        log_v2::sql()->error("mysql_connection: {}", err_msg);
        logging::error(logging::medium) << "mysql_connection: " << err_msg;
        if (++attempts >= MAX_ATTEMPTS) {
          msg_fmt e(err_msg);
          task->promise->set_exception(
              std::make_exception_ptr<msg_fmt>(e));
          break;
        }
      } else {
        _need_commit = true;
        mysql_result res(this, task->statement_id);
        MYSQL_STMT* stmt(_stmt[task->statement_id]);
        MYSQL_RES* prepare_meta_result(mysql_stmt_result_metadata(stmt));
        if (prepare_meta_result == nullptr) {
          if (mysql_stmt_errno(stmt)) {
            std::string err_msg(::mysql_stmt_error(stmt));
            if (_server_error(mysql_stmt_errno(stmt)))
              set_error_message(err_msg);
            msg_fmt e(err_msg);
            task->promise->set_exception(
                std::make_exception_ptr<msg_fmt>(e));
          } else
            task->promise->set_value(nullptr);
        } else {
          int size(mysql_num_fields(prepare_meta_result));
          std::unique_ptr<mysql_bind> bind(new mysql_bind(size, STR_SIZE));

          if (mysql_stmt_bind_result(stmt, bind->get_bind())) {
            std::string err_msg(::mysql_stmt_error(stmt));
            if (_server_error(::mysql_stmt_errno(stmt)))
              set_error_message(err_msg);
            msg_fmt e(err_msg);
            task->promise->set_exception(
                std::make_exception_ptr<msg_fmt>(e));
          } else {
            if (mysql_stmt_store_result(stmt)) {
              std::string err_msg(::mysql_stmt_error(stmt));
              if (_server_error(::mysql_stmt_errno(stmt)))
                set_error_message(err_msg);
              msg_fmt e(err_msg);
              task->promise->set_exception(
                  std::make_exception_ptr<msg_fmt>(e));
            }
            // Here, we have the first row.
            res.set(prepare_meta_result);
            bind->set_empty(true);
          }
          res.set_bind(move(bind));
          task->promise->set_value(std::move(res));
        }
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

template <typename T>
void mysql_connection::_statement_int(mysql_task* t) {
  mysql_task_statement_int<T>* task(
      static_cast<mysql_task_statement_int<T>*>(t));
  log_v2::sql()->debug("mysql_connection: execute statement {}: {}",
                       task->statement_id, _stmt_query[task->statement_id]);
  MYSQL_STMT* stmt(_stmt[task->statement_id]);
  if (!stmt) {
    log_v2::sql()->error("mysql_connection: no statement to execute");
    msg_fmt e("statement not prepared");
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
    return;
  }
  MYSQL_BIND* bb(nullptr);
  if (task->bind)
    bb = const_cast<MYSQL_BIND*>(task->bind->get_bind());

  if (bb && mysql_stmt_bind_param(stmt, bb)) {
    std::string err_msg(::mysql_stmt_error(stmt));
    log_v2::sql()->error("mysql_connection: {}", err_msg);
    msg_fmt e(err_msg);
    task->promise->set_exception(std::make_exception_ptr<msg_fmt>(e));
  } else {
    int32_t attempts = 0;
    for (;;) {
      if (mysql_stmt_execute(stmt)) {
        std::string err_msg(::mysql_stmt_error(stmt));
        if (_server_error(mysql_stmt_errno(stmt))) {
          set_error_message(err_msg);
          msg_fmt e(err_msg);
          task->promise->set_exception(
              std::make_exception_ptr<msg_fmt>(e));
          break;
        }
        if (mysql_stmt_errno(stmt) != 1213 &&
            mysql_stmt_errno(stmt) != 1205)  // Dead Lock error
          attempts = MAX_ATTEMPTS;

        mysql_commit(_conn);

        log_v2::sql()->error("mysql_connection: {}", err_msg);
        logging::error(logging::medium) << "mysql_connection: " << err_msg;
        if (++attempts >= MAX_ATTEMPTS) {
          msg_fmt e(err_msg);
          task->promise->set_exception(
              std::make_exception_ptr<msg_fmt>(e));
          break;
        }
      } else {
        _need_commit = true;
        if (task->return_type == mysql_task::AFFECTED_ROWS)
          task->promise->set_value(
              mysql_stmt_affected_rows(_stmt[task->statement_id]));
        else /* LAST_INSERT_ID */
          task->promise->set_value(
              mysql_stmt_insert_id(_stmt[task->statement_id]));
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
}

void mysql_connection::_fetch_row_sync(mysql_task* t) {
  mysql_task_fetch* task(static_cast<mysql_task_fetch*>(t));
  int stmt_id(task->result->get_statement_id());
  if (stmt_id) {
    MYSQL_STMT* stmt(_stmt[stmt_id]);
    int res(mysql_stmt_fetch(stmt));
    if (res != 0)
      task->result->get_bind()->set_empty(true);
    task->promise->set_value(res == 0);
  } else {
    MYSQL_ROW r(mysql_fetch_row(task->result->get()));
    task->result->set_row(r);
    task->promise->set_value(r != nullptr);
  }
}

/**
 * @brief If the connection has encountered an error, this method returns true.
 *
 * @return a boolean True on error, False otherwise.
 */
bool mysql_connection::is_in_error() const {
  return _error.is_active();
}

std::string mysql_connection::get_error_message() {
  std::lock_guard<std::mutex> lck(_error_m);
  return _error.get_message();
}

/**
 * @brief Disable the connection's error. Therefore, the connection is no more
 * in error.
 */
void mysql_connection::clear_error() {
  _error.clear();
}

void mysql_connection::_finish(mysql_task* t __attribute__((unused))) {
  _finished = true;
  _tasks_condition.notify_all();
}

bool mysql_connection::is_finished() const {
  return _finished;
}

std::string mysql_connection::_get_stack() {
  std::string retval;
  for (std::shared_ptr<mysql_task> t : _tasks_list) {
    switch (t->type) {
      case mysql_task::RUN:
        retval += "RUN ; ";
        break;
      case mysql_task::RUN_RES:
        retval += "RUN with Result; ";
        break;
      case mysql_task::RUN_INT:
        retval += "RUN with int return; ";
        break;
      case mysql_task::COMMIT:
        retval += "COMMIT ; ";
        break;
      case mysql_task::PREPARE:
        retval += "PREPARE ; ";
        break;
      case mysql_task::STATEMENT:
        retval += "STATEMENT ; ";
        break;
      case mysql_task::STATEMENT_RES:
        retval += "STATEMENT with result; ";
        break;
      case mysql_task::STATEMENT_INT:
        retval += "STATEMENT with int return; ";
        break;
      case mysql_task::STATEMENT_UINT:
        retval += "STATEMENT with uint return; ";
        break;
      case mysql_task::STATEMENT_INT64:
        retval += "STATEMENT with int64 return; ";
        break;
      case mysql_task::STATEMENT_UINT64:
        retval += "STATEMENT with uint64 return; ";
        break;
      case mysql_task::FETCH_ROW:
        retval += "FETCH_ROW ; ";
        break;
      case mysql_task::FINISH:
        retval += "FINISH ; ";
        break;
    }
  }
  return retval;
}

void mysql_connection::_run() {
  std::unique_lock<std::mutex> locker(_result_mutex);
  _conn = mysql_init(nullptr);
  if (!_conn) {
    set_error_message(::mysql_error(_conn));
    _state = finished;
    _result_condition.notify_all();
    return;
  } else {
    uint32_t timeout = 10;
    mysql_options(_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    while (config::applier::state != config::applier::finished &&
           !mysql_real_connect(_conn, _host.c_str(), _user.c_str(),
                               _pwd.c_str(), _name.c_str(), _port, nullptr,
                               CLIENT_FOUND_ROWS)) {
      set_error_message(fmt::format(
          "mysql_connection: The mysql/mariadb database seems not started. "
          "Waiting before attempt to connect again: {}",
          ::mysql_error(_conn)));
      _state = finished;
      _result_condition.notify_all();
      return;
    }
  }

  if (config::applier::state == config::applier::finished) {
    _state = finished;
    locker.unlock();
    _result_condition.notify_all();
  } else {
    mysql_set_character_set(_conn, "utf8mb4");

    if (_qps > 1)
      mysql_autocommit(_conn, 0);
    else
      mysql_autocommit(_conn, 1);

    _state = running;
    locker.unlock();
    _result_condition.notify_all();

    while (!_finished) {
      std::list<std::shared_ptr<database::mysql_task>> tasks_list;
      std::unique_lock<std::mutex> locker(_list_mutex);
      if (!_tasks_list.empty()) {
        std::swap(_tasks_list, tasks_list);
        locker.unlock();
      } else {
        _tasks_count = 0;
        _tasks_condition.wait(locker);
        continue;
      }
      for (auto task : tasks_list) {
        --_tasks_count;
        if (_task_processing_table[task->type])
          (this->*(_task_processing_table[task->type]))(task.get());
        else {
          logging::error(logging::medium)
              << "mysql_connection: Error type not managed...";
        }
      }
    }
    for (std::unordered_map<uint32_t, MYSQL_STMT*>::iterator it(_stmt.begin()),
         end(_stmt.end());
         it != end; ++it)
      mysql_stmt_close(it->second);
  }
  mysql_close(_conn);
  mysql_thread_end();
}

/******************************************************************************/
/*                    Methods executed by the main thread                     */
/******************************************************************************/

mysql_connection::mysql_connection(database_config const& db_cfg)
    : _conn(nullptr),
      _finished(false),
      _tasks_count(0),
      _need_commit(false),
      _host(db_cfg.get_host()),
      _user(db_cfg.get_user()),
      _pwd(db_cfg.get_password()),
      _name(db_cfg.get_name()),
      _port(db_cfg.get_port()),
      _state(not_started),
      _qps(db_cfg.get_queries_per_transaction()) {
  std::unique_lock<std::mutex> locker(_result_mutex);
  log_v2::sql()->info("mysql_connection: starting connection");
  _thread.reset(new std::thread(&mysql_connection::_run, this));
  _result_condition.wait(locker, [this] { return _state != not_started; });
  if (_state == finished) {
    _thread->join();
    throw msg_fmt("mysql_connection: error while starting connection");
  }
  log_v2::sql()->info("mysql_connection: connection started");
}

/**
 * @brief Destructor. When called, the finish task is post on the stack. So
 * the end will occur only when all the queries will be played.
 */
mysql_connection::~mysql_connection() {
  log_v2::sql()->info("mysql_connection: finished");
  finish();
  _thread->join();
}

void mysql_connection::_push(std::shared_ptr<mysql_task> const& q) {
  if (_finished)
    throw msg_fmt(
        "This connection is closed and does not accept any query");

  std::lock_guard<std::mutex> locker(_list_mutex);
  _tasks_list.push_back(q);
  ++_tasks_count;
  _tasks_condition.notify_all();
}

/**
 *  This method finishes to send current tasks and then commits. The commited
 * variable is then incremented of the queries committed count. This function is
 * called by mysql::commit whom goal is to commit on each of the connections.
 *  So, this last method waits all the commits to be done ; the semaphore is
 * there for that purpose.
 *
 *  @param[out] promise This promise is set when count == 0
 *  @param count The integer counting how many queries are committed.
 */
void mysql_connection::commit(std::promise<bool>* promise,
                              std::atomic_int& count) {
  _push(std::make_shared<mysql_task_commit>(promise, count));
}

void mysql_connection::prepare_query(int stmt_id, std::string const& query) {
  _push(std::make_shared<mysql_task_prepare>(stmt_id, query));
}

/**
 *  This method is used from the main thread to execute asynchronously a query.
 *  No exception is thrown in case of error since this query is made
 * asynchronously.
 *
 *  @param query The SQL query
 *  @param error_msg The error message to return in case of error.
 *  @param p A pointer to a promise.
 */
void mysql_connection::run_query(std::string const& query,
                                 my_error::code ec,
                                 bool fatal) {
  _push(std::make_shared<mysql_task_run>(query, ec, fatal));
}

void mysql_connection::run_query_and_get_result(
    std::string const& query,
    std::promise<mysql_result>* promise) {
  _push(std::make_shared<mysql_task_run_res>(query, promise));
}

void mysql_connection::run_query_and_get_int(std::string const& query,
                                             std::promise<int>* promise,
                                             mysql_task::int_type type) {
  _push(std::make_shared<mysql_task_run_int>(query, promise, type));
}

void mysql_connection::run_statement(database::mysql_stmt& stmt,
                                     my_error::code ec,
                                     bool fatal) {
  _push(std::make_shared<mysql_task_statement>(stmt, ec, fatal));
}

void mysql_connection::run_statement_and_get_result(
    database::mysql_stmt& stmt,
    std::promise<mysql_result>* promise) {
  _push(std::make_shared<mysql_task_statement_res>(stmt, promise));
}

void mysql_connection::finish() {
  _push(std::make_shared<mysql_task_finish>());
}

bool mysql_connection::fetch_row(mysql_result& result) {
  std::promise<bool> promise;
  _push(std::make_shared<mysql_task_fetch>(&result, &promise));
  return promise.get_future().get();
}

bool mysql_connection::match_config(database_config const& db_cfg) const {
  std::lock_guard<std::mutex> lock(_cfg_mutex);
  return db_cfg.get_host() == _host && db_cfg.get_user() == _user &&
         db_cfg.get_password() == _pwd && db_cfg.get_name() == _name &&
         db_cfg.get_port() == _port &&
         db_cfg.get_queries_per_transaction() == _qps;
}

int mysql_connection::get_tasks_count() const {
  return _tasks_count;
}
