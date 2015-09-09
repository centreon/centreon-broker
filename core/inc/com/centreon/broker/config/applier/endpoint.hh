/*
** Copyright 2011-2012 Centreon
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
#  define CCB_CONFIG_APPLIER_ENDPOINT_HH

#  include <list>
#  include <map>
#  include <QMutex>
#  include <QObject>
#  include <set>
#  include <string>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace              io {
  class                endpoint;
}
namespace              multiplexing {
  class                subscriber;
}
namespace              processing {
  class                failover;
  class                thread;
}

namespace              config {
  // Forward declaration.
  class                endpoint;

  namespace            applier {
    /**
     *  @class endpoint endpoint.hh "com/centreon/broker/config/applier/endpoint.hh"
     *  @brief Apply the configuration of endpoints.
     *
     *  Apply the configuration of the configured endpoints.
     */
    class              endpoint {
    public:
      typedef          std::map<config::endpoint, processing::thread*>::iterator
                       iterator;

                       ~endpoint();
      void             apply(
                         std::list<config::endpoint> const& endpoints,
                         std::string const& cache_directory);
      void             discard();
      iterator         endpoints_begin();
      iterator         endpoints_end();
      QMutex&          endpoints_mutex();
      static endpoint& instance();
      static void      load();
      static void      unload();

    private:
                       endpoint();
                       endpoint(endpoint const& other);
      endpoint&        operator=(endpoint const& other);
      processing::failover*
                       _create_failover(
                         config::endpoint& cfg,
                         misc::shared_ptr<multiplexing::subscriber> sbscrbr,
                         misc::shared_ptr<io::endpoint> endp,
                         std::list<config::endpoint>& l);
      misc::shared_ptr<io::endpoint>
                       _create_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor);
      multiplexing::subscriber*
                       _create_subscriber(config::endpoint& cfg);
      void             _diff_endpoints(
                         std::map<config::endpoint, processing::thread*> const& current,
                         std::list<config::endpoint> const& new_endpoints,
                         std::list<config::endpoint>& to_create);
      uset<unsigned int>
                       _filters(std::set<std::string> const& str_filters);

      std::map<config::endpoint, processing::thread*>
                       _endpoints;
      QMutex           _endpointsm;
    };
  }
}

CCB_END()

#endif // !CCB_CONFIG_APPLIER_ENDPOINT_HH
