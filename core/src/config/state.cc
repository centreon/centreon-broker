/*
** Copyright 2011-2012,2017 Centreon
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

#include "com/centreon/broker/config/state.hh"

#include "com/centreon/broker/logging/file.hh"

using namespace com::centreon::broker::config;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
state::state() {
  clear();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
state::state(const state& other)
    : _broker_id(other._broker_id),
      _rpc_port(other._rpc_port),
      _broker_name(other._broker_name),
      _cache_directory(other._cache_directory),
      _command_file(other._command_file),
      _command_protocol(other._command_protocol),
      _endpoints(other._endpoints),
      _event_queue_max_size(other._event_queue_max_size),
      _log_thread_id(other._log_thread_id),
      _log_timestamp(other._log_timestamp),
      _log_human_readable_timestamp(other._log_human_readable_timestamp),
      _loggers(other._loggers),
      _module_dir(other._module_dir),
      _module_list(other._module_list),
      _params(other._params),
      _poller_id(other._poller_id),
      _poller_name(other._poller_name),
      _pool_size(other._pool_size) {}

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& other) {
  if (this != &other) {
    _broker_id = other._broker_id;
    _rpc_port = other._rpc_port;
    _broker_name = other._broker_name;
    _cache_directory = other._cache_directory;
    _command_file = other._command_file;
    _command_protocol = other._command_protocol;
    _endpoints = other._endpoints;
    _event_queue_max_size = other._event_queue_max_size;
    _log_thread_id = other._log_thread_id;
    _log_timestamp = other._log_timestamp;
    _log_human_readable_timestamp = other._log_human_readable_timestamp;
    _loggers = other._loggers;
    _module_dir = other._module_dir;
    _module_list = other._module_list;
    _params = other._params;
    _poller_id = other._poller_id;
    _poller_name = other._poller_name;
    _pool_size = other._pool_size;
  }
  return *this;
}

/**
 *  Reset state to default values.
 */
void state::clear() {
  _broker_id = 0;
  _rpc_port = 0;
  _broker_name.clear();
  _cache_directory.clear();
  _command_file.clear();
  _command_protocol = "json";
  _endpoints.clear();
  _event_queue_max_size = 10000;
  _flush_logs = true;
  _log_thread_id = false;
  _log_timestamp = com::centreon::broker::logging::file::with_timestamp();
  _log_human_readable_timestamp =
      com::centreon::broker::logging::file::with_human_redable_timestamp();
  _loggers.clear();
  _module_dir.clear();
  _module_list.clear();
  _params.clear();
  _poller_id = 0;
  _poller_name.clear();
  _pool_size = 0;
}

/**
 *  Set the Broker ID.
 *
 *  @param[in] id  Broker ID.
 */
void state::broker_id(int id) noexcept {
  _broker_id = id;
}

/**
 *  Get the Broker ID.
 *
 *  @return Broker ID.
 */
int state::broker_id() const noexcept {
  return _broker_id;
}

/**
 *  Set the Broker name.
 *
 *  @param[in] name  Broker name.
 */
void state::broker_name(std::string const& name) {
  _broker_name = name;
}

/**
 *  Get the Broker name.
 *
 *  @return Broker name.
 */
std::string const& state::broker_name() const noexcept {
  return _broker_name;
}

/**
 *  Set the cache directory.
 *
 *  @param[in] dir  Cache directory.
 */
void state::cache_directory(std::string const& dir) {
  _cache_directory = dir;
  if (_cache_directory[_cache_directory.size() - 1] != '/')
    _cache_directory.append("/");
}

/**
 *  Get the cache directory.
 *
 *  @return Cache directory.
 */
std::string const& state::cache_directory() const noexcept {
  return _cache_directory;
}

/**
 *  Set the command file.
 *
 *  @param[in] file  The command file.
 */
void state::command_file(std::string const& file) {
  _command_file = file;
}

/**
 *  Get the command file.
 *
 *  @return  The command file.
 */
std::string const& state::command_file() const noexcept {
  return _command_file;
}

/**
 *  Set the command protocol.
 *
 *  @param[in] prot  The command protocol.
 */
void state::command_protocol(std::string const& prot) {
  _command_protocol = prot;
}

/**
 *  Get the command protocol.
 *
 *  @return  The command protocol.
 */
std::string const& state::command_protocol() const noexcept {
  return _command_protocol;
}

/**
 *  Get the list of endpoints.
 *
 *  @return Endpoint list.
 */
std::list<endpoint>& state::endpoints() noexcept {
  return _endpoints;
}

/**
 *  Get the list of endpoints.
 *
 *  @return Endpoint list.
 */
std::list<endpoint> const& state::endpoints() const noexcept {
  return _endpoints;
}

/**
 *  Set the maximum limit size of the event queue.
 *
 *  @param[in] val Size limit.
 */
void state::event_queue_max_size(int val) noexcept {
  _event_queue_max_size = val;
}

/**
 *  Get the maximum limit size of the event queue.
 *
 *  @return The size limit.
 */
int state::event_queue_max_size() const noexcept {
  return _event_queue_max_size;
}

/**
 *  Set if logs must be flushed.
 *
 *  @param[in] flush true to automatically flush log files.
 */
void state::flush_logs(bool flush) noexcept {
  _flush_logs = flush;
}

/**
 *  Check if logs must be flushed.
 *
 *  @return true if logs must be automatically flushed.
 */
bool state::flush_logs() const noexcept {
  return _flush_logs;
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
std::list<logger>& state::loggers() noexcept {
  return _loggers;
}

/**
 *  Set whether or not to log thread IDs.
 *
 *  @param[in] log_id true to log thread IDs.
 */
void state::log_thread_id(bool log_id) noexcept {
  _log_thread_id = log_id;
}

/**
 *  Get whether or not to log thread IDs.
 *
 *  @return true if thread IDs must be logged.
 */
bool state::log_thread_id() const noexcept {
  return _log_thread_id;
}

/**
 *  Set if and how timestamp should be stored.
 *
 *  @param[in] log_time  Any acceptable value.
 */
void state::log_timestamp(
    com::centreon::broker::logging::timestamp_type log_time) noexcept {
  _log_timestamp = log_time;
}

/**
 *  Get if and how timestamp should be stored.
 *
 *  @return Any acceptable value.
 */
com::centreon::broker::logging::timestamp_type state::log_timestamp() const
    noexcept {
  return _log_timestamp;
}

/**
 *  Set whether or not a human readable timestamp logging should be enabled.
 *
 *  @param[in] human_log_time true to log a human readable timestamp.
 */
void state::log_human_readable_timestamp(bool human_log_time) noexcept {
  _log_human_readable_timestamp = human_log_time;
}

/**
 *  Get whether or not to log a human readable timestamp.
 *
 *  @return true if a human redable timestamp must be logged.
 */
bool state::log_human_readable_timestamp() const noexcept {
  return _log_human_readable_timestamp;
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
std::list<logger> const& state::loggers() const noexcept {
  return _loggers;
}

/**
 *  Get the module directory.
 *
 *  @return Module directory.
 */
std::string const& state::module_directory() const noexcept {
  return _module_dir;
}

/**
 *  Set the module directory.
 *
 *  @param[in] dir Module directory.
 */
void state::module_directory(std::string const& dir) {
  _module_dir = dir;
}

/**
 *  Get the module list.
 *
 *  @return Modifiable module list.
 */
std::list<std::string>& state::module_list() noexcept {
  return _module_list;
}

/**
 *  Get the module list.
 *
 *  @return Const module list.
 */
std::list<std::string> const& state::module_list() const noexcept {
  return _module_list;
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
std::map<std::string, std::string>& state::params() noexcept {
  return _params;
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
std::map<std::string, std::string> const& state::params() const noexcept {
  return _params;
}

/**
 *  Set the poller ID.
 *
 *  @param[in] id  Poller ID.
 */
void state::poller_id(int id) noexcept {
  _poller_id = id;
}

/**
 *  Get the poller ID.
 *
 *  @return Poller ID.
 */
int state::poller_id() const noexcept {
  return _poller_id;
}

/**
 * @brief Set the pool thread size. This pool is used at the moment by the tcp
 * connector, but should be used by others very soon.
 *
 * @param size A non negative integer. If 0, the pool size will be computed
 * automatically with max(2, number of CPUs / 2).
 */
void state::pool_size(int size) noexcept {
  _pool_size = size;
}

/**
 * @brief Get the pool thread size. This pool is used at the moment by the TCP
 * connector, but should be used by others very soon.
 *
 * @return a int integer.
 */
int state::pool_size() const noexcept {
  return _pool_size;
}

/**
 *  Set the poller name.
 *
 *  @param[in] name  Poller name.
 */
void state::poller_name(std::string const& name) {
  _poller_name = name;
}

/**
 *  Get the poller name.
 *
 *  @return Poller name.
 */
std::string const& state::poller_name() const noexcept {
  return _poller_name;
}

void state::rpc_port(uint16_t port) noexcept {
  _rpc_port = port;
}
uint16_t state::rpc_port(void) const noexcept {
  return _rpc_port;
}
