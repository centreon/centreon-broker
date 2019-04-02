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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

std::once_flag init_flag;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
mysql::mysql(database_config const& db_cfg)
  : _db_cfg(db_cfg),
    _pending_queries(0),
    _version(mysql::v2),
    _current_connection(0) {

  mysql_manager& mgr(mysql_manager::instance());
  _connection = mgr.get_connections(db_cfg);

  try {
    std::promise<mysql_result> promise;
    run_query_and_get_result(
      "SELECT instance_id FROM instances LIMIT 1",
      &promise,
      "");
    _version = v2;
    promise.get_future().get();
    logging::info(logging::low)
      << "mysql: database is using version 2 of Centreon schema";
  }
  catch (std::exception const& e) {
    logging::info(logging::low)
      << "mysql: database is using version 3 of Centreon schema";
  }
}

/**
 *  Destructor
 */
mysql::~mysql() {
  //bool retval(finish());
  //if (!retval)
  //  logging::error(logging::medium)
  //    << "mysql: A thread was forced to stop after a timeout of 20s";
  mysql_manager::instance().update_connections();
}

/**
 *  Commit all pending queries of all the threads.
 *  This function waits for all the threads to be committed.
 *
 *  @param thread_id: -1 to commit all the threads or the index of the thread to
 *                    commit.
 *  If an error occures, an exception is thrown.
 */
void mysql::commit(int thread_id) {
  std::promise<bool> promise;
  std::future<bool> future(promise.get_future());
  std::atomic_int ko;
  if (thread_id < 0) {
    ko = _connection.size();
    for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
           it(_connection.begin()),
           end(_connection.end());
         it != end;
         ++it) {
      (*it)->commit(&promise, ko);
    }
  }
  else {
    ko = 1;
    _connection[thread_id]->commit(&promise, ko);
  }

  try {
    if (future.get())
      _pending_queries = 0;
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "mysql: Unable to commit transactions: " << e.what();
  }
}

bool mysql::fetch_row(mysql_result& res) {
  _check_errors();
  return res.get_connection()->fetch_row(res);
}

/**
 *  This method commits only if the max queries per transaction is reached.
 *
 * @return true if a commit has been done, false otherwise.
 */
bool mysql::commit_if_needed() {
  bool retval(false);
  int qpt(_db_cfg.get_queries_per_transaction());
  if (qpt > 1) {
    ++_pending_queries;
    if (_pending_queries >= qpt) {
      commit();
      retval = true;
    }
  }
  return retval;
}

/**
 *  Checks for previous errors. As queries are made asynchronously, errors
 *  may arise after the calls. This function is generally called just before
 *  adding a new query and throw an exception if an error is active.
 *
 */
void mysql::_check_errors() {
  if (mysql_manager::instance().is_in_error()) {
    database::mysql_error err(mysql_manager::instance().get_error());
    if (err.is_fatal())
      throw exceptions::msg() << err.get_message();
    else
      logging::error(logging::medium)
        << "mysql: " << err.get_message();
  }
}

int mysql::_get_best_connection() {
  /* We work with _current_connection to avoid always working with the same
   * connections. */
  int retval(_current_connection);
  int task_count(std::numeric_limits<int>::max());
  int count(_connection.size());
  for (int i(0); i < count; i++) {
    ++_current_connection;
    if (_current_connection >= count)
      _current_connection = 0;
    if (_connection[_current_connection]->get_tasks_count() < task_count) {
      retval = _current_connection;
      task_count = _connection[_current_connection]->get_tasks_count();
    }
  }
  return retval;
}

/**
 *  The simplest way to execute a query. Only the first arg is needed.
 *
 * @param query The query to execute.
 * @param error_msg An error message to complete the error message returned
 *                  by the mysql connector.
 * @param fatal A boolean telling if the error is fatal. In that case, an
 *              exception will be thrown if the error occures.
 * @param thread A thread id or 0 to keep the library choosing which one.
 * @param p A pointer to a promise or NULL. If it exists, it will be filled
 *        with the result of the query.
 *
 * @return The thread id that executed the query.
 */
int mysql::run_query(std::string const& query,
             std::string const& error_msg, bool fatal,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_query(
    query,
    error_msg, fatal);
  return thread_id;
}

int mysql::run_query_and_get_result(
             std::string const& query,
             std::promise<mysql_result>* promise,
             std::string const& error_msg,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_query_and_get_result(
                            query,
                            promise,
                            error_msg);
  return thread_id;
}

int mysql::run_query_and_get_int(
             std::string const& query,
             std::promise<int>* promise, mysql_task::int_type type,
             std::string const& error_msg,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_query_and_get_int(
                            query,
                            promise, type,
                            error_msg);
  return thread_id;
}

int mysql::run_statement(
             database::mysql_stmt& stmt,
             std::string const& error_msg, bool fatal,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_statement(stmt, error_msg, fatal);
  return thread_id;
}

int mysql::run_statement_and_get_result(
             database::mysql_stmt& stmt,
             std::promise<mysql_result>* promise,
             std::string const& error_msg,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_statement_and_get_result(
                            stmt,
                            promise,
                            error_msg);
  return thread_id;
}

int mysql::run_statement_and_get_int(
             database::mysql_stmt& stmt,
             std::promise<int>* promise, mysql_task::int_type type,
             std::string const& error_msg,
             int thread_id) {
  _check_errors();
  if (thread_id < 0)
    // Here, we use _current_thread
    thread_id = _get_best_connection();

  _connection[thread_id]->run_statement_and_get_int(
                            stmt,
                            promise,
                            type,
                            error_msg);
  return thread_id;
}

void mysql::prepare_statement(mysql_stmt const& stmt) {
  for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
         it(_connection.begin()),
         end(_connection.end());
       it != end;
       ++it)
    (*it)->prepare_query(stmt.get_id(), stmt.get_query());
}

mysql_stmt mysql::prepare_query(std::string const& query,
                         mysql_bind_mapping const& bind_mapping) {
  mysql_stmt retval(query, bind_mapping);
  prepare_statement(retval);

  return retval;
}

mysql::version mysql::schema_version() const {
  return _version;
}

int mysql::connections_count() const {
  return _connection.size();
}
