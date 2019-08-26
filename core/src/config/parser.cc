/*
** Copyright 2011-2013,2015,2017 Centreon
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

#include <json11.hpp>
#include <fstream>
#include <streambuf>
#include <syslog.h>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/defines.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config;
using namespace json11;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  Destructor.
 */
parser::~parser() {}

template <typename T, typename U>
static bool get_conf(std::pair<std::string const, Json> const& obj,
                            std::string key,
                            U &s,
                            void (U::*set_state)(T),
                            bool (Json::*is_goodtype)() const,
                            T (Json::*get_value)() const) {
  if (obj.first == key) {
    Json const& value{obj.second};
    if ((value.*is_goodtype)())
      (s.*set_state)((value.*get_value)());
    else
      throw exceptions::msg() << "config parser: cannot parse key '"
        << key << "': " << "value type is invalid";;
    return true;
  }

  return false;
}

/**
 *  Parse a configuration file.
 *
 *  @param[in]  file File to process.
 *  @param[out] s    Resulting configuration state.
 */
void parser::parse(std::string const& file, state& s) {
  // Parse JSON document.
  std::ifstream f(file);
  std::string const& json_to_parse{
    std::istreambuf_iterator<char>(f),
    std::istreambuf_iterator<char>()};
  std::string err;

  Json const& js{Json::parse(json_to_parse, err)};

  if (js.is_null())
    throw exceptions::msg() << "config parser: cannot parse file '"
             << file << "': " << err;

  // Clear state.
  s.clear();

  if (js.is_object() && js["centreonBroker"].is_object()) {

    for (std::pair<std::string const, Json> const& object : js["centreonBroker"].object_items()) {
      if (get_conf<int, state>(object, "broker_id", s,
        &state::broker_id, &Json::is_number, &Json::int_value))
        ;
      else if (get_conf<std::string const&, state>(object, "broker_name", s,
        &state::broker_name, &Json::is_string, &Json::string_value))
        ;
      else if (get_conf<int, state>(object, "poller_id", s,
        &state::poller_id, &Json::is_number, &Json::int_value))
        ;
      else if (get_conf<std::string const&, state>(object, "poller_name", s,
        &state::broker_name, &Json::is_string, &Json::string_value))
        ;
      else if (get_conf<std::string const&, state>(object, "module_directory", s,
        &state::module_directory, &Json::is_string, &Json::string_value))
        ;
      else if (get_conf<std::string const&, state>(object, "cache_directory", s,
        &state::cache_directory, &Json::is_string, &Json::string_value))
        ;
      else if (get_conf<std::string const&, state>(object, "command_file", s,
        &state::command_file, &Json::is_string, &Json::string_value))
        ;
      else if (get_conf<int, state>(object, "event_queue_max_size", s,
        &state::event_queue_max_size, &Json::is_number, &Json::int_value))
        ;
      else if (get_conf<bool, state>(object, "log_thread_id", s,
        &state::log_thread_id, &Json::is_bool, &Json::bool_value))
        ;
      else if (get_conf<bool, state>(object, "log_human_readable_timestamp", s,
        &state::log_human_readable_timestamp, &Json::is_bool, &Json::bool_value))
        ;
      else if (object.first == "output") {
        if (object.second.is_array()) {
          for(Json const& node : object.second.array_items()) {
            endpoint out;
            out.read_filters.insert("all");
            out.write_filters.insert("all");
            _parse_endpoint(node, out);
            s.endpoints().push_back(out);
          }
        }
        else if(object.second.is_object()) {
          endpoint out;
          out.read_filters.insert("all");
          out.write_filters.insert("all");
          _parse_endpoint(object.second, out);
          s.endpoints().push_back(out);
        } else {
          throw exceptions::msg() << "config parser: cannot parse key '"
            <<"'output':  value type must be an object";
        }
      }

      else if (object.first == "input") {
        if (object.second.is_array()) {
          for(Json const& node : object.second.array_items()) {
            endpoint in;
            in.read_filters.insert("all");
            _parse_endpoint(node, in);
            s.endpoints().push_back(in);
          }
        }
        else if (object.second.is_object()) {
          endpoint in;
          in.read_filters.insert("all");
          _parse_endpoint(object.second, in);
          s.endpoints().push_back(in);
        } else {
          throw exceptions::msg() << "config parser: cannot parse key '"
            <<"'input':  value type must be an object";
        }
      }

      else if (object.first == "logger") {
        if (object.second.is_array()) {
          for(Json const& node : object.second.array_items()) {
            logger logr;
            _parse_logger(node, logr);
            s.loggers().push_back(logr);
          }
        }
        else if (object.second.is_object()) {
          logger logr;
          _parse_logger(object.second, logr);
          s.loggers().push_back(logr);
        } else {
          throw exceptions::msg() << "config parser: cannot parse key "
            <<"'logger':  value type must be an object";
        }
      }
      else
        s.params()[object.first] = object.second.dump();
    }
  }
}

/**
 *  Parse a boolean value.
 *
 *  @param[in] value String representation of the boolean.
 */
bool parser::parse_boolean(std::string const& value) {
  return !strcasecmp(value.c_str(), "yes")
          || !strcasecmp(value.c_str(), "enable")
          || !strcasecmp(value.c_str(), "enabled")
          || !strcasecmp(value.c_str(), "true")
          || std::stol(value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Parse the configuration of an endpoint.
 *
 *  @param[in]  elem XML element that have the endpoint configuration.
 *  @param[out] e    Element object.
 */
void parser::_parse_endpoint(Json const& elem, endpoint& e) {
  e.cfg = elem;

  for (std::pair<std::string const, Json> const& object : elem.object_items()) {
    if (object.first == "buffering_timeout")
      e.buffering_timeout = static_cast<time_t>(std::stoul(object.second.string_value()));
    else if (object.first == "failover")
      e.failovers.push_back(object.second.string_value());
    else if (object.first == "name")
      e.name = object.second.string_value();
    else if (object.first == "read_timeout")
      e.read_timeout = static_cast<time_t>(std::stoi(object.second.string_value()));
    else if (object.first == "retry_interval")
      e.retry_interval = static_cast<time_t>(std::stoul(object.second.string_value()));
    else if (object.first == "filters") {
      std::set<std::string> (endpoint::* member);
      if (e.write_filters.empty()) // Input.
        member = &endpoint::read_filters;
      else // Output.
        member = &endpoint::write_filters;
      (e.*member).clear();
      if (object.second.is_object() && object.second["category"].is_array())
        for (auto & cat : object.second["category"].array_items())
          (e.*member).insert(cat.string_value());
      else if (object.second.is_string() && object.second["all"].is_string()) {
        (e.*member).insert("all");
      }
    }
    else if (object.first == "cache")
      e.cache_enabled = parse_boolean(object.second.string_value());
    else if (object.first == "type")
      e.type = QString::fromStdString(object.second.string_value());
    e.params[object.first] = object.second.string_value();
  }
}

/**
 *  Parse the configuration of a logging object.
 *
 *  @param[in]  elem XML element that have the logger configuration.
 *  @param[out] l    Logger object.
 */
void parser::_parse_logger(Json const& elem, logger& l) {
  for (std::pair<std::string const, Json> const& object : elem.object_items()) {
    if (get_conf<bool, logger>(object, "config", l,
      &logger::config, &Json::is_bool, &Json::bool_value))
      ;
    else if (get_conf<bool, logger>(object, "debug", l,
      &logger::debug, &Json::is_bool, &Json::bool_value))
      ;
    else if (get_conf<bool, logger>(object, "error", l,
      &logger::error, &Json::is_bool, &Json::bool_value))
      ;
    else if (get_conf<bool, logger>(object, "info", l,
      &logger::info, &Json::is_bool, &Json::bool_value))
      ;
    else if (get_conf<bool, logger>(object, "perf", l,
      &logger::perf, &Json::is_bool, &Json::bool_value))
      ;
    else if (get_conf<bool, logger>(object, "perf", l,
      &logger::perf, &Json::is_bool, &Json::bool_value))
      ;
    else if (object.first == "facility") {
      QString const& val{QString::fromStdString(object.second.string_value())};
      if (!val.compare("kern", Qt::CaseInsensitive))
          l.facility(LOG_KERN);
      else if (!val.compare("user", Qt::CaseInsensitive))
        l.facility(LOG_USER);
      else if (!val.compare("mail", Qt::CaseInsensitive))
        l.facility(LOG_MAIL);
      else if (!val.compare("news", Qt::CaseInsensitive))
        l.facility(LOG_NEWS);
      else if (!val.compare("uucp", Qt::CaseInsensitive))
        l.facility(LOG_UUCP);
      else if (!val.compare("daemon", Qt::CaseInsensitive))
        l.facility(LOG_DAEMON);
      else if (!val.compare("auth", Qt::CaseInsensitive))
        l.facility(LOG_AUTH);
      else if (!val.compare("cron", Qt::CaseInsensitive))
        l.facility(LOG_CRON);
      else if (!val.compare("lpr", Qt::CaseInsensitive))
        l.facility(LOG_LPR);
      else if (!val.compare("local0", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL0);
      else if (!val.compare("local1", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL1);
      else if (!val.compare("local2", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL2);
      else if (!val.compare("local3", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL3);
      else if (!val.compare("local4", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL4);
      else if (!val.compare("local5", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL5);
      else if (!val.compare("local6", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL6);
      else if (!val.compare("local7", Qt::CaseInsensitive))
        l.facility(LOG_LOCAL7);
      else
        l.facility(val.toUInt());
    }
    else if (object.first == "level") {
      const QString& val_str = {QString::fromStdString(object.second.string_value())};
      int val(val_str.toInt());
      if ((val == 3) || (val_str == "high"))
        l.level(com::centreon::broker::logging::low);
      else if ((val == 2) || (val_str == "medium"))
        l.level(com::centreon::broker::logging::medium);
      else if ((val == 1) || (val_str == "low"))
        l.level(com::centreon::broker::logging::high);
      else
        l.level(com::centreon::broker::logging::none);
    }
    else if (object.first == "max_size")
      l.max_size(std::stoul(object.second.string_value()));
    else if (object.first == "name")
      l.name(QString::fromStdString(object.second.string_value()));
    else if (object.first == "type") {
      const QString & val{QString::fromStdString(object.second.string_value())};
      if (val == "file")
        l.type(logger::file);
      else if (val == "monitoring")
        l.type(logger::monitoring);
      else if (val == "standard")
        l.type(logger::standard);
      else if (val == "syslog")
        l.type(logger::syslog);
      else
        throw (exceptions::msg()
               << "config parser: unknown logger type '"
               << val << "'");
    }
  }
}
