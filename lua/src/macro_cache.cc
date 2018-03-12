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

#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/lua/macro_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::logging;
using namespace com::centreon::broker::lua;

/**
 *  Construct a macro cache
 *
 *  @param[in] cache  Persistent cache used by the macro cache.
 */
macro_cache::macro_cache(misc::shared_ptr<persistent_cache> const& cache)
  : _cache(cache) {
  if (!_cache.isNull()) {
    misc::shared_ptr<io::data> d;
    do {
      _cache->get(d);
      write(d);
    } while (!d.isNull());
  }
}

/**
 *  Destructor.
 */
macro_cache::~macro_cache() {
  if (!_cache.isNull()) {
    try {
      _save_to_disk();
    } catch (std::exception e) {
      logging::error(logging::medium)
        << "lua: macro cache couldn't save data to disk: '"
        << e.what() << "'";
    }
  }
}

/**
 *  Get the mapping of an index.
 *
 *  @param[in] index_id   ID of the index.
 *
 *  @return               The status mapping.
 */
storage::index_mapping const& macro_cache::get_index_mapping(
                                 unsigned int index_id) const {
  QHash<unsigned int, storage::index_mapping>::const_iterator
    found(_index_mappings.find(index_id));
  if (found == _index_mappings.end())
    throw (exceptions::msg()
           << "lua: could not find host/service of index "
           << index_id);
  return (*found);
}

/**
 *  Get the metric mapping of a metric.
 *
 *  @param[in] metric_id  The id of this metric.
 *
 *  @return               The metric mapping.
 */
storage::metric_mapping const& macro_cache::get_metric_mapping(
                                 unsigned int metric_id) const {
  QHash<unsigned int, storage::metric_mapping>::const_iterator
    found(_metric_mappings.find(metric_id));
  if (found == _metric_mappings.end())
    throw (exceptions::msg()
           << "lua: could not find index of metric " << metric_id);
  return (*found);
}

/**
 *  Get the name of a host.
 *
 *  @param[in] host_id  The id of the host.
 *
 *  @return             The name of the host.
 */
QString const& macro_cache::get_host_name(unsigned int host_id) const {
  QHash<unsigned int, neb::host>::const_iterator
    found(_hosts.find(host_id));

  if (found == _hosts.end())
    throw (exceptions::msg()
           << "lua: could not find information on host "
           << host_id);
  return (found->host_name);
}


/**
 *  Get a list of the host groups containing a host.
 *
 *  @param[in] host_id  The id of the host.
 *
 *  @return             A QStringList.
 */
QMultiHash<unsigned int, neb::host_group_member> const&
                                   macro_cache::get_host_group_members() const {
  return _host_group_members;
}

/**
 *  Get the name of a host group.
 *
 *  @param[in] id  The id of the host group.
 *
 *  @return             The name of the host group.
 */
QString const& macro_cache::get_host_group_name(unsigned int id) const {
  QHash<unsigned int, neb::host_group>::const_iterator
    found(_host_groups.find(id));

  if (found == _host_groups.end())
    throw (exceptions::msg()
           << "lua: could not find information on host group " << id);
  return (found->name);
}

/**
 *  Get the description of a service.
 *
 *  @param[in] host_id  The id of the host.
 *  @param service_id
 *
 *  @return             The description of the service.
 */
QString const& macro_cache::get_service_description(
                 unsigned int host_id,
                 unsigned int service_id) const {
  QHash<QPair<unsigned int, unsigned int>, neb::service>::const_iterator
    found(_services.find(qMakePair(host_id, service_id)));
  if (found == _services.end())
    throw (exceptions::msg()
           << "lua: could not find information on service ("
           << host_id << ", " << service_id << ")");
  return (found->service_description);
}

/**
 *  Service group members accessor
 *
 *  @param[in] host_id  The id of the host.
 *  @param[in] service_id  The id of the service.
 *
 *  @return             A QStringList.
 */
QMultiHash<QPair<unsigned int, unsigned int>, neb::service_group_member> const&
                               macro_cache::get_service_group_members() const {
  return _service_group_members;
}

/**
 *  Get the name of a service group.
 *
 *  @param[in] id  The id of the service group.
 *
 *  @return            The name of the service group.
 */
QString const& macro_cache::get_service_group_name(unsigned int id) const {
  QHash<unsigned int, neb::service_group>::const_iterator
    found(_service_groups.find(id));

  if (found == _service_groups.end())
    throw (exceptions::msg()
           << "lua: could not find information on service group " << id);
  return (found->name);
}

/**
 *  Get the name of an instance.
 *
 *  @param[in] instance_id  The id of the instance.
 *
 *  @return   The name of the instance.
 */
QString const& macro_cache::get_instance(unsigned int instance_id) const {
  QHash<unsigned int, instance_broadcast>::const_iterator
    found(_instances.find(instance_id));
  if (found == _instances.end())
    throw (exceptions::msg()
           << "lua: could not find information on instance "
           << instance_id);
  return (found->poller_name);
}

/**
 *  Write an event into the cache.
 *
 *  @param[in] data  The event to write.
 */
void macro_cache::write(misc::shared_ptr<io::data> const& data) {
  if (data.isNull())
    return ;

  if (data->type() == instance_broadcast::static_type())
    _process_instance(data.ref_as<instance_broadcast const>());
  else if (data->type() == neb::host::static_type())
    _process_host(data.ref_as<neb::host const>());
  else if (data->type() == neb::host_group::static_type())
    _process_host_group(data.ref_as<neb::host_group const>());
  else if (data->type() == neb::host_group_member::static_type())
    _process_host_group_member(data.ref_as<neb::host_group_member const>());
  else if (data->type() == neb::service::static_type())
    _process_service(data.ref_as<neb::service const>());
  else if (data->type() == neb::service_group::static_type())
    _process_service_group(data.ref_as<neb::service_group const>());
  else if (data->type() == neb::service_group_member::static_type())
    _process_service_group_member(data.ref_as<neb::service_group_member const>());
  else if (data->type() == storage::index_mapping::static_type())
    _process_index_mapping(data.ref_as<storage::index_mapping const>());
  else if (data->type() == storage::metric_mapping::static_type())
    _process_metric_mapping(data.ref_as<storage::metric_mapping const>());
}

/**
 *  Process an instance event.
 *
 *  @param in  The event.
 */
void macro_cache::_process_instance(instance_broadcast const& in) {
  _instances[in.poller_id] = in;
}

/**
 *  Process a host event.
 *
 *  @param h  The event.
 */
void macro_cache::_process_host(neb::host const& h) {
  logging::debug(logging::medium)
    << "macro_cache: process host --- "
    << "id = " << h.host_id
    << " ; name = " << h.host_name;
  _hosts[h.host_id] = h;
}

/**
 *  Process a host group event.
 *
 *  @param hg  The event.
 */
void macro_cache::_process_host_group(neb::host_group const& hg) {
  logging::debug(logging::medium)
    << "macro_cache: process host group --- "
    << "host group id = " << hg.id
    << " ; name = " << hg.name;
  _host_groups[hg.id] = hg;
}

/**
 *  Process a host group member event.
 *
 *  @param hgm  The event.
 */
void macro_cache::_process_host_group_member(
       neb::host_group_member const& hgm) {
  logging::debug(logging::medium)
    << "macro_cache: process host group member --- "
    << "host id = " << hgm.host_id
    << "host group id = " << hgm.group_id
    << "host group name = " << hgm.group_name;
  _host_group_members.insert(hgm.host_id, hgm);
}

/**
 *  Process a service event.
 *
 *  @param s  The event.
 */
void macro_cache::_process_service(neb::service const& s) {
  logging::debug(logging::medium)
    << "macro_cache: process service --- "
    << "host id = " << s.host_id
    << " ; service id = " << s.service_id
    << " ; description = " << s.service_description;
  _services[qMakePair(s.host_id, s.service_id)] = s;
}

/**
 *  Process a service group event.
 *
 *  @param sg  The event.
 */
void macro_cache::_process_service_group(neb::service_group const& sg) {
  logging::debug(logging::medium)
    << "macro_cache: process service group --- "
    << "service group id = " << sg.id
    << " ; name = " << sg.name;
  _service_groups[sg.id] = sg;
}

/**
 *  Process a service group member event.
 *
 *  @param sgm  The event.
 */
void macro_cache::_process_service_group_member(
       neb::service_group_member const& sgm) {
  logging::debug(logging::medium)
    << "macro_cache: process service group member --- "
    << "host id = " << sgm.host_id
    << "service id = " << sgm.service_id
    << "service group id = " << sgm.group_id
    << "service group name = " << sgm.group_name;
  _service_group_members.insert(qMakePair(sgm.host_id, sgm.service_id), sgm);
}

/**
 *  Process an index mapping event.
 *
 *  @param im  The event.
 */
void macro_cache::_process_index_mapping(storage::index_mapping const& im) {
  logging::debug(logging::medium)
    << "macro_cache: process index mapping --- "
    << "index id = " << im.index_id;
  _index_mappings[im.index_id] = im;
  return ;
}

/**
 *  Process a metric mapping event.
 *
 *  @param mm  The event.
 */
void macro_cache::_process_metric_mapping(storage::metric_mapping const& mm) {
  logging::debug(logging::medium)
    << "macro_cache: process metric mapping --- "
    << "index id = " << mm.index_id;
  _metric_mappings[mm.metric_id] = mm;
}

/**
 *  Save all data to disk.
 */
void macro_cache::_save_to_disk() {
  _cache->transaction();

  for (QHash<unsigned int, instance_broadcast>::const_iterator
         it(_instances.begin()),
         end(_instances.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new instance_broadcast(*it)));

  for (QHash<unsigned int, neb::host>::const_iterator
         it(_hosts.begin()),
         end(_hosts.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new neb::host(*it)));

  for (QHash<QPair<unsigned int, unsigned int>, neb::service>::const_iterator
         it(_services.begin()),
         end(_services.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new neb::service(*it)));

  for (QHash<unsigned int, storage::index_mapping>::const_iterator
         it(_index_mappings.begin()),
         end(_index_mappings.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new storage::index_mapping(*it)));

  for (QHash<unsigned int, storage::metric_mapping>::const_iterator
         it(_metric_mappings.begin()),
         end(_metric_mappings.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new storage::metric_mapping(*it)));

  _cache->commit();
}
