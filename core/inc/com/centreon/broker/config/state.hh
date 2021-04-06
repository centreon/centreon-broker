/*
** Copyright 2011-2012,2017, 2021 Centreon
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

#ifndef CCB_CONFIG_STATE_HH
#define CCB_CONFIG_STATE_HH

#include <fmt/format.h>
#include <list>
#include <map>
#include <string>
#include <unordered_map>

#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace config {
/**
 *  @class state state.hh "com/centreon/broker/config/state.hh"
 *  @brief Full configuration state.
 *
 *  A fully parsed configuration is represented within this class
 *  which holds mandatory parameters as well as optional parameters,
 *  along with object definitions.
 */
class state {
  int _broker_id;
  uint16_t _rpc_port;
  std::string _broker_name;
  std::string _cache_directory;
  std::string _command_file;
  std::string _command_protocol;
  std::list<endpoint> _endpoints;
  int _event_queue_max_size;
  bool _flush_logs;
  bool _log_thread_id;
  logging::timestamp_type _log_timestamp;
  bool _log_human_readable_timestamp;
  std::list<logger> _loggers;
  std::string _module_dir;
  std::list<std::string> _module_list;
  std::map<std::string, std::string> _params;
  int _poller_id;
  std::string _poller_name;
  size_t _pool_size;

  struct log {
    std::string directory;
    std::string filename;
    std::size_t max_size;
    std::unordered_map<std::string, std::string> loggers;

    std::string log_path() const {
      return fmt::format("{}/{}", directory, filename);
    }
  } _log_conf;

 public:
  state();
  state(state const& other);
  ~state();
  state& operator=(state const&);
  void broker_id(int id) noexcept;
  int broker_id() const noexcept;
  void rpc_port(uint16_t port) noexcept;
  uint16_t rpc_port(void) const noexcept;
  void broker_name(std::string const& name);
  const std::string& broker_name() const noexcept;
  void cache_directory(std::string const& dir);
  std::string const& cache_directory() const noexcept;
  void command_file(std::string const& file);
  std::string const& command_file() const noexcept;
  void command_protocol(std::string const& prot);
  std::string const& command_protocol() const noexcept;
  void clear();
  std::list<endpoint>& endpoints() noexcept;
  std::list<endpoint> const& endpoints() const noexcept;
  void event_queue_max_size(int val) noexcept;
  int event_queue_max_size() const noexcept;
  void flush_logs(bool flush) noexcept;
  bool flush_logs() const noexcept;
  void log_thread_id(bool log_id) noexcept;
  bool log_thread_id() const noexcept;
  void log_timestamp(logging::timestamp_type log_time) noexcept;
  logging::timestamp_type log_timestamp() const noexcept;
  void log_human_readable_timestamp(bool human_log_time) noexcept;
  bool log_human_readable_timestamp() const noexcept;
  std::list<logger>& loggers() noexcept;
  std::list<logger> const& loggers() const noexcept;
  std::string const& module_directory() const noexcept;
  void module_directory(std::string const& dir);
  std::list<std::string>& module_list() noexcept;
  std::list<std::string> const& module_list() const noexcept;
  std::map<std::string, std::string>& params() noexcept;
  std::map<std::string, std::string> const& params() const noexcept;
  void poller_id(int id) noexcept;
  int poller_id() const noexcept;
  void pool_size(int size) noexcept;
  int pool_size() const noexcept;
  void poller_name(std::string const& name);
  std::string const& poller_name() const noexcept;
  log& log_conf();
  const log& log_conf() const;
};
}  // namespace config

CCB_END()

#endif  // !CCB_CONFIG_STATE_HH
