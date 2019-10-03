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
#include "com/centreon/broker/mysql_manager.hh"
#include <chrono>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

mysql_manager mysql_manager::_singleton;

mysql_manager& mysql_manager::instance() {
  return _singleton;
}

/**
 *  Constructor
 */
mysql_manager::mysql_manager()
    : _current_thread(0),
      _version(mysql::v2),
      _stats_connections_timestamp(time(nullptr)) {}

/**
 *  Destructor
 */
mysql_manager::~mysql_manager() {
  // If connections are still active but unique here, we can remove them
  std::lock_guard<std::mutex> cfg_lock(_cfg_mutex);
  std::lock_guard<std::mutex> err_lock(_err_mutex);
  for (std::vector<std::shared_ptr<mysql_connection>>::const_iterator
           it(_connection.begin()),
       end(_connection.end());
       it != end; ++it) {
    while (!it->unique()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  _connection.clear();
  mysql_library_end();
}

std::vector<std::shared_ptr<mysql_connection>> mysql_manager::get_connections(
    database_config const& db_cfg) {
  std::vector<std::shared_ptr<mysql_connection>> retval;
  uint32_t connection_count(db_cfg.get_connections_count());

  if (_connection.size() == 0) {
    if (mysql_library_init(0, nullptr, nullptr))
      throw exceptions::msg()
          << "mysql_manager: unable to initialize the MySQL connector";
  }

  {
    uint32_t current_connection(0);
    std::lock_guard<std::mutex> lock(_cfg_mutex);
    for (std::shared_ptr<mysql_connection>& c : _connection) {
      // Is this thread matching what the configuration needs?
      if (c->match_config(db_cfg) && !c->is_finished()) {
        // Yes
        retval.push_back(c);
        ++current_connection;
        if (current_connection >= connection_count)
          return retval;
      }
    }

    // We are still missing threads in the configuration to return
    while (retval.size() < connection_count) {
      std::shared_ptr<mysql_connection> c(
          std::make_shared<mysql_connection>(db_cfg));
      _connection.push_back(c);
      retval.push_back(c);
    }
  }
  update_connections();
  return retval;
}

void mysql_manager::clear() {
  std::lock_guard<std::mutex> lock(_cfg_mutex);
  // If connections are still active but unique here, we can remove them
  for (std::shared_ptr<mysql_connection>& conn : _connection) {
    if (!conn.unique() && !conn->is_finished())
      try {
        conn->finish();
      } catch (std::exception const& e) {
        logging::info(logging::low)
            << "mysql_manager: Unable to stop a connection: " << e.what();
      }
  }
  logging::debug(logging::low) << "mysql_manager: clear finished";
}

void mysql_manager::update_connections() {
  std::lock_guard<std::mutex> lock(_cfg_mutex);
  // If connections are still active but unique here, we can remove them
  std::vector<std::shared_ptr<mysql_connection>>::iterator it(
      _connection.begin());
  while (it != _connection.end()) {
    if (it->unique() || (*it)->is_finished()) {
      logging::info(logging::low) << "mysql_manager: one connection removed";
      it = _connection.erase(it);
    } else
      ++it;
  }
  logging::info(logging::medium)
      << "mysql_manager: active connections: " << _connection.size();

  if (_connection.size() == 0)
    mysql_library_end();
}

bool mysql_manager::is_in_error() const {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return _error.is_active();
}

database::mysql_error mysql_manager::get_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  return std::move(_error);
}

void mysql_manager::set_error(std::string const& message) {
  std::lock_guard<std::mutex> locker(_err_mutex);
  if (!_error.is_active())
    _error = database::mysql_error(message.c_str(), true);
}

void mysql_manager::clear_error() {
  std::lock_guard<std::mutex> locker(_err_mutex);
  _error.clear();
}

std::map<std::string, std::string> mysql_manager::get_stats() {
  int delay(0);
  std::unique_lock<std::mutex> locker(_cfg_mutex, std::defer_lock);
  int stats_connections_count(_stats_counts.size());
  if (locker.try_lock()) {
    _stats_connections_timestamp = time(nullptr);
    stats_connections_count = _connection.size();
    _stats_counts.resize(stats_connections_count);
    for (int i(0); i < stats_connections_count; ++i)
      _stats_counts[i] = _connection[i]->get_tasks_count();
  } else
    delay = time(nullptr) - _stats_connections_timestamp;

  std::map<std::string, std::string> retval;
  retval.insert(
      std::make_pair("delay since last check", std::to_string(delay)));
  std::string key("waiting tasks in connection ");
  int key_len(key.size());
  for (int i(0); i < stats_connections_count; ++i) {
    key.replace(key_len, std::string::npos, std::to_string(i));
    retval.insert(std::make_pair(key, std::to_string(_stats_counts[i])));
  }
  return retval;
}
