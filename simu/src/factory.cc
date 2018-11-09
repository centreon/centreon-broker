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

#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/simu/connector.hh"
#include "com/centreon/broker/simu/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::simu;

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
  bool is_simu(!cfg.type.compare("simu", Qt::CaseInsensitive));
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
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  QMap<QString, QVariant> conf_map;
  std::string filename(find_param(cfg, "path"));
  QDomNodeList config = cfg.cfg.elementsByTagName("lua_parameter");
  for (int i(0); i < config.size(); ++i) {
    QDomNode conf(config.item(i));
    QDomNode name = conf.namedItem("name");
    QDomNode type = conf.namedItem("type");
    QDomNode value = conf.namedItem("value");
    if (name.isNull())
      throw (exceptions::msg())
             << "lua: couldn't read a configuration field because"
             << " its name is empty";
    if (value.isNull())
      throw (exceptions::msg())
             << "lua: couldn't read the '"
             << name.toElement().text().toStdString()
             << "' configuration field because its value is empty";
    std::string t((type.isNull())
                  ? "string" : type.toElement().text().toStdString());
    if (t == "string" || t == "password")
      conf_map.insert(name.toElement().text(), QVariant(value.toElement().text()));
    else if (t == "number") {
      bool ok;
      int val(value.toElement().text().toInt(&ok, 10));
      if (ok)
        conf_map.insert(name.toElement().text(), QVariant(val));
      else {
        double val(value.toElement().text().toDouble(&ok));
        if (ok)
          conf_map.insert(name.toElement().text(), QVariant(val));
        else {
          throw (exceptions::msg())
                 << "lua: unable to read '"
                 << name.toElement().text()
                 << "' content (" << value.toElement().text()
                 << ") as a number";
        }
      }
    }
    else {
      throw (exceptions::msg())
        << "lua: unable to read '"
        << name.toElement().text()
        << "' content: type unrecognized (" << t << ")";
    }
  }
  // Connector.
  std::unique_ptr<simu::connector> c(new simu::connector);
  c->connect_to(filename, conf_map);
  is_acceptor = false;
  return c.release();
}
