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

#include "com/centreon/broker/correlation/factory.hh"
#include <cstring>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/correlation/connector.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check if an endpoint configuration matches streams build by this
 *  factory.
 *
 *  @param[in,out] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches streams build by this factory.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_correlation(!strncasecmp(cfg.type.c_str(), "correlation", 12));
  if (is_correlation) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_correlation;
}

/**
 *  Create a new endpoint matching configuration.
 *
 *  @param[in,out] cfg          Endpoint configuration.
 *  @param[out]    is_acceptor  Set to false.
 *  @param[in,out] cache        Persistent cache for this endpoint.
 *
 *  @return New endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  // Find correlation file.
  std::string correlation_file;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("file")};
    if (it == cfg.params.end())
      throw exceptions::msg()
          << "correlation: no 'file' defined for endpoint '" << cfg.name << "'";
    correlation_file = it->second;
  }

  // Check if this is a passive endpoint.
  bool passive;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("passive")};
    if (it != cfg.params.end())
      passive = config::parser::parse_boolean(it->second);
    else
      passive = false;
  }

  // Connector.
  is_acceptor = false;
  return new correlation::connector(correlation_file, passive, cache);
}
