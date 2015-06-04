/*
** Copyright 2011-2015 Merethis
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

#ifndef CCB_NOTIFICATION_NODE_CACHE_HH
#  define CCB_NOTIFICATION_NODE_CACHE_HH

#  include <string>
#  include <deque>
#  include <vector>
#  include <QMutex>
#  include <QMultiHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bbdo/stream.hh"
#  include "com/centreon/broker/compression/stream.hh"
#  include "com/centreon/broker/file/stream.hh"
#  include "com/centreon/broker/notification/object_cache.hh"
#  include "com/centreon/broker/notification/objects/node.hh"
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/service.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/neb/host.hh"
#  include "com/centreon/broker/neb/host_status.hh"
#  include "com/centreon/broker/neb/custom_variable_status.hh"
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class node_cache node_cache.hh "com/centreon/broker/notification/node_cache.hh"
   *  @brief Contain and update a cache of node information.
   *
   *  Used by and for the macro processing. Also contains/generates the current
   *  acknowledgements and downtimes. Load from a file at startup,
   *  unload at shutdown.
   */
  class           node_cache : public multiplexing::hooker {
  public:
    typedef object_cache<neb::host, neb::host_status>
                  host_node_state;
    typedef object_cache<neb::service, neb::service_status>
                  service_node_state;

                  node_cache(misc::shared_ptr<persistent_cache> cache);
                  ~node_cache();
                  node_cache(node_cache const& f);
    node_cache&   operator=(node_cache const& f);

    virtual void  starting();
    virtual void  stopping();
    virtual bool  read(
                    misc::shared_ptr<io::data>& d,
                    time_t deadline);
    virtual unsigned int
                  write(misc::shared_ptr<io::data> const& d);

    void          update(neb::host const& hst);
    void          update(neb::host_status const& hs);
    void          update(neb::service const& svc);
    void          update(neb::service_status const& ss);
    void          update(neb::custom_variable_status const& cvs);
    void          update(neb::acknowledgement const& ack);
    void          update(neb::downtime const& dwn);

    host_node_state const&
                  get_host(objects::node_id id) const;
    service_node_state const&
                  get_service(objects::node_id id) const;

    bool          node_in_downtime(objects::node_id node) const;
    unsigned int  node_downtimes(objects::node_id node) const;
    bool          node_acknowledged(objects::node_id node) const;

  private:
    void          _prepare_serialization();

    QHash<objects::node_id, host_node_state>
                  _host_node_states;
    QHash<objects::node_id, service_node_state>
                  _service_node_states;
    QHash<objects::node_id, neb::acknowledgement>
                  _acknowledgements;
    QHash<unsigned int, neb::downtime>
                  _downtimes;
    QMultiHash<objects::node_id, unsigned int>
                  _downtime_id_by_nodes;
    QMutex        _mutex;

    misc::shared_ptr<persistent_cache>
                  _cache;

    std::deque<misc::shared_ptr<io::data> >
                  _serialized_data;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_CACHE_HH
