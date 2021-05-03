/*
** Copyright 2011-2013,2015 Centreon
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

#include "com/centreon/broker/simu/factory.hh"
#include <cstring>
#include <memory>
#include <nlohmann/json.hpp>
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/simu/connector.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::simu;
using namespace nlohmann;

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static std::string find_param(config::endpoint const& cfg,
                              std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw msg_fmt("lua: no '{}' defined for endpoint '{}'", key, cfg.name);
  return it->second;
}

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint& cfg, io::extension* ext) {
  if (ext)
    *ext = io::extension("SIMU", false, false);
  bool is_simu{!strncasecmp(cfg.type.c_str(), "simu", 5)};
  if (is_simu) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_simu;
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       The persistent cache.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(config::endpoint& cfg,
                                    bool& is_acceptor,
                                    std::shared_ptr<persistent_cache> cache
                                    __attribute__((unused))) const {
  std::map<std::string, misc::variant> conf_map;
  std::string err;

  std::string filename(find_param(cfg, "path"));

  json const& js{cfg.cfg["lua_parameter"]};

  if (!err.empty())
    throw msg_fmt("simu: couldn't read a configuration json");

  if (js.is_object()) {
    json const& name{js["name"]};
    json const& type{js["type"]};
    json const& value{js["value"]};

    if (name.get<std::string>().empty())
      throw msg_fmt(
          "simu: couldn't read a configuration field because"
          " its name is empty");
    if (value.get<std::string>().empty())
      throw msg_fmt(
          "simu: couldn't read a configuration field because"
          "' configuration field because its value is empty");
    std::string t((type.get<std::string>().empty()) ? "string"
                                                    : type.get<std::string>());
    if (t == "string" || t == "password")
      conf_map.insert(
          {name.get<std::string>(), misc::variant(value.get<std::string>())});
    else if (t == "number") {
      bool ko = false;
      size_t pos;
      std::string const& v(value.get<std::string>());
      try {
        int val = std::stol(v, &pos);
        if (pos == v.size())  // All the string is read
          conf_map.insert({name.get<std::string>(), misc::variant(val)});
        else
          ko = true;
      } catch (std::exception const& e) {
        ko = true;
      }
      // Second attempt using floating point numbers
      if (ko) {
        try {
          double val = std::stod(v, &pos);
          if (pos == v.size())  // All the string is read
            conf_map.insert({name.get<std::string>(), misc::variant(val)});
          else
            ko = true;
        } catch (std::exception const& e) {
          ko = true;
        }
      }
      if (ko)
        throw msg_fmt(
            "simu: unable to read '{}' content ({}"
            ") as a number",
            name.get<std::string>(), value.get<std::string>());
    }
  } else if (js.is_array()) {
    for (json const& obj : js) {
      json const& name{obj["name"]};
      json const& type{obj["type"]};
      json const& value{obj["value"]};

      if (name.get<std::string>().empty())
        throw msg_fmt(
            "lua: couldn't read a configuration field because"
            " its name is empty");
      if (value.get<std::string>().empty())
        throw msg_fmt(
            "simu: couldn't read a configuration field because"
            "' configuration field because its value is empty");
      std::string t((type.get<std::string>().empty())
                        ? "string"
                        : type.get<std::string>());
      if (t == "string" || t == "password")
        conf_map.insert(
            {name.get<std::string>(), misc::variant(value.get<std::string>())});
      else if (t == "number") {
        try {
          int val = std::stol(value.get<std::string>());
          conf_map.insert({name.get<std::string>(), misc::variant(val)});
        } catch (std::exception const& e) {
          throw msg_fmt(
              "lua: unable to read '{}' content ({}"
              ") as a number",
              name.get<std::string>(), value.get<std::string>());
        }
      }
    }
  }
  // Connector.
  std::unique_ptr<simu::connector> c(new simu::connector);
  c->connect_to(filename, conf_map);
  is_acceptor = false;
  return c.release();
}
