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

#include <spdlog/sinks/file_sinks.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <fstream>
#include <json11.hpp>

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
}

static auto json_validate = [](Json const& js) -> bool {
  if (!js.is_object() || !js["console"].is_bool() || !js["loggers"].is_array())
    return false;

  for (auto& log : js["loggers"].array_items()) {
    if (!log["name"].is_string() || !log["level"].is_string())
      return false;

    std::string lvl{log["level"].is_string()};

    if (dbg_lvls.find(lvl) == dbg_lvls.end())
      return false;
  }

  return true;
};

bool log_v2::load(std::string const& file) {
  std::ifstream my_file(file);
  if (my_file.good()) {
    std::string const& json_to_parse{std::istreambuf_iterator<char>(my_file),
                                     std::istreambuf_iterator<char>()};
    std::string err;

    Json const& js{Json::parse(json_to_parse, err)};
    if (json_validate(js)) {
      // reset loggers to null sink
      auto null_sink = std::make_shared<sinks::null_sink_mt>();
      _tls_log = std::make_shared<logger>("tls", null_sink);
      _bbdo_log = std::make_shared<logger>("bbdo", null_sink);
      _tcp_log = std::make_shared<logger>("tcp", null_sink);

      level::off;

      std::vector<sink_ptr> sinks;
      sinks.push_back(std::make_shared<sinks::ansicolor_stdout_sink_mt>());
      if (js["log_file"].is_string())
        sinks.push_back(std::make_shared<sinks::simple_file_sink_mt>(
            js["log_file"].string_value()));

      for (auto& entry : js["loggers"].array_items()) {
        std::shared_ptr<logger>* l;
        if (entry["name"].string_value() == "tls")
          l = &_tls_log;
        else if (entry["name"].string_value() == "tcp")
          l = &_tcp_log;
        else if (entry["name"].string_value() == "bbdo")
          l = &_bbdo_log;
        else
          continue;

        *l = std::make_shared<logger>("tls", sinks.begin(), sinks.end());
        (*l)->set_level(dbg_lvls[entry["level"].string_value()]);
      }

      return true;
    }
  }
  return false;
}

std::shared_ptr<spdlog::logger> log_v2::tls() {
  return _tls_log;
}

std::shared_ptr<spdlog::logger> log_v2::bbdo() {
  return _bbdo_log;
}

std::shared_ptr<spdlog::logger> log_v2::tcp() {
  return _tcp_log;
}