/*
** Copyright 2011-2013,2015,2017-2021 Centreon
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

#include "com/centreon/broker/config/parser.hh"

#include <syslog.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <streambuf>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/defines.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::config;
using namespace nlohmann;

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Destructor.
 */
parser::~parser() {}

template <typename T, typename U>
static bool get_conf(std::pair<std::string const, json> const& obj,
                     std::string key,
                     U& s,
                     void (U::*set_state)(T),
                     bool (json::*is_goodtype)() const,
                     T (json::*get_value)() const) {
  if (obj.first == key) {
    json const& value = obj.second;
    if ((value.*is_goodtype)())
      (s.*set_state)((value.*get_value)());
    else
      throw msg_fmt(
          "config parser: cannot parse key '{}': "
          "value type is invalid",
          key);
    ;
    return true;
  }
  return false;
}

template <typename U>
static bool get_conf(std::pair<std::string const, json> const& obj,
                     std::string key,
                     U& s,
                     void (U::*set_state)(const std::string&),
                     bool (json::*is_goodtype)() const) {
  if (obj.first == key) {
    json const& value = obj.second;
    if ((value.*is_goodtype)())
      (s.*set_state)(value.get<std::string>());
    else
      throw msg_fmt(
          "config parser: cannot parse key '{}': "
          "value type is invalid",
          key);
    ;
    return true;
  }
  return false;
}

/**
 *  Parse a configuration file.
 *
 *  @param[in]  file File to process.
 *
 *  @return a state corresponding to the json file processed.
 */
state parser::parse(std::string const& file) {
  state retval;
  // Parse JSON document.
  std::ifstream f(file);

  if (f.fail())
    throw msg_fmt("Config parser: Cannot read file '{}': {}", file,
                  std::strerror(errno));

  std::string const& json_to_parse{std::istreambuf_iterator<char>(f),
                                   std::istreambuf_iterator<char>()};
  std::string err;
  nlohmann::json json_document;

  try {
    json_document = json::parse(json_to_parse);
  } catch (const json::parse_error& e) {
    err = e.what();
  }

  if (json_document.is_null())
    throw msg_fmt("Config parser: Cannot parse file '{}': {}", file, err);

  try {
    if (json_document.is_object() &&
        json_document["centreonBroker"].is_object()) {
      for (auto it = json_document["centreonBroker"].begin();
           it != json_document["centreonBroker"].end(); ++it) {
        if (it.key() == "command_file" && it.value().is_object())
          ;
        else if (get_conf<int, state>({it.key(), it.value()}, "broker_id",
                                      retval, &state::broker_id,
                                      &json::is_number, &json::get<int>))
          ;
        else if (it.key() == "grpc" && it.value().is_object()) {
          if (json_document["centreonBroker"]["grpc"]["rpc_port"].is_number()) {
            retval.rpc_port(static_cast<uint16_t>(
                json_document["centreonBroker"]["grpc"]["rpc_port"]
                    .get<int>()));
          }
        } else if (get_conf<state>({it.key(), it.value()}, "broker_name",
                                   retval, &state::broker_name,
                                   &json::is_string))
          ;
        else if (get_conf<int, state>({it.key(), it.value()}, "poller_id",
                                      retval, &state::poller_id,
                                      &json::is_number, &json::get<int>))
          ;
        else if (get_conf<bool, state>({it.key(), it.value()}, "log_thread_id",
                                       retval, &state::log_thread_id,
                                       &json::is_boolean, &json::get<bool>))
          ;
        else if (get_conf<state>({it.key(), it.value()}, "poller_name", retval,
                                 &state::poller_name, &json::is_string))
          ;
        else if (get_conf<state>({it.key(), it.value()}, "module_directory",
                                 retval, &state::module_directory,
                                 &json::is_string))
          ;
        else if (get_conf<state>({it.key(), it.value()}, "cache_directory",
                                 retval, &state::cache_directory,
                                 &json::is_string))
          ;
        else if (get_conf<int, state>({it.key(), it.value()}, "pool_size",
                                      retval, &state::pool_size,
                                      &json::is_number, &json::get<int>))
          ;
        else if (get_conf<state>({it.key(), it.value()}, "command_file", retval,
                                 &state::command_file, &json::is_string))
          ;
        else if (get_conf<int, state>({it.key(), it.value()},
                                      "event_queue_max_size", retval,
                                      &state::event_queue_max_size,
                                      &json::is_number, &json::get<int>))
          ;
        else if (get_conf<bool, state>({it.key(), it.value()}, "log_thread_id",
                                       retval, &state::log_thread_id,
                                       &json::is_boolean, &json::get<bool>))
          ;
        else if (get_conf<bool, state>({it.key(), it.value()},
                                       "log_human_readable_timestamp", retval,
                                       &state::log_human_readable_timestamp,
                                       &json::is_boolean, &json::get<bool>))
          ;
        else if (it.key() == "output") {
          if (it.value().is_array()) {
            for (json const& node : it.value()) {
              endpoint out(endpoint::io_type::output);
              out.read_filters.insert("all");
              out.write_filters.insert("all");
              _parse_endpoint(node, out);
              retval.endpoints().push_back(out);
            }
          } else if (it.value().is_object()) {
            endpoint out(endpoint::io_type::output);
            out.read_filters.insert("all");
            out.write_filters.insert("all");
            _parse_endpoint(it.value(), out);
            retval.endpoints().push_back(out);
          } else
            throw msg_fmt(
                "config parser: cannot parse key '"
                "'output':  value type must be an object");
        }

        else if (it.key() == "input") {
          if (it.value().is_array()) {
            for (json const& node : it.value()) {
              endpoint in(endpoint::io_type::input);
              in.read_filters.insert("all");
              _parse_endpoint(node, in);
              retval.endpoints().push_back(in);
            }
          } else if (it.value().is_object()) {
            endpoint in(endpoint::io_type::input);
            in.read_filters.insert("all");
            _parse_endpoint(it.value(), in);
            retval.endpoints().push_back(in);
          } else
            throw msg_fmt(
                "config parser: cannot parse key '"
                "'input':  value type must be an object");

        }

        else if (it.key() == "log") {
          if (!it.value().is_object())
            throw msg_fmt(
                "config parser: cannot parse key "
                "'log': value type must be an object");

          const json& conf_js = it.value();
          if (!conf_js.is_object())
            throw msg_fmt("the log configuration should be a json object");

          auto& conf = retval.log_conf();
          if (conf_js.contains("directory") && conf_js["directory"].is_string())
            conf.directory = conf_js["directory"].get<std::string>();
          else if (conf_js.contains("directory") &&
                   !conf_js["directory"].is_null())
            throw msg_fmt(
                "'directory' key in the log configuration must contain a "
                "directory name");
          if (conf.directory.empty())
            conf.directory = "/var/log/centreon-broker";

          conf.filename = "";

          if (conf_js.contains("filename") && conf_js["filename"].is_string()) {
            conf.filename = conf_js["filename"].get<std::string>();
            if (conf.filename.find("/") != std::string::npos)
              throw msg_fmt(
                  "'filename' must only contain a filename without directory");

          } else if (conf_js.contains("filename") &&
                     !conf_js["filename"].is_null())
            throw msg_fmt(
                "'filename' key in the log configuration must contain the log "
                "file name");

          conf.max_size = 0u;

          if (conf_js.contains("max_size") && conf_js["max_size"].is_string()) {
            try {
              conf.max_size =
                  std::stoul(conf_js["max_size"].get<std::string>());
            } catch (const std::exception& e) {
              throw msg_fmt(
                  "'max_size' key in the log configuration must contain a size "
                  "in bytes");
            }
          } else if (conf_js.contains("max_size") &&
                     conf_js["max_size"].is_number()) {
            int64_t tmp = conf_js["max_size"].get<int>();
            if (tmp < 0)
              throw msg_fmt(
                  "'max_size' key in the log configuration must contain a "
                  "positive number.");
            conf.max_size = tmp;
          } else if (conf_js.contains("max_size") &&
                     !conf_js["max_size"].is_null())
            throw msg_fmt(
                "'max_size' key in the log configuration must contain a size "
                "in "
                "bytes (as number or string)");

          if (conf_js.contains("loggers") && conf_js["loggers"].is_object()) {
            conf.loggers.clear();
            for (auto it = conf_js["loggers"].begin();
                 it != conf_js["loggers"].end(); ++it) {
              const auto& loggers = log_v2::loggers;
              const auto levels = log_v2::levels();
              if (std::find(loggers.begin(), loggers.end(), it.key()) ==
                  loggers.end())
                throw msg_fmt("'{}' is not available as logger", it.key());
              if (!it.value().is_string() ||
                  std::find(levels.begin(), levels.end(),
                            it.value().get<std::string>()) == levels.end())
                throw msg_fmt(
                    "The logger '{}' must contain a string among 'trace', "
                    "'debug', 'info', 'warning', 'error', 'critical', "
                    "'disabled'",
                    it.key());

              conf.loggers.emplace(it.key(), it.value().get<std::string>());
            }
          }
        }

        else if (it.key() == "logger") {
          if (it.value().is_array()) {
            for (json const& node : it.value()) {
              logger logr;
              _parse_logger(node, logr);
              retval.loggers().push_back(logr);
            }
          } else if (it.value().is_object()) {
            logger logr;
            _parse_logger(it.value(), logr);
            retval.loggers().push_back(logr);
          } else {
            throw msg_fmt(
                "config parser: cannot parse key "
                "'logger':  value type must be an object");
          }
        } else
          retval.params()[it.key()] = it.value().dump();
      }
    }
  } catch (const json::parse_error& e) {
    throw msg_fmt("Config parser: Cannot parse the file '{}': {}", file,
                  e.what());
  }

  /* Post configuration */
  auto& conf = retval.log_conf();
  if (conf.filename.empty())
    conf.filename = fmt::format("{}.log", retval.broker_name());
  return retval;
}

/**
 *  Parse a boolean value.
 *
 *  @param[in] value String representation of the boolean.
 */
bool parser::parse_boolean(std::string const& value) {
  if (std::all_of(value.begin(), value.end(), ::isdigit))
    return std::stol(value);

  return !strcasecmp(value.c_str(), "yes") ||
         !strcasecmp(value.c_str(), "enable") ||
         !strcasecmp(value.c_str(), "enabled") ||
         !strcasecmp(value.c_str(), "true") || false;
}

/**
 *  Parse the configuration of an endpoint.
 *
 *  @param[in]  elem XML element that have the endpoint configuration.
 *  @param[out] e    Element object.
 */
void parser::_parse_endpoint(json const& elem, endpoint& e) {
  e.cfg = elem;

  for (auto it = elem.begin(); it != elem.end(); ++it) {
    if (it.key() == "buffering_timeout")
      e.buffering_timeout =
          static_cast<time_t>(std::stoul(it.value().get<std::string>()));
    else if (it.key() == "failover")
      e.failovers.push_back(it.value().get<std::string>());
    else if (it.key() == "name")
      e.name = it.value().get<std::string>();
    else if (it.key() == "read_timeout")
      e.read_timeout =
          static_cast<time_t>(std::stoi(it.value().get<std::string>()));
    else if (it.key() == "retry_interval") {
      try {
        e.retry_interval =
            static_cast<uint32_t>(std::stoul(it.value().get<std::string>()));
      } catch (const std::exception& e) {
        throw msg_fmt(
            "config parser: cannot parse key "
            "'retry_interval': value must be an integer");
      }
    }

    else if (it.key() == "filters") {
      std::set<std::string> endpoint::*member;
      if (e.write_filters.empty())  // Input.
        member = &endpoint::read_filters;
      else  // Output.
        member = &endpoint::write_filters;
      (e.*member).clear();
      if (it.value().is_object() && it.value()["category"].is_array())
        for (auto& cat : it.value()["category"])
          (e.*member).insert(cat.get<std::string>());
      else if (it.value().is_object() && it.value()["category"].is_string())
        (e.*member).insert(it.value()["category"].get<std::string>());
      else if (it.value().is_string() && it.value().get<std::string>() == "all")
        (e.*member).insert("all");
      else
        throw msg_fmt(
            "config parser: cannot parse key "
            "'filters':  value is invalid");
    } else if (it.key() == "cache")
      e.cache_enabled = parse_boolean(it.value().get<std::string>());
    else if (it.key() == "type")
      e.type = it.value().get<std::string>();
    if (it.value().is_string())
      e.params[it.key()] = it.value().get<std::string>();
    else
      std::cout << "for key: " << it.key() << " value is not a string."
                << std::endl;
  }
}

/**
 *  Parse the configuration of a logging object.
 *
 *  @param[in]  elem json element that have the logger configuration.
 *  @param[out] l    Logger object.
 */
void parser::_parse_logger(json const& elem, logger& l) {
  for (auto it = elem.begin(); it != elem.end(); ++it) {
    if (it.key() == "config" && it.value().is_string()) {
    } else if (it.key() == "debug" && it.value().is_string()) {
      if (it.value().get<std::string>() == "yes")
        l.config(true);
      else
        l.config(false);
    } else if (it.key() == "error" && it.value().is_string()) {
      if (it.value().get<std::string>() == "yes")
        l.config(true);
      else
        l.config(false);
    } else if (it.key() == "info" && it.value().is_string()) {
      if (it.value().get<std::string>() == "yes")
        l.config(true);
      else
        l.config(false);
    } else if (it.key() == "perf" && it.value().is_string()) {
      if (it.value().get<std::string>() == "yes")
        l.config(true);
      else
        l.config(false);
    } else if (get_conf<bool, logger>({it.key(), it.value()}, "config", l,
                                      &logger::config, &json::is_boolean,
                                      &json::get<bool>))
      ;
    else if (get_conf<bool, logger>({it.key(), it.value()}, "debug", l,
                                    &logger::debug, &json::is_boolean,
                                    &json::get<bool>))
      ;
    else if (get_conf<bool, logger>({it.key(), it.value()}, "error", l,
                                    &logger::error, &json::is_boolean,
                                    &json::get<bool>))
      ;
    else if (get_conf<bool, logger>({it.key(), it.value()}, "info", l,
                                    &logger::info, &json::is_boolean,
                                    &json::get<bool>))
      ;
    else if (get_conf<bool, logger>({it.key(), it.value()}, "perf", l,
                                    &logger::perf, &json::is_boolean,
                                    &json::get<bool>))
      ;
    else if (it.key() == "facility") {
      std::string const& val{it.value().get<std::string>()};
      if (!strcasecmp(val.c_str(), "kern"))
        l.facility(LOG_KERN);
      else if (!strcasecmp(val.c_str(), "user"))
        l.facility(LOG_USER);
      else if (!strcasecmp(val.c_str(), "mail"))
        l.facility(LOG_MAIL);
      else if (!strcasecmp(val.c_str(), "news"))
        l.facility(LOG_NEWS);
      else if (!strcasecmp(val.c_str(), "uucp"))
        l.facility(LOG_UUCP);
      else if (!strcasecmp(val.c_str(), "daemon"))
        l.facility(LOG_DAEMON);
      else if (!strcasecmp(val.c_str(), "auth"))
        l.facility(LOG_AUTH);
      else if (!strcasecmp(val.c_str(), "cron"))
        l.facility(LOG_CRON);
      else if (!strcasecmp(val.c_str(), "lpr"))
        l.facility(LOG_LPR);
      else if (!strcasecmp(val.c_str(), "local0"))
        l.facility(LOG_LOCAL0);
      else if (!strcasecmp(val.c_str(), "local1"))
        l.facility(LOG_LOCAL1);
      else if (!strcasecmp(val.c_str(), "local2"))
        l.facility(LOG_LOCAL2);
      else if (!strcasecmp(val.c_str(), "local3"))
        l.facility(LOG_LOCAL3);
      else if (!strcasecmp(val.c_str(), "local4"))
        l.facility(LOG_LOCAL4);
      else if (!strcasecmp(val.c_str(), "local5"))
        l.facility(LOG_LOCAL5);
      else if (!strcasecmp(val.c_str(), "local6"))
        l.facility(LOG_LOCAL6);
      else if (!strcasecmp(val.c_str(), "local7"))
        l.facility(LOG_LOCAL7);
      else
        l.facility(std::stoul(val));
    } else if (it.key() == "level") {
      std::string const& val_str = {it.value().get<std::string>()};
      int val{0};
      if (misc::string::is_number(val_str))
        val = std::stoi(val_str);

      if ((val == 3) || (val_str == "high"))
        l.level(com::centreon::broker::logging::low);
      else if ((val == 2) || (val_str == "medium"))
        l.level(com::centreon::broker::logging::medium);
      else if ((val == 1) || (val_str == "low"))
        l.level(com::centreon::broker::logging::high);
      else
        l.level(com::centreon::broker::logging::none);
    } else if (it.key() == "max_size")
      l.max_size(std::stoul(it.value().get<std::string>()));
    else if (it.key() == "name")
      l.name(it.value().get<std::string>());
    else if (it.key() == "type") {
      std::string const& val{it.value().get<std::string>()};
      if (val == "file")
        l.type(logger::file);
      else if (val == "monitoring")
        l.type(logger::monitoring);
      else if (val == "standard")
        l.type(logger::standard);
      else if (val == "syslog")
        l.type(logger::syslog);
      else
        throw msg_fmt("config parser: unknown logger type '{}'", val);
    }
  }
}
