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

#include <memory>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/node_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
node_cache::node_cache() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
node_cache::node_cache(node_cache const& other)
  : _hosts(other._hosts),
    _services(other._services),
    _host_statuses(other._host_statuses),
    _service_statuses(other._service_statuses),
    _names_to_node(other._names_to_node) {}

/**
 *  Destructor.
 */
node_cache::~node_cache() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
node_cache& node_cache::operator=(
  node_cache const& other) {
  if (this != &other) {
    _hosts = other._hosts;
    _services = other._services;
    _host_statuses = other._host_statuses;
    _service_statuses = other._service_statuses;
    _names_to_node = other._names_to_node;
  }
  return (*this);
}

/**
 *  Write data to the node cache.
 *
 *  @param[in] d  The data.
 */
void node_cache::write(misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return ;

  if (d->type() == neb::host::static_type()) {
    _process_host(d.ref_as<neb::host const>());
  }
  else if (d->type() == neb::service::static_type()) {
    _process_service(d.ref_as<neb::service const>());
  }
  else if (d->type() == neb::host_status::static_type()) {
    _process_host_status(d.ref_as<neb::host_status const>());
  }
  else if (d->type() == neb::service_status::static_type()) {
    _process_service_status(d.ref_as<neb::service_status const>());
  }
}

/**
 *  Serialize the node cache.
 *
 *  @param[in] cache  The cache.
 */
void node_cache::serialize(std::shared_ptr<persistent_cache> cache) {
  if (cache.get() == NULL)
    return ;
  for (QHash<node_id, neb::host>::const_iterator
         it = _hosts.begin(),
         end = _hosts.end();
       it != end;
       ++it)
    cache->add(misc::make_shared(new neb::host(*it)));
  for (QHash<node_id, neb::service>::const_iterator
         it = _services.begin(),
         end = _services.end();
       it != end;
       ++it)
    cache->add(misc::make_shared(new neb::service(*it)));
  for (QHash<node_id, neb::host_status>::const_iterator
         it = _host_statuses.begin(),
         end = _host_statuses.end();
       it != end;
       ++it)
    cache->add(misc::make_shared(new neb::host_status(*it)));
  for (QHash<node_id, neb::service_status>::const_iterator
         it = _service_statuses.begin(),
         end = _service_statuses.end();
       it != end;
       ++it)
    cache->add(misc::make_shared(new neb::service_status(*it)));
}

/**
 *  Get a node by its names.
 *
 *  @param[in] host_name            The host name.
 *  @param[in] service_description  The service description, or empty.
 *
 *  @return  The node id.
 */
node_id node_cache::get_node_by_names(
          std::string const& host_name,
          std::string const& service_description) {
  QHash<QPair<QString, QString>, node_id>::const_iterator
    found(_names_to_node.find(qMakePair(
                                QString::fromStdString(host_name),
                                QString::fromStdString(service_description))));
  if (found != _names_to_node.end())
    return (*found);
  else
    return (node_id());
}

/**
 *  Get the current state of a node, or zero.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return        Current state, or zero.
 */
unsigned short  node_cache::get_current_state(node_id id) {
  if (id.is_host()) {
    QHash<node_id, neb::host_status>::const_iterator found
      = _host_statuses.find(id);
    return (found != _host_statuses.end() ? found->last_hard_state : 0);
  }
  else {
    QHash<node_id, neb::service_status>::const_iterator found
      = _service_statuses.find(id);
    return (found != _service_statuses.end() ? found->last_hard_state : 0);
  }
}

/**
 *  Get a host status, or null.
 *
 *  @param[in] id  The id.
 *
 *  @return  A host status, or null.
 */
neb::host_status* node_cache::get_host_status(node_id id) {
  QHash<node_id, neb::host_status>::iterator found
    = _host_statuses.find(id);
  return (found != _host_statuses.end() ? &*found : NULL);
}

/**
 *  Get a service status, or null.
 *
 *  @param[in] id  The id.
 *
 *  @return  A service status, or null.
 */
neb::service_status* node_cache::get_service_status(node_id id) {
  QHash<node_id, neb::service_status>::iterator found
    = _service_statuses.find(id);
  return (found != _service_statuses.end() ? &*found : NULL);
}

/**
 *  Process a host event.
 *
 *  @param[in] hst  The host event.
 */
void node_cache::_process_host(
                   neb::host const& hst) {
  logging::debug(logging::medium)
    << "node events: processing host declaration for ("
    << hst.host_id << ")";
  _hosts[node_id(hst.host_id)] = hst;
  _names_to_node[qMakePair(hst.host_name, QString())] = node_id(hst.host_id);
}

/**
 *  Process a service event.
 *
 *  @param[in] svc  The service event.
 */
void node_cache::_process_service(
                   neb::service const& svc) {
  logging::debug(logging::medium)
    << "node events: processing service declaration for ("
    << svc.host_id << ", " << svc.service_id << ")";
  _services[node_id(svc.host_id, svc.service_id)] = svc;
  _names_to_node[qMakePair(svc.host_name, svc.service_description)]
    = node_id(svc.host_id, svc.service_id);
}

/**
 *  Process a host status event.
 *
 *  @param[in] hst  Host status event.
 */
void node_cache::_process_host_status(
                   neb::host_status const& hst) {
  logging::debug(logging::medium)
    << "node events: processing host status for ("
    << hst.host_id << ")";
  node_id id(hst.host_id);
  _host_statuses[id] = hst;
  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] sst  Service status event.
 */
void node_cache::_process_service_status(
                   neb::service_status const& sst) {
  logging::debug(logging::medium)
    << "node events: processing service status for ("
    << sst.host_id << ", " << sst.service_id  << ")";
  node_id id(sst.host_id, sst.service_id);
  _service_statuses[id] = sst;
  return ;
}
