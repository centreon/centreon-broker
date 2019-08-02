/*
** Copyright 2017-2018 Centreon
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
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/lua/connector.hh"
#include "com/centreon/broker/lua/factory.hh"
#include "../../../../../../usr/include/qt4/QtCore/QString"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;
using namespace json11;

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static std::string find_param(
                     config::endpoint const& cfg,
                     QString const& key) {
  QMap<QString, QString>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    throw (exceptions::msg() << "lua: no '" << key
           << "' defined for endpoint '" << cfg.name << "'");
  return it.value().toStdString();
}

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
factory::factory(factory const& other) : io::factory(other) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& other) {
  io::factory::operator=(other);
  return (*this);
}

/**
 *  Clone the factory.
 *
 *  @return Copy of the factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_lua(!cfg.type.compare("lua", Qt::CaseInsensitive));
  if (is_lua) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_lua;
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
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  QMap<QString, QVariant> conf_map;
  std::string err;

  std::string filename(find_param(cfg, "path"));
  Json const& js{cfg.cfg["lua_parameter"]};

  if (!err.empty())
    throw (exceptions::msg())
      << "lua: couldn't read a configuration json";

  if (js.is_array()) {
    for (Json const &obj : js.array_items()) {
      Json const &name{obj["name"]};
      Json const &type{obj["type"]};
      Json const &value{obj["value"]};

      if (name.string_value().empty())
        throw (exceptions::msg())
          << "lua: couldn't read a configuration field because"
          << " its name is empty";
      if (value.string_value().empty())
        throw (exceptions::msg())
          << "lua: couldn't read a configuration field because"
          << "' configuration field because its value is empty";
      std::string t((type.string_value().empty())
                    ? "string" : type.string_value());
      if (t == "string" || t == "password")
        conf_map.insert(QString::fromStdString(name.string_value()), QVariant(QString::fromStdString(value.string_value())));
      else if (t == "number") {
        bool ok;
        int val(QString::fromStdString(value.string_value()).toInt(&ok, 10));
        if (ok)
          conf_map.insert(QString::fromStdString(name.string_value()), QVariant(val));
        else {
          double val(QString::fromStdString(value.string_value()).toDouble(&ok));
          if (ok)
            conf_map.insert(QString::fromStdString(name.string_value()), QVariant(val));
          else {
            throw (exceptions::msg())
              << "lua: unable to read '"
              << name.string_value()
              << "' content (" << value.string_value()
              << ") as a number";
          }
        }
      }
    }
  }
  // Connector.
  std::unique_ptr<lua::connector> c(new lua::connector);
  c->connect_to(filename, conf_map, cache);
  is_acceptor = false;
  return c.release();
}
