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

#include "com/centreon/broker/graphite/factory.hh"
#include <cstring>
#include <memory>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/graphite/connector.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;
using namespace com::centreon::exceptions;

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
static std::string find_param(config::endpoint const& cfg,
                              std::string const& key) {
  std::map<std::string, std::string>::const_iterator it{cfg.params.find(key)};
  if (cfg.params.end() == it)
    throw msg_fmt("graphite: no '{}' defined for endpoint '{}'", key, cfg.name);
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
static std::string get_string_param(config::endpoint const& cfg,
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
static uint32_t get_uint_param(config::endpoint const& cfg,
                               std::string const& key,
                               uint32_t def) {
  std::map<std::string, std::string>::const_iterator it(cfg.params.find(key));
  if (cfg.params.end() == it)
    return (def);
  else {
    try {
      return std::stoul(it->second);
    } catch (std::exception const& ex) {
      throw msg_fmt("graphite: '{}' must be numeric for endpoint '{}'", key,
                    cfg.name);
    }
  }

  return 0;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if a configuration match the storage layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return true if the configuration matches the storage layer.
 */
bool factory::has_endpoint(config::endpoint& cfg, io::extension* ext) {
  if (ext)
    *ext = io::extension("GRAPHITE", false, false);
  bool is_gpdb{!strncasecmp(cfg.type.c_str(), "graphite", 9)};
  if (is_gpdb) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_gpdb;
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
  unsigned short db_port(get_uint_param(cfg, "db_port", 2003));
  std::string db_user(get_string_param(cfg, "db_user", ""));
  std::string db_password(get_string_param(cfg, "db_password", ""));
  uint32_t queries_per_transaction(
      get_uint_param(cfg, "queries_per_transaction", 1));
  std::string metric_naming(
      get_string_param(cfg, "metric_naming", "centreon.metrics.$METRICID$"));
  std::string status_naming(
      get_string_param(cfg, "status_naming", "centreon.statuses.$INDEXID$"));
  std::string escape_string(get_string_param(cfg, "escape_string", "_"));

  // Connector.
  std::unique_ptr<graphite::connector> c(new graphite::connector);
  c->connect_to(metric_naming, status_naming, escape_string, db_user,
                db_password, db_host, db_port, queries_per_transaction, cache);
  is_acceptor = false;
  return (c.release());
}
