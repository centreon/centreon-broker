/*
** Copyright 2011-2012,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
