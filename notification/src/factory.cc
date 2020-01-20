/*
** Copyright 2011-2016 Centreon
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

#include "com/centreon/broker/notification/factory.hh"
#include <memory>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/connector.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**************************************
 *                                     *
 *            Local Objects            *
 *                                     *
 **************************************/

/**
 *  Find a parameter in configuration.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *
 *  @return Property value.
 */
static std::string const& find_param(config::endpoint const& cfg,
                                     std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw exceptions::msg() << "notification: no '" << key
                            << "' defined for endpoint '" << cfg.name << "'";
  return it->second;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if an endpoint match a configuration.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if the endpoint match the configuration.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_notif(!cfg.type.compare("notification", Qt::CaseInsensitive));
  if (is_notif) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return (is_notif);
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       The persistent cache for this module.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  // Find DB type.
  std::string type(find_param(cfg, "db_type"));

  // Find DB host.
  std::string host(find_param(cfg, "db_host"));

  // Find DB port.
  unsigned short port{
      static_cast<unsigned short>(std::stol(find_param(cfg, "db_port")))};

  // Find DB user.
  std::string user{find_param(cfg, "db_user")};

  // Find DB password.
  std::string password{find_param(cfg, "db_password")};

  // Find DB name.
  std::string db_name{find_param(cfg, "db_name")};

  // Check replication status ?
  bool check_replication{true};
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("check_replication")};
    if (it != cfg.params.end())
      check_replication = config::parser::parse_boolean(it->second);
  }

  // Connector.
  std::unique_ptr<notification::connector> c{
      new notification::connector(cache)};
  c->connect_to(type, host, port, user, password, db_name, check_replication);
  is_acceptor = false;
  return c.release();
}
