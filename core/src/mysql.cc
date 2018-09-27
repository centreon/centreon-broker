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
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/mysql_error.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
mysql::mysql(database_config const& db_cfg)
  : _db_cfg(db_cfg),
    _pending_queries(0),
    _version(mysql::v3),
    _current_thread(0),
    _prepare_count(0) {
  std::cout << "mysql constructor" << std::endl;
  if (mysql_library_init(0, NULL, NULL))
    throw exceptions::msg()
      << "mysql: unable to initialize the MySQL connector";

  for (int i(0); i < db_cfg.get_connections_count(); ++i) {
    std::cout << "mysql constructor thread " << i << " construction" << std::endl;
    _thread.push_back(new mysql_thread(db_cfg));
    std::cout << "mysql constructor next one" << std::endl;
  }
  try {
    run_query_sync("SELECT instance_id FROM instances LIMIT 1");
    _version = v2;
    logging::info(logging::low)
      << "mysql: database is using version 2 of Centreon schema";
  }
  catch (std::exception const& e) {
    _version = v3;
    logging::info(logging::low)
      << "mysql: database is using version 3 of Centreon schema";
  }

  std::cout << "mysql constructor return" << std::endl;
}

/**
 *  Destructor
 */
mysql::~mysql() {
  std::cout << "mysql destructor" << std::endl;
  bool retval(finish());
  if (!retval)
    logging::error(logging::medium)
      << "mysql: A thread was forced to stop after a timeout of 20s";

  for (std::vector<mysql_thread*>::const_iterator
         it(_thread.begin()),
         end(_thread.end());
       it != end;
       ++it)
    delete *it;

  mysql_library_end();
  std::cout << "mysql destructor return" << std::endl;
}

/**
 *  Commit all pending queries of all the threads.
 *  This function waits for all the threads to be committed.
 *
 *  If an error occures, an exception is thrown.
 */
void mysql::commit() {
  QSemaphore sem;
  std::cout << "sem available : " << sem.available() << std::endl;
  QAtomicInt ko(0);
  for (std::vector<mysql_thread*>::const_iterator
         it(_thread.begin()),
         end(_thread.end());
       it != end;
       ++it) {
    std::cout << "SEND COMMIT TO THREAD" << std::endl;
    (*it)->commit(sem, ko);
    std::cout << "COMMIT SENT TO THREAD" << std::endl;
  }
  // Let's wait for each thread to release the semaphore.
  sem.acquire(_thread.size());
  std::cout << "ALL THE THREADS COMMITS DONE..." << std::endl;
  if (int(ko))
    throw exceptions::msg()
      << "mysql: Unable to commit transactions";
  _pending_queries = 0;
}

/**
 *  This function is used after a run_query_sync() to get its result. This
 *  function gives the result only one time. After that, the result will
 *  be empty.
 *
 * @param thread_id The thread number that made the query.
 *
 * @return a mysql_result.
 */
mysql_result mysql::get_result(int thread_id) {
  return _thread[thread_id]->get_result();
}

int mysql::get_affected_rows(int thread_id) {
  return _thread[thread_id]->get_affected_rows();
}

/**
 *  This method commits only if the max queries per transaction is reached.
 *
 * @return true if a commit has been done, false otherwise.
 */
bool mysql::_commit_if_needed() {
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
  mysql_error err(_thread[thread_id]->get_error());
  if (err.is_active()) {
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
 * @param fn A callback to execute when the query has been executed.
 * @param data A data to give to the callback.
 * @param error_msg An error message to complete the error message returned
 *                  by the mysql connector.
 * @param fatal A boolean telling if the error is fatal. In that case, an
 *              exception will be thrown if the error occures.
 * @param thread A thread id or 0 to keep the library choosing which one.
 *
 * @return A boolean telling if a commit has been done after the query.
 */
bool mysql::run_query(std::string const& query,
              std::string const& error_msg, bool fatal,
              mysql_callback fn, void* data,
              int thread) {
  if (thread < 0) {
    // Here, we use _current_thread
    thread = _current_thread++;
    if (_current_thread >= _thread.size())
      _current_thread = 0;
  }
  std::cout << "-> CHECK ERRORS FATAL" << std::endl;
  _check_errors(thread);
  std::cout << "-> FATAL 1" << std::endl;
  _thread[thread]->run_query(
    query,
    error_msg, fatal,
    fn, data);
  return _commit_if_needed();
}

/**
 *  Execute a query synchronously, that is to say the method waits for the
 *  query to be done before return.
 *
 * @param query The query to execute, generally a select.
 * @param error_msg An error message to complete the one that could be returned
 *                  by the library.
 * @param thread A thread id or 0 to keep the library to choose the id.
 *
 * @return The thread id that executes the query.
 */
int mysql::run_query_sync(std::string const& query,
             std::string const& error_msg,
             int thread) {
  if (thread < 0) {
    // Here, we use _current_thread
    thread = _current_thread++;
    if (_current_thread >= _thread.size())
      _current_thread = 0;
  }
  _check_errors(thread);
  _thread[thread]->run_query_sync(
    query,
    error_msg);

  return thread;
}

int mysql::get_last_insert_id(int thread_id) {
  return _thread[thread_id]->get_last_insert_id();
}

bool mysql::run_statement(int statement_id, mysql_bind const& bind,
              std::string const& error_msg, bool fatal,
              mysql_callback fn, void* data,
              int thread) {
  if (thread < 0) {
    // Here, we use _current_thread
    thread = _current_thread++;
    if (_current_thread >= _thread.size())
      _current_thread = 0;
  }
  _check_errors(thread);
  _thread[thread]->run_statement(
    statement_id, bind,
    error_msg, fatal,
    fn, data);
  return _commit_if_needed();
}

int mysql::run_statement_sync(int statement_id, mysql_bind const& bind,
             std::string const& error_msg, int thread) {
  if (thread < 0) {
    // Here, we use _current_thread
    thread = _current_thread++;
    if (_current_thread >= _thread.size())
      _current_thread = 0;
  }
  _check_errors(thread);
  _thread[thread]->run_statement_sync(
    statement_id, bind,
    error_msg);

  return thread;
}

int mysql::prepare_query(std::string const& query,
                         mysql_stmt_mapping const& bind_mapping) {
  for (std::vector<mysql_thread*>::const_iterator
         it(_thread.begin()),
         end(_thread.end());
       it != end;
       ++it)
    (*it)->prepare_query(query, bind_mapping);

  return _prepare_count++;
}

bool mysql::finish() {
  bool retval(true);
  for (std::vector<mysql_thread*>::const_iterator
         it(_thread.begin()),
         end(_thread.end());
       it != end;
       ++it) {
    std::cout << "mysql destructor send finish to thread" << std::endl;
    (*it)->finish();
    std::cout << "mysql destructor wait for thread to finish" << std::endl;
    retval &= (*it)->wait(20000);
  }
  std::cout << "mysql destructor thread just finished" << std::endl;
  return retval;
}

mysql::version mysql::schema_version() const {
  return _version;
}

int mysql::connections_count() const {
  return _thread.size();
}

int mysql::get_stmt_size() const {
  return _thread[0]->get_stmt_size();
}

mysql_stmt_mapping mysql::get_stmt_mapping(int stmt_id) const {
  return _thread[0]->get_stmt_mapping(stmt_id);
}
