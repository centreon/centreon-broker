/*
** Copyright 2011-2012,2015 Centreon
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

#ifndef CCB_CONFIG_APPLIER_ENDPOINT_HH
#define CCB_CONFIG_APPLIER_ENDPOINT_HH

#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_set>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace io {
class endpoint;
}
namespace multiplexing {
class subscriber;
}
namespace processing {
class failover;
class endpoint;
}  // namespace processing

namespace config {
// Forward declaration.
class endpoint;

namespace applier {
/**
 *  @class endpoint endpoint.hh "com/centreon/broker/config/applier/endpoint.hh"
 *  @brief Apply the configuration of endpoints.
 *
 *  Apply the configuration of the configured endpoints.
 */
class endpoint {
  std::map<config::endpoint, processing::endpoint*> _endpoints;
  std::timed_mutex _endpointsm;
  std::atomic_bool _discarding;

  endpoint();
  ~endpoint();
  void _discard();
  processing::failover* _create_failover(
      config::endpoint& cfg,
      std::shared_ptr<multiplexing::subscriber> sbscrbr,
      std::shared_ptr<io::endpoint> endp,
      std::list<config::endpoint>& l);
  std::shared_ptr<io::endpoint> _create_endpoint(config::endpoint& cfg,
                                                 bool& is_acceptor);
  multiplexing::subscriber* _create_subscriber(config::endpoint& cfg);
  void _diff_endpoints(
      std::map<config::endpoint, processing::endpoint*> const& current,
      std::list<config::endpoint> const& new_endpoints,
      std::list<config::endpoint>& to_create,
      std::list<config::endpoint>& to_delete);
  std::unordered_set<uint32_t> _filters(
      std::set<std::string> const& str_filters);

 public:
  typedef std::map<config::endpoint, processing::endpoint*>::iterator iterator;

  endpoint& operator=(const endpoint&) = delete;
  endpoint(const endpoint&) = delete;
  void apply(std::list<config::endpoint> const& endpoints);
  iterator endpoints_begin();
  iterator endpoints_end();
  std::timed_mutex& endpoints_mutex();
  static endpoint& instance();
  static void load();
  static void unload();
  static bool loaded();
};
}  // namespace applier
}  // namespace config

CCB_END()

#endif  // !CCB_CONFIG_APPLIER_ENDPOINT_HH
