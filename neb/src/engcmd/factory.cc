/*
** Copyright 2013,2015 Centreon
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

#include "com/centreon/broker/neb/engcmd/factory.hh"
#include <memory>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/neb/engcmd/endpoint.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb::engcmd;

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
    throw exceptions::msg() << "engcmd: no '" << key
                            << "' defined for endpoint '" << cfg.name << "'";
  return it->second;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Check if a configuration supports this protocol.
 *
 *  The endpoint 'protocol' tag must have the 'bbdo' value.
 *
 *  @param[in] cfg       Object configuration.
 *
 *  @return True if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  return (cfg.type == "engcmd");
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;
  std::string command_module_path{find_param(cfg, "command_module_path")};
  std::unique_ptr<io::endpoint> end{
      new endpoint(cfg.name, command_module_path)};
  is_acceptor = false;
  return end.release();
}
