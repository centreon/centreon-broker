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

#include "com/centreon/broker/sql/factory.hh"

#include <cstring>
#include <memory>

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/sql/connector.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

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
  bool is_sql{!strncasecmp(cfg.type.c_str(), "sql", 4)};
  return is_sql;
}

/**
 *  Create an endpoint.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Unused.
 *
 *  @return New endpoint.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Database configuration.
  database_config dbcfg(cfg);

  // Cleanup check interval.
  uint32_t cleanup_check_interval(0);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("cleanup_check_interval")};
    if (it != cfg.params.end())
      cleanup_check_interval = std::stoul(it->second);
  }

  bool enable_cmd_cache(false);
  {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("enable_command_cache"));
    if (it != cfg.params.end())
      enable_cmd_cache = std::stoul(it->second);
  }

  // Loop timeout
  // By default, 30 seconds
  int32_t loop_timeout = cfg.read_timeout;
  if (loop_timeout < 0)
    loop_timeout = 30;

  // Instance timeout
  // By default, 5 minutes.
  uint32_t instance_timeout(5 * 60);
  {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("instance_timeout"));
    if (it != cfg.params.end())
      instance_timeout = std::stoul(it->second);
  }

  // Use state events ?
  bool wse(false);
  {
    std::map<std::string, std::string>::const_iterator it(
        cfg.params.find("with_state_events"));
    if (it != cfg.params.end())
      wse = config::parser::parse_boolean(it->second);
  }

  // Connector.
  std::unique_ptr<sql::connector> c{new sql::connector};
  c->connect_to(dbcfg, cleanup_check_interval, loop_timeout, instance_timeout,
                wse, enable_cmd_cache);
  is_acceptor = false;
  return c.release();
}
