/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/extcmd/factory.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/extcmd/command_server.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

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
 *  @param[in] cfg  Object configuration.
 *
 *  @return True if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  return (cfg.type == "extcmd");
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
  is_acceptor = true;
  // Find the protocol to use.
  command_server::protocol prot = command_server::json;
  std::map<std::string, std::string>::const_iterator it{
      cfg.params.find("command_protocol")};
  if (it != cfg.params.end()) {
    std::string command_prot{it->second};
    if (command_prot == "plaintext")
      prot = command_server::plaintext;
  }

  return new command_server(prot, cfg.params["extcmd"], cache);
}
