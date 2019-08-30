/*
** Copyright 2011-2017 Centreon
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
#include <sstream>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/graphite/connector.hh"
#include "com/centreon/broker/graphite/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

/**************************************
*                                     *
*           Static Objects            *
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
static std::string find_param(
                     config::endpoint const& cfg,
                     std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw exceptions::msg() << "graphite: no '" << key
           << "' defined for endpoint '" << cfg.name << "'";
  return it->second;
}

/**
 *  Get a parameter in configuration, or return a default value.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *  @param[in] def The default value if nothing found.
 *
 *  @return Property value.
 */
static std::string get_string_param(
              config::endpoint const& cfg,
              std::string const& key,
              std::string const& def) {
  std::map<std::string, std::string>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    return def;
  else
    return it->second;
}

/**
 *  Get a parameter in configuration, or return a default value.
 *
 *  @param[in] cfg Configuration object.
 *  @param[in] key Property to get.
 *  @param[in] def The default value if nothing found.
 *
 *  @return Property value.
 */
static unsigned int get_uint_param(
             config::endpoint const& cfg,
             std::string const& key,
             unsigned int def) {
  std::map<std::string, std::string>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    return (def);
  else
    return std::stoul(it->second);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

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
  return *this;
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this factory.
 */
io::factory* factory::clone() const {
  return new factory(*this);
}

/**
 *  Check if a configuration match the storage layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_ifdb{!strncasecmp(cfg.type.c_str(), "graphite", 9)};
  if (is_ifdb) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_ifdb;
}

/**
 *  Build a storage endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       The persistent cache.
 *
 *  @return Endpoint matching the given configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         std::shared_ptr<persistent_cache> cache) const {
  std::string db_host(find_param(cfg, "db_host"));
  unsigned short db_port(
    get_uint_param(cfg, "db_port", 2003));
  std::string db_user(
    get_string_param(cfg, "db_user", ""));
  std::string db_password(
    get_string_param(cfg, "db_password", ""));
  unsigned int queries_per_transaction(
    get_uint_param(cfg, "queries_per_transaction", 1));
  std::string metric_naming(
    get_string_param(cfg, "metric_naming", "centreon.metrics.$METRICID$"));
  std::string status_naming(
    get_string_param(cfg, "status_naming", "centreon.statuses.$INDEXID$"));
  std::string escape_string(
    get_string_param(cfg, "escape_string", "_"));

  // Connector.
  std::unique_ptr<graphite::connector> c(new graphite::connector);
  c->connect_to(
       metric_naming,
       status_naming,
       escape_string,
       db_user,
       db_password,
       db_host,
       db_port,
       queries_per_transaction,
       cache);
  is_acceptor = false;
  return (c.release());
}
