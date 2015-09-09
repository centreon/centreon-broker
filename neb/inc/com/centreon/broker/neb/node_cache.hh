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

#ifndef CCB_NEB_NODE_CACHE_HH
#  define CCB_NEB_NODE_CACHE_HH

#  include <QHash>
#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/host_status.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/neb/node_id.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

CCB_BEGIN()

namespace                        neb {
  /**
   *  @class node_cache node_cache.hh "com/centreon/broker/neb/node_cache.hh"
   *  @brief Cache node data.
   */
  class                          node_cache {
  public:
                                 node_cache();
                                 node_cache(node_cache const& other);
                                 ~node_cache();
    node_cache&                  operator=(node_cache const& other);

    void                         write(misc::shared_ptr<io::data> const& d);
    void                         serialize(
                                   misc::shared_ptr<persistent_cache> cache);
    node_id                      get_node_by_names(
                                  std::string const& host_name,
                                  std::string const& service_description);
    unsigned short               get_current_state(node_id id);
    neb::host_status*            get_host_status(node_id id);
    neb::service_status*         get_service_status(node_id id);

  private:
    QHash<node_id, neb::host>   _hosts;
    QHash<node_id, neb::service>
                                _services;
    QHash<node_id, neb::host_status>
                                _host_statuses;
    QHash<node_id, neb::service_status>
                                _service_statuses;
    QHash<QPair<QString, QString>, node_id>
                                _names_to_node;
    QHash<node_id, unsigned short>
                                _previous_statuses;

    void                        _process_host(neb::host const& hst);
    void                        _process_service(
                                  neb::service const& svc);
    void                        _process_host_status(
                                  neb::host_status const& hst);
    void                        _process_service_status(
                                  neb::service_status const& sst);
  };
}

CCB_END()

#endif // !CCB_NEB_NODE_CACHE_HH
