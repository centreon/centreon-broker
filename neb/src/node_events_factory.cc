/*
** Copyright 2015-2016 Centreon
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

#include <libgen.h>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/node_events_factory.hh"
#include "com/centreon/broker/neb/node_events_connector.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
node_events_factory::node_events_factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
node_events_factory::node_events_factory(node_events_factory const& other)
    : io::factory(other) {}

/**
 *  Destructor.
 */
node_events_factory::~node_events_factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
node_events_factory& node_events_factory::operator=(
    node_events_factory const& other) {
  io::factory::operator=(other);
  return *this;
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this object.
 */
io::factory* node_events_factory::clone() const {
  return new node_events_factory(*this);
}

/**
 *  Check if a configuration match the node event layer layer.
 *
 *  @param[in] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches the node event layer.
 */
bool node_events_factory::has_endpoint(config::endpoint& cfg) const {
  bool is_node_events(cfg.type == "node_events");
  if (is_node_events) {
    cfg.params["cache"] = "yes";
    cfg.cache_enabled = true;
  }
  return is_node_events;
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Persistent cache.
 *
 *  @return Connector matching configuration.
 */
io::endpoint* node_events_factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  std::string name;
  std::map<std::string, std::string>::const_iterator found{cfg.params.find("cfg_file")};
  if (found != cfg.params.end())
    name = found->second;

  is_acceptor = false;
  return new node_events_connector(cfg.name, cache, name);
}
