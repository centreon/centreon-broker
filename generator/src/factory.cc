/*
** Copyright 2017 Centreon
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

#include "com/centreon/broker/generator/factory.hh"
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/generator/endpoint.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::generator;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Check if a configuration match the generator streams.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches any of the generator streams.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  return ((cfg.type == "generator_receiver") ||
          (cfg.type == "generator_sender"));
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg          Endpoint configuration.
 *  @param[out] is_acceptor  Will be set to false.
 *  @param[in]  cache        Unused.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Generate opener.
  std::auto_ptr<io::endpoint> s;
  if (cfg.type == "generator_receiver")
    s.reset(new endpoint(endpoint::type_receiver));
  else if (cfg.type == "generator_sender")
    s.reset(new endpoint(endpoint::type_sender));
  is_acceptor = false;
  return (s.release());
}
