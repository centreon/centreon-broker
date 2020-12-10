/*
** Copyright 2020 Centreon
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

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <fstream>
#include <json11.hpp>

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace spdlog;
using namespace json11;

static std::map<std::string, level::level_enum> dbg_lvls{
    {"trace", level::trace}, {"debug", level::debug},
    {"info", level::info},   {"warn", level::warn},
    {"err", level::err},     {"critical", level::critical},
    {"off", level::off}};

log_v2& log_v2::instance() {
  static log_v2 instance;
  return instance;
}

log_v2::log_v2() {
  auto null_sink = std::make_shared<sinks::null_sink_mt>();
  _tls_log = std::make_shared<logger>("tls", null_sink);
  _bbdo_log = std::make_shared<logger>("bbdo", null_sink);
  _tcp_log = std::make_shared<logger>("tcp", null_sink);
  _core_log = std::make_shared<logger>("core", null_sink);
  _config_log = std::make_shared<logger>("config", null_sink);
  _sql_log = std::make_shared<logger>("sql", null_sink);
  _perfdata_log = std::make_shared<logger>("perfdata", null_sink);
  _lua_log = std::make_shared<logger>("lua", null_sink);
  _processing_log = std::make_shared<logger>("processing", null_sink);
  _bam_log = std::make_shared<logger>("bam", null_sink);
}

log_v2::~log_v2() {
  _core_log->info("log finished");
}

static auto json_validate = [](Json const& js) -> bool {
  if (!js.is_object() || !js["console"].is_bool() || !js["loggers"].is_array())
    return false;

  for (auto& log : js["loggers"].array_items()) {
    if (!log.is_object() || !log["name"].is_string() ||
        !log["level"].is_string())
      return false;

    std::string lvl{log["level"].string_value()};

    if (dbg_lvls.find(lvl) == dbg_lvls.end())
      return false;
  }

  return true;
};

bool log_v2::load(std::string const& file,
                  std::string const& broker_name,
                  std::string& err) {
  std::lock_guard<std::mutex> lock(_load_m);

  std::ifstream my_file(file);
  if (my_file.good()) {
    std::string const& json_to_parse{std::istreambuf_iterator<char>(my_file),
                                     std::istreambuf_iterator<char>()};

    Json const& js{Json::parse(json_to_parse, err)};
    if (!err.empty())
      return false;

    if (json_validate(js)) {
      // reset loggers to null sink
      std::vector<sink_ptr> sinks{std::make_shared<sinks::null_sink_mt>()};

      if (js["console"].bool_value())
        sinks.push_back(std::make_shared<sinks::stdout_color_sink_mt>());

      if (js["log_path"].is_string()) {
        std::string log_name =
            js["log_path"].string_value() + "/" + broker_name + ".log";
        try {
          sinks.push_back(
              std::make_shared<sinks::basic_file_sink_mt>(log_name));
        } catch (...) {
          err = "log_v2 cannot log on '" + log_name + "'";
          return false;
        }
      }

      _core_log = std::make_shared<logger>("core", sinks.begin(), sinks.end());
      _core_log->set_level(level::info);
      _core_log->flush_on(level::info);
      _core_log->info("{} : log started", broker_name);

      for (auto& entry : js["loggers"].array_items()) {
        std::shared_ptr<logger>* l;
        if (entry["name"].string_value() == "core")
          l = &_core_log;
        else if (entry["name"].string_value() == "config")
          l = &_config_log;
        else if (entry["name"].string_value() == "tls")
          l = &_tls_log;
        else if (entry["name"].string_value() == "tcp")
          l = &_tcp_log;
        else if (entry["name"].string_value() == "bbdo")
          l = &_bbdo_log;
        else if (entry["name"].string_value() == "sql")
          l = &_sql_log;
        else if (entry["name"].string_value() == "perfdata")
          l = &_perfdata_log;
        else if (entry["name"].string_value() == "lua")
          l = &_lua_log;
        else if (entry["name"].string_value() == "processing")
          l = &_processing_log;
        else if (entry["name"].string_value() == "bam")
          l = &_bam_log;
        else
          continue;

        *l = std::make_shared<logger>(entry["name"].string_value(),
                                      sinks.begin(), sinks.end());
        (*l)->set_level(dbg_lvls[entry["level"].string_value()]);
        (*l)->flush_on(dbg_lvls[entry["level"].string_value()]);
      }

      return true;
    }

    err = "bad format for config file '" + file + "'";
    return false;
  }

  err = "file '" + file + "' does not exist";
  return false;
}

std::shared_ptr<spdlog::logger> log_v2::core() {
  return instance()._core_log;
}

std::shared_ptr<spdlog::logger> log_v2::config() {
  return instance()._config_log;
}

std::shared_ptr<spdlog::logger> log_v2::tls() {
  return instance()._tls_log;
}

std::shared_ptr<spdlog::logger> log_v2::bbdo() {
  return instance()._bbdo_log;
}

std::shared_ptr<spdlog::logger> log_v2::tcp() {
  return instance()._tcp_log;
}

std::shared_ptr<spdlog::logger> log_v2::sql() {
  return instance()._sql_log;
}

std::shared_ptr<spdlog::logger> log_v2::perfdata() {
  return instance()._perfdata_log;
}

std::shared_ptr<spdlog::logger> log_v2::lua() {
  return instance()._lua_log;
}

std::shared_ptr<spdlog::logger> log_v2::processing() {
  return instance()._processing_log;
}

std::shared_ptr<spdlog::logger> log_v2::bam() {
  return instance()._bam_log;
}
