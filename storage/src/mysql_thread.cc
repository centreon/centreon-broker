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
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/mysql_thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

void mysql_thread::run() {
  while (true) {
    std::cout << "run mutex lock" << std::endl;
    QMutexLocker locker(&_list_mutex);
    std::cout << "run mutex locked" << std::endl;
    if (_finished) {
      if (!_queries_list.empty()) {
        std::cout << "run new query" << std::endl;
        std::string query(_queries_list.front());
        _queries_list.pop_front();
        if (mysql_query(_conn, query.c_str())) {
          std::cout << "run query failed: " << mysql_error(_conn) << std::endl;
          logging::error(logging::medium)
            << "storage: Error while sending query '" << query << "'";
        }
        else
          std::cout << "run query successed" << std::endl;
      }
      std::cout << "run finished1" << std::endl;
      break;
    }
    std::cout << "run wait for condition on queries or finish" << std::endl;
    _queries_or_finished.wait(locker.mutex());
    std::cout << "run condition realized" << std::endl;
    if (_finished) {
      if (!_queries_list.empty()) {
        std::cout << "run new query" << std::endl;
        std::string query(_queries_list.front());
        _queries_list.pop_front();
        if (mysql_query(_conn, query.c_str())) {
          std::cout << "run query failed: " << mysql_error(_conn) << std::endl;
          logging::error(logging::medium)
            << "storage: Error while sending query '" << query << "'";
        }
        else
          std::cout << "run query successed" << std::endl;
      }
      std::cout << "run finished2" << std::endl;
      break;
    }
    std::cout << "run new query" << std::endl;
    std::string query(_queries_list.front());
    _queries_list.pop_front();
    locker.unlock();
    std::cout << "run mutex unlocked" << std::endl;
    std::cout << "run query execution" << std::endl;
    if (mysql_query(_conn, query.c_str())) {
      std::cout << "run query failed: " << mysql_error(_conn) << std::endl;
      logging::error(logging::medium)
        << "storage: Error while sending query '" << query << "'";
    }
    else
      std::cout << "run query successed" << std::endl;
  }
  std::cout << "run return" << std::endl;
}

mysql_thread::mysql_thread(
                std::string const& address,
                std::string const& user,
                std::string const& password,
                std::string const& database,
                int port)
  : _conn(mysql_init(NULL)),
    _finished(false) {
  std::cout << "mysql_thread constructor" << std::endl;
  if (!_conn) {
    std::cout << "mysql_thread throw exception" << std::endl;
    throw exceptions::msg()
      << "storage: Unable to initialize the MySQL client connector: "
      << mysql_error(_conn);
  }

  std::cout << "mysql_thread real connect..." << std::endl;
  if (!mysql_real_connect(
         _conn,
         address.c_str(),
         user.c_str(),
         password.c_str(),
         database.c_str(),
         port,
         NULL,
         0)) {
    std::cout << "mysql_thread constructor real connect failed" << std::endl;
    std::cout << "mysql_thread throw exception" << std::endl;
    throw exceptions::msg()
      << "storage: The connection to '"
      << database << ":" << port << "' MySQL database failed: "
      << mysql_error(_conn);
  }
  std::cout << "mysql_thread start thread..." << std::endl;
  start();
  std::cout << "mysql_thread return" << std::endl;
}

mysql_thread::~mysql_thread() {
  std::cout << "destructor" << std::endl;
  mysql_close(_conn);
  mysql_thread_end();
  std::cout << "destructor return" << std::endl;
}

void mysql_thread::run_query(std::string const& query) {
  std::cout << "run_query lock" << std::endl;
  QMutexLocker locker(&_list_mutex);
  std::cout << "run_query locked" << std::endl;
  _queries_list.push_back(query);
  std::cout << "run_query query pushed" << std::endl;
  _queries_or_finished.wakeAll();
  std::cout << "run_query thread woke up" << std::endl;
}

void mysql_thread::finish() {
  std::cout << "finish mutex lock" << std::endl;
  QMutexLocker locker(&_list_mutex);
  std::cout << "finish mutex locked" << std::endl;
  _finished = true;
  std::cout << "finish set" << std::endl;
  _queries_or_finished.wakeAll();
  std::cout << "finish thread woke up" << std::endl;
  std::cout << "finish mutex unlocked" << std::endl;
}
