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

#ifndef CCB_NEB_NODE_EVENTS_CONNECTOR_HH
#define CCB_NEB_NODE_EVENTS_CONNECTOR_HH

#include <memory>
#include <string>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class node_events_connector node_events_connector.hh
 * "com/centreon/broker/neb/node_events_connector.hh"
 *  @brief Open a correlation stream.
 *
 *  Generate a correlation stream that will generation correlation
 *  events (issue, issue parenting, host/service state events, ...).
 */
class node_events_connector : public io::endpoint {
 public:
  node_events_connector(std::string const& name,
                        std::shared_ptr<persistent_cache> cache,
                        std::string const& config_file);
  node_events_connector(node_events_connector const& other);
  ~node_events_connector();
  node_events_connector& operator=(node_events_connector const& other);
  std::shared_ptr<io::stream> open();

 private:
  std::shared_ptr<persistent_cache> _cache;
  std::string _config_file;
  std::string _name;
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_NODE_EVENTS_CONNECTOR_HH
