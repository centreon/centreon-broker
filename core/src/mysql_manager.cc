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
#include <chrono>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mysql_manager.hh"

using namespace com::centreon::broker;

mysql_manager mysql_manager::_singleton;

mysql_manager& mysql_manager::instance() {
  return _singleton;
}

mysql_manager::mysql_manager()
  : _current_thread(0),
    _version(mysql::v2) {   // FIXME DBR
  if (mysql_library_init(0, nullptr, nullptr))
    throw exceptions::msg()
      << "mysql_manager: unable to initialize the MySQL connector";
}

mysql_manager::~mysql_manager() {
  // If connections are still active but unique here, we can remove them
  for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
       it(_connection.begin()), end(_connection.end());
       it != end;
       ++it) {
    while (!it->unique()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  mysql_library_end();
}

std::vector<std::shared_ptr<mysql_connection>> mysql_manager::get_connections(
                      database_config const& db_cfg) {
  std::vector<std::shared_ptr<mysql_connection>> retval;
  int connection_count(db_cfg.get_connections_count());
  int current_connection(0);

  std::lock_guard<std::mutex> lock(_cfg_mutex);
  for (std::shared_ptr<mysql_connection> c : _connection) {
    // Is this thread matching what the configuration needs?
    if (c->match_config(db_cfg)) {
      // Yes
      retval.push_back(c);
      ++current_connection;
      if (current_connection > connection_count)
        return retval;
    }
  }

  // We are still missing threads in the configuration to return
  for ( ; current_connection < connection_count; ++current_connection) {
    std::shared_ptr<mysql_connection> c(std::make_shared<mysql_connection>(db_cfg));
    _connection.push_back(c);
    retval.push_back(c);
  }

  // If connections are still active but unique here, we can remove them
  for (std::vector<std::shared_ptr<mysql_connection>>::iterator
       it(_connection.begin()), end(_connection.end());
       it != end;
       ++it) {
    if (it->unique()) {
      logging::info(logging::low)
        << "mysql_manager: one connection removed";
      _connection.erase(it);
    }
  }
  return retval;
}

bool mysql_manager::is_in_error() const {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return _error.is_active();
}

com::centreon::broker::mysql_error mysql_manager::get_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return std::move(_error);
}

void mysql_manager::set_error(std::string const& message, bool fatal) {
  std::lock_guard<std::mutex> locker(_err_mutex);
  if (!_error.is_active())
    _error = mysql_error(message.c_str(), fatal);
}

void mysql_manager::clear_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  _error.clear();
}

std::map<std::string, std::string> mysql_manager::get_stats() const {
  std::map<std::string, std::string> retval;

  std::lock_guard<std::mutex> locker(_cfg_mutex);
  int connection_count(_connection.size());
  std::string key("connection ");
  int key_len(key.size());
  std::string value;
  for (int i(0); i < connection_count; ++i) {
    key.replace(key_len, std::string::npos, std::to_string(i));
    int count(_connection[i]->get_tasks_count());
    value = std::to_string(_connection[i]->get_tasks_count())
              + " waiting tasks";
    if (count == 0)
      retval.insert(std::make_pair(key, "no task"));
    else
      retval.insert(std::make_pair(key, value));
  }
  return retval;
}
