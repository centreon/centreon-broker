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
#include <QSemaphore>
#include <atomic>
#include <iostream>
#include <mutex>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/mysql_error.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;

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
    _current_thread(0) {

  mysql_manager& mgr(mysql_manager::instance());
  _connection = mgr.get_connections(db_cfg);

  try {
    std::promise<mysql_result> promise;
    int thread_id(run_query(
                    "SELECT instance_id FROM instances LIMIT 1",
                    &promise,
                    "", true));
    _version = v2;
    promise.get_future().get();
    logging::info(logging::low)
      << "mysql: database is using version 2 of Centreon schema";
    std::cout << "mysql: database in version 2..." << std::endl;
  }
  catch (std::exception const& e) {
    logging::info(logging::low)
      << "mysql: database is using version 3 of Centreon schema";
    std::cout << "mysql: database in version 3..." << std::endl;
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
  QSemaphore sem;
  std::atomic_int ko(0);
  int commits;
  if (thread_id < 0) {
    for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
           it(_connection.begin()),
           end(_connection.end());
         it != end;
         ++it) {
      (*it)->commit(sem, ko);
    }
    commits = _connection.size();
  }
  else {
    _connection[thread_id]->commit(sem, ko);
    commits = 1;
  }
  sem.acquire(commits);
  if (int(ko))
    throw exceptions::msg()
      << "mysql: Unable to commit transactions";
  _pending_queries = 0;
}

bool mysql::fetch_row(int thread_id, mysql_result& res) {
  _check_errors(thread_id);
  return _connection[thread_id]->fetch_row(res);
}

void mysql::check_affected_rows(
             int thread_id,
             std::string const& message) {
  _check_errors(thread_id);
  _connection[thread_id]->check_affected_rows(message);
}

void mysql::check_affected_rows(
             int thread_id,
             mysql_stmt const& stmt,
             std::string const& message) {
  _check_errors(thread_id);
  _connection[thread_id]->check_affected_rows(message, stmt.get_id());
}

int mysql::get_affected_rows(int thread_id) {
  _check_errors(thread_id);
  return _connection[thread_id]->get_affected_rows();
}

int mysql::get_affected_rows(int thread_id, mysql_stmt const& stmt) {
  _check_errors(thread_id);
  return _connection[thread_id]->get_affected_rows(stmt.get_id());
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
 * @param thread_id The thread id to check.
 */
void mysql::_check_errors(int thread_id) {
  if (mysql_manager::instance().is_in_error()) {
    mysql_error err(mysql_manager::instance().get_error());
    if (err.is_fatal())
      throw exceptions::msg() << err.get_message();
    else
      logging::error(logging::medium)
        << "mysql: " << err.get_message();
  }
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
              std::promise<mysql_result>* p,
              std::string const& error_msg, bool fatal,
              int thread_id) {
  if (thread_id < 0) {
    // Here, we use _current_thread
    thread_id = _current_thread++;
    if (_current_thread >= _connection.size())
      _current_thread = 0;
  }
  _check_errors(thread_id);
  _connection[thread_id]->run_query(
    query,
    p,
    error_msg, fatal);
  return thread_id;
}

int mysql::get_last_insert_id(int thread_id) {
  return _connection[thread_id]->get_last_insert_id();
}

int mysql::run_statement_on_condition(
             mysql_stmt& stmt,
             std::promise<mysql_result>* p,
             mysql_task::condition condition,
             std::string const& error_msg, bool fatal,
             int thread_id) {
  _connection[thread_id]->run_statement_on_condition(
                        stmt,
                        p,
                        condition,
                        error_msg,
                        fatal);
  return thread_id;
}

int mysql::run_statement(mysql_stmt& stmt,
             std::promise<mysql_result>* promise,
             std::string const& error_msg, bool fatal,
             int thread_id) {
  if (thread_id < 0) {
    // Here, we use _current_thread
    thread_id = _current_thread++;
    if (_current_thread >= _connection.size())
      _current_thread = 0;
  }
  _check_errors(thread_id);
  _connection[thread_id]->run_statement(stmt, promise, error_msg, fatal);
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

//bool mysql::finish() {
//  bool retval(true);
//  for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
//         it(_connection.begin()),
//         end(_connection.end());
//       it != end;
//       ++it) {
//    std::cout << "mysql destructor send finish to thread" << std::endl;
//    (*it)->finish();
//    std::cout << "mysql destructor wait for thread to finish" << std::endl;
//    retval &= (*it)->wait(20000);
//  }
//  std::cout << "mysql destructor thread just finished" << std::endl;
//  return retval;
//}

mysql::version mysql::schema_version() const {
  return _version;
}

int mysql::connections_count() const {
  return _connection.size();
}
