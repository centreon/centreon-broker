/*
** Copyright 2011-2015 Centreon
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

#ifndef CCB_NOTIFICATION_NODE_CACHE_HH
#define CCB_NOTIFICATION_NODE_CACHE_HH

#include <QMultiHash>
#include <QMutex>
#include <deque>
#include <string>
#include <vector>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/multiplexing/hooker.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/custom_variable_status.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/notification/object_cache.hh"
#include "com/centreon/broker/notification/objects/node.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class node_cache node_cache.hh
 * "com/centreon/broker/notification/node_cache.hh"
 *  @brief Contain and update a cache of node information.
 *
 *  Used by and for the macro processing. Also contains/generates the current
 *  acknowledgements and downtimes. Load from a file at startup,
 *  unload at shutdown.
 */
class node_cache : public multiplexing::hooker {
 public:
  typedef object_cache<neb::host, neb::host_status> host_node_state;
  typedef object_cache<neb::service, neb::service_status> service_node_state;

  node_cache(std::shared_ptr<persistent_cache> cache);
  ~node_cache();
  node_cache(node_cache const& f);
  node_cache& operator=(node_cache const& f);

  virtual void starting();
  virtual void stopping();
  virtual bool read(std::shared_ptr<io::data>& d, time_t deadline);
  virtual int write(std::shared_ptr<io::data> const& d);

  using io::stream::update;
  void update(neb::host const& hst);
  void update(neb::host_status const& hs);
  void update(neb::service const& svc);
  void update(neb::service_status const& ss);
  void update(neb::custom_variable_status const& cvs);
  void update(neb::acknowledgement const& ack);
  void update(neb::downtime const& dwn);

  host_node_state const& get_host(objects::node_id id) const;
  service_node_state const& get_service(objects::node_id id) const;

  bool node_in_downtime(objects::node_id node) const;
  uint32_t node_downtimes(objects::node_id node) const;
  bool node_acknowledged(objects::node_id node) const;

 private:
  void _save_cache();

  QHash<objects::node_id, host_node_state> _host_node_states;
  QHash<objects::node_id, service_node_state> _service_node_states;
  QHash<objects::node_id, neb::acknowledgement> _acknowledgements;
  QHash<uint32_t, neb::downtime> _downtimes;
  QMultiHash<objects::node_id, uint32_t> _downtime_id_by_nodes;
  QMutex _mutex;

  std::shared_ptr<persistent_cache> _cache;
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_NODE_CACHE_HH
