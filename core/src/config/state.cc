/*
** Copyright 2011-2012,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
state::state(state const& other) {
  _internal_copy(other);
}

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
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Reset state to default values.
 */
void state::clear() {
  _cache_directory.clear();
  _command_file.clear();
  _endpoints.clear();
  _event_queue_max_size = 10000;
  _flush_logs = true;
  _instance_id = 0;
  _instance_name.clear();
  _log_thread_id = false;
  _log_timestamp
    = com::centreon::broker::logging::file::with_timestamp();
  _log_human_readable_timestamp
    = com::centreon::broker::logging::file::with_human_redable_timestamp();
  _loggers.clear();
  _module_dir.clear();
  _module_list.clear();
  _params.clear();
  return ;
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
  return ;
}

/**
 *  Get the cache directory.
 *
 *  @return Cache directory.
 */
std::string const& state::cache_directory() const throw () {
  return (_cache_directory);
}

/**
 *  Set the command file.
 *
 *  @param[in] file  The command file.
 */
void state::command_file(std::string const& file) {
  _command_file = file;
  return ;
}

/**
 *  Get the command file.
 *
 *  @return  The command file.
 */
std::string const& state::command_file() const throw() {
  return (_command_file);
}

/**
 *  Get the list of endpoints.
 *
 *  @return Endpoint list.
 */
std::list<endpoint>& state::endpoints() throw () {
  return (_endpoints);
}

/**
 *  Get the list of endpoints.
 *
 *  @return Endpoint list.
 */
std::list<endpoint> const& state::endpoints() const throw () {
  return (_endpoints);
}

/**
 *  Set the maximum limit size of the event queue.
 *
 *  @param[in] val Size limit.
 */
void state::event_queue_max_size(unsigned int val) throw () {
  _event_queue_max_size = val;
}

/**
 *  Get the maximum limit size of the event queue.
 *
 *  @return The size limit.
 */
unsigned int state::event_queue_max_size() const throw () {
  return (_event_queue_max_size);
}

/**
 *  Set if logs must be flushed.
 *
 *  @param[in] flush true to automatically flush log files.
 */
void state::flush_logs(bool flush) throw () {
  _flush_logs = flush;
  return ;
}

/**
 *  Check if logs must be flushed.
 *
 *  @return true if logs must be automatically flushed.
 */
bool state::flush_logs() const throw () {
  return (_flush_logs);
}

/**
 *  Set the instance id.
 *
 *  @param[in] id Instance id.
 */
void state::instance_id(unsigned int id) throw () {
  _instance_id = id;
}

/**
 *  Get the instance id.
 *
 *  @return Instance id.
 */
unsigned int state::instance_id() const throw () {
  return (_instance_id);
}

/**
 *  Set the instance name.
 *
 *  @param[in] name Instance name.
 */
void state::instance_name(std::string const& name) throw () {
  _instance_name = name;
}

/**
 *  Get the instance name.
 *
 *  @return Instance name.
 */
std::string const& state::instance_name() const throw () {
  return (_instance_name);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
std::list<logger>& state::loggers() throw () {
  return (_loggers);
}

/**
 *  Set whether or not to log thread IDs.
 *
 *  @param[in] log_id true to log thread IDs.
 */
void state::log_thread_id(bool log_id) throw () {
  _log_thread_id = log_id;
  return ;
}

/**
 *  Get whether or not to log thread IDs.
 *
 *  @return true if thread IDs must be logged.
 */
bool state::log_thread_id() const throw () {
  return (_log_thread_id);
}

/**
 *  Set whether or not timestamp logging should be enabled.
 *
 *  @param[in] log_time true to log timestamp.
 */
void state::log_timestamp(bool log_time) throw () {
  _log_timestamp = log_time;
  return ;
}

/**
 *  Get whether or not to log timestamp.
 *
 *  @return true if timestamp must be logged.
 */
bool state::log_timestamp() const throw () {
  return (_log_timestamp);
}

/**
 *  Set whether or not a human readable timestamp logging should be enabled.
 *
 *  @param[in] human_log_time true to log a human readable timestamp.
 */
void state::log_human_readable_timestamp(bool human_log_time) throw () {
  _log_human_readable_timestamp = human_log_time;
  return ;
}


/**
 *  Get whether or not to log a human readable timestamp.
 *
 *  @return true if a human redable timestamp must be logged.
 */
bool state::log_human_readable_timestamp() const throw() {
  return (_log_human_readable_timestamp);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
std::list<logger> const& state::loggers() const throw () {
  return (_loggers);
}

/**
 *  Get the module directory.
 *
 *  @return Module directory.
 */
std::string const& state::module_directory() const throw () {
  return (_module_dir);
}

/**
 *  Set the module directory.
 *
 *  @param[in] dir Module directory.
 */
void state::module_directory(std::string const& dir) {
  _module_dir = dir;
  return ;
}

/**
 *  Get the module list.
 *
 *  @return Modifiable module list.
 */
std::list<std::string>& state::module_list() throw () {
  return (_module_list);
}

/**
 *  Get the module list.
 *
 *  @return Const module list.
 */
std::list<std::string> const& state::module_list() const throw () {
  return (_module_list);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
std::map<std::string, std::string>& state::params() throw () {
  return (_params);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
std::map<std::string, std::string> const& state::params() const throw () {
  return (_params);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void state::_internal_copy(state const& other) {
  _cache_directory = other._cache_directory;
  _command_file = other._command_file;
  _endpoints = other._endpoints;
  _event_queue_max_size = other._event_queue_max_size;
  _instance_id = other._instance_id;
  _instance_name = other._instance_name;
  _log_thread_id = other._log_thread_id;
  _log_timestamp = other._log_timestamp;
  _log_human_readable_timestamp = other._log_human_readable_timestamp;
  _loggers = other._loggers;
  _module_dir = other._module_dir;
  _module_list = other._module_list;
  _params = other._params;
  return ;
}
