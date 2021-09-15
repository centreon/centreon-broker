/*
** Copyright 2020-2021 Centreon
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

#include "com/centreon/broker/log_v2.hh"

#include <fmt/format.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fstream>

#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace spdlog;

const std::array<std::string, 16> log_v2::loggers{
    "bam",      "bbdo",         "config", "core",  "lua",      "influxdb",
    "graphite", "notification", "rrd",    "stats", "perfdata", "processing",
    "sql",      "neb",          "tcp",    "tls"};

std::map<std::string, level::level_enum> log_v2::_levels_map{
    {"trace", level::trace}, {"debug", level::debug},
    {"info", level::info},   {"warning", level::warn},
    {"error", level::err},   {"critical", level::critical},
    {"disabled", level::off}};

log_v2& log_v2::instance() {
  static log_v2 instance;
  return instance;
}

log_v2::log_v2() {
  auto stdout_sink = std::make_shared<sinks::stdout_color_sink_mt>();
  auto null_sink = std::make_shared<sinks::null_sink_mt>();

  _bam_log = std::make_shared<logger>("bam", stdout_sink);
  _bbdo_log = std::make_shared<logger>("bbdo", stdout_sink);
  _config_log = std::make_shared<logger>("config", stdout_sink);
  _core_log = std::make_shared<logger>("core", stdout_sink);
  _graphite_log = std::make_shared<logger>("graphite", stdout_sink);
  _notification_log = std::make_shared<logger>("notification", stdout_sink);
  _rrd_log = std::make_shared<logger>("rrd", stdout_sink);
  _stats_log = std::make_shared<logger>("stats", stdout_sink);
  _influxdb_log = std::make_shared<logger>("influxdb", stdout_sink);
  _lua_log = std::make_shared<logger>("lua", stdout_sink);
  _neb_log = std::make_shared<logger>("neb", stdout_sink);
  _perfdata_log = std::make_shared<logger>("perfdata", stdout_sink);
  _processing_log = std::make_shared<logger>("processing", stdout_sink);
  _sql_log = std::make_shared<logger>("sql", stdout_sink);
  _tcp_log = std::make_shared<logger>("tcp", stdout_sink);
  _tls_log = std::make_shared<logger>("tls", stdout_sink);
}

log_v2::~log_v2() {
  _core_log->info("log finished");
  _bam_log->flush();
  _bbdo_log->flush();
  _config_log->flush();
  _core_log->flush();
  _graphite_log->flush();
  _notification_log->flush();
  _rrd_log->flush();
  _stats_log->flush();
  _influxdb_log->flush();
  _lua_log->flush();
  _neb_log->flush();
  _perfdata_log->flush();
  _processing_log->flush();
  _sql_log->flush();
  _tcp_log->flush();
  _tls_log->flush();
}

void log_v2::apply(const config::state& conf) {
  std::lock_guard<std::mutex> lock(_load_m);

  const auto& log = conf.log_conf();

  _log_name = log.log_path();
  // reset loggers to null sink
  auto null_sink = std::make_shared<sinks::null_sink_mt>();
  std::shared_ptr<sinks::base_sink<std::mutex>> file_sink;

  if (log.max_size)
    file_sink = std::make_shared<sinks::rotating_file_sink_mt>(
        _log_name, log.max_size, 99);
  else
    file_sink = std::make_shared<sinks::basic_file_sink_mt>(_log_name);

  _core_log = std::make_shared<logger>("core", file_sink);
  _core_log->set_level(level::info);
  _core_log->flush_on(level::info);
  _core_log->set_pattern("[%Y-%m-%dT%H:%M:%S.%e%z] [%n] [%l] %v");
  _core_log->info("{} : log started", _log_name);

  _bam_log = std::make_shared<logger>("bam", null_sink);
  _bbdo_log = std::make_shared<logger>("bbdo", null_sink);
  _config_log = std::make_shared<logger>("config", null_sink);
  _graphite_log = std::make_shared<logger>("graphite", null_sink);
  _notification_log = std::make_shared<logger>("notification", null_sink);
  _rrd_log = std::make_shared<logger>("rrd", null_sink);
  _stats_log = std::make_shared<logger>("stats", null_sink);
  _influxdb_log = std::make_shared<logger>("influxdb", null_sink);
  _lua_log = std::make_shared<logger>("lua", null_sink);
  _neb_log = std::make_shared<logger>("neb", null_sink);
  _perfdata_log = std::make_shared<logger>("perfdata", null_sink);
  _processing_log = std::make_shared<logger>("processing", null_sink);
  _sql_log = std::make_shared<logger>("sql", null_sink);
  _tcp_log = std::make_shared<logger>("tcp", null_sink);
  _tls_log = std::make_shared<logger>("tls", null_sink);

  for (auto it = log.loggers.begin(), end = log.loggers.end(); it != end;
       ++it) {
    std::shared_ptr<logger>* l;
    if (it->first == "core")
      l = &_core_log;
    else if (it->first == "config")
      l = &_config_log;
    else if (it->first == "tls")
      l = &_tls_log;
    else if (it->first == "tcp")
      l = &_tcp_log;
    else if (it->first == "bbdo")
      l = &_bbdo_log;
    else if (it->first == "sql")
      l = &_sql_log;
    else if (it->first == "perfdata")
      l = &_perfdata_log;
    else if (it->first == "lua")
      l = &_lua_log;
    else if (it->first == "processing")
      l = &_processing_log;
    else if (it->first == "bam")
      l = &_bam_log;
    else if (it->first == "influxdb")
      l = &_influxdb_log;
    else if (it->first == "graphite")
      l = &_graphite_log;
    else if (it->first == "notification")
      l = &_notification_log;
    else if (it->first == "rrd")
      l = &_rrd_log;
    else if (it->first == "stats")
      l = &_stats_log;
    else if (it->first == "neb")
      l = &_neb_log;
    else
      continue;

    *l = std::make_shared<logger>(it->first, file_sink);
    (*l)->set_level(_levels_map[it->second]);
    (*l)->flush_on(_levels_map[it->second]);
    (*l)->set_pattern("[%Y-%m-%dT%H:%M:%S.%e%z] [%n] [%l] %v");
  }
}

std::shared_ptr<spdlog::logger> log_v2::bam() {
  return instance()._bam_log;
}

std::shared_ptr<spdlog::logger> log_v2::bbdo() {
  return instance()._bbdo_log;
}

std::shared_ptr<spdlog::logger> log_v2::config() {
  return instance()._config_log;
}

std::shared_ptr<spdlog::logger> log_v2::core() {
  return instance()._core_log;
}

std::shared_ptr<spdlog::logger> log_v2::influxdb() {
  return instance()._influxdb_log;
}

std::shared_ptr<spdlog::logger> log_v2::graphite() {
  return instance()._graphite_log;
}

std::shared_ptr<spdlog::logger> log_v2::notification() {
  return instance()._notification_log;
}

std::shared_ptr<spdlog::logger> log_v2::rrd() {
  return instance()._rrd_log;
}

std::shared_ptr<spdlog::logger> log_v2::stats() {
  return instance()._stats_log;
}

std::shared_ptr<spdlog::logger> log_v2::lua() {
  return instance()._lua_log;
}

std::shared_ptr<spdlog::logger> log_v2::neb() {
  return instance()._neb_log;
}

std::shared_ptr<spdlog::logger> log_v2::perfdata() {
  return instance()._perfdata_log;
}

std::shared_ptr<spdlog::logger> log_v2::processing() {
  return instance()._processing_log;
}

std::shared_ptr<spdlog::logger> log_v2::sql() {
  return instance()._sql_log;
}

std::shared_ptr<spdlog::logger> log_v2::tcp() {
  return instance()._tcp_log;
}

std::shared_ptr<spdlog::logger> log_v2::tls() {
  return instance()._tls_log;
}

const std::string& log_v2::log_name() const {
  return _log_name;
}

std::list<std::string> log_v2::levels() {
  std::list<std::string> retval;
  for (auto it = _levels_map.begin(); it != _levels_map.end(); ++it)
    retval.push_back(it->first);
  return retval;
}
