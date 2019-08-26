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

#include <unordered_set>
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
macro_cache::macro_cache(std::shared_ptr<persistent_cache> const& cache)
  : _cache(cache), _services{}
{
  if (_cache.get() != NULL) {
    std::shared_ptr<io::data> d;
    do {
      _cache->get(d);
      write(d);
    } while (d);
  }
}

/**
 *  Destructor.
 */
macro_cache::~macro_cache() {
  if (_cache.get() != NULL) {
    try {
      _save_to_disk();
    } catch (std::exception const& e) {
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
  std::unordered_map<uint64_t, storage::index_mapping>::const_iterator
    found{_index_mappings.find(index_id)};
  if (found == _index_mappings.end())
    throw exceptions::msg()
           << "lua: could not find host/service of index "
           << index_id;
  return found->second;
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
  std::unordered_map<uint64_t, storage::metric_mapping>::const_iterator
    found{_metric_mappings.find(metric_id)};
  if (found == _metric_mappings.end())
    throw exceptions::msg()
           << "lua: could not find index of metric " << metric_id;
  return found->second;
}

/**
 *  Get the name of a host.
 *
 *  @param[in] host_id  The id of the host.
 *
 *  @return             The name of the host.
 */
std::string const& macro_cache::get_host_name(uint64_t host_id) const {
  std::unordered_map<uint64_t, neb::host>::const_iterator found{
      _hosts.find(host_id)};

  if (found == _hosts.end())
    throw exceptions::msg()
        << "lua: could not find information on host " << host_id;
  return found->second.host_name;
}

/**
 *  Get a map of the host groups members index by host_id and host_group.
 *
 *  @return             A std::map
 */
std::map<std::pair<uint64_t, uint64_t>, neb::host_group_member> const&
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
std::string const& macro_cache::get_host_group_name(uint64_t id) const {
  std::unordered_map<uint64_t, neb::host_group>::const_iterator
    found{_host_groups.find(id)};

  if (found == _host_groups.end())
    throw exceptions::msg()
           << "lua: could not find information on host group " << id;
  return found->second.name;
}

/**
 *  Get the description of a service.
 *
 *  @param[in] host_id  The id of the host.
 *  @param service_id
 *
 *  @return             The description of the service.
 */
std::string const& macro_cache::get_service_description(
                 uint64_t host_id,
                 uint64_t service_id) const {
  std::unordered_map<std::pair<uint64_t, uint64_t>, neb::service>::const_iterator
    found{_services.find({host_id, service_id})};
  if (found == _services.end())
    throw exceptions::msg()
           << "lua: could not find information on service ("
           << host_id << ", " << service_id << ")";
  return found->second.service_description;
}

/**
 *  Service group members accessor
 *
 *  @param[in] host_id  The id of the host.
 *  @param[in] service_id  The id of the service.
 *
 *  @return   A map indexed by host_id/service_id/group_id.
 */
std::map<std::tuple<uint64_t, uint64_t, uint64_t>,
         neb::service_group_member> const&
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
std::string const& macro_cache::get_service_group_name(uint64_t id) const {
  std::unordered_map<uint64_t, neb::service_group>::const_iterator
    found{_service_groups.find(id)};

  if (found == _service_groups.end())
    throw exceptions::msg()
           << "lua: could not find information on service group " << id;
  return found->second.name;
}

/**
 *  Get the name of an instance.
 *
 *  @param[in] instance_id  The id of the instance.
 *
 *  @return   The name of the instance.
 */
std::string const& macro_cache::get_instance(uint64_t instance_id) const {
  std::unordered_map<uint64_t, neb::instance>::const_iterator
    found{_instances.find(instance_id)};
  if (found == _instances.end())
    throw exceptions::msg()
           << "lua: could not find information on instance "
           << instance_id;
  return found->second.name;
}

/**
 *  Accessor to the multi hash containing ba bv relations.
 *
 * @return A reference to an unordered_multimap containining ba/bv relation
 * events.
 */
std::unordered_multimap<uint64_t, bam::dimension_ba_bv_relation_event> const&
       macro_cache::get_dimension_ba_bv_relation_events() const {
  return _dimension_ba_bv_relation_events;
}

/**
 *  Return a dimension_ba_event from its id.
 *
 * @param ba_id The id
 *
 * @return a reference to the dimension_ba_event.
 */
bam::dimension_ba_event const& macro_cache::get_dimension_ba_event(
       uint64_t ba_id) const {
  std::unordered_map<uint64_t, bam::dimension_ba_event>::const_iterator
    found{_dimension_ba_events.find(ba_id)};
  if (found == _dimension_ba_events.end())
    throw exceptions::msg()
           << "lua: could not find information on dimension ba event "
           << ba_id;
  return found->second;
}

/**
 *  Return a dimension_bv_event from its id.
 *
 * @param bv_id The id
 *
 * @return a reference to the dimension_bv_event.
 */
bam::dimension_bv_event const& macro_cache::get_dimension_bv_event(
       uint64_t bv_id) const {
  std::unordered_map<uint64_t, bam::dimension_bv_event>::const_iterator
    found{_dimension_bv_events.find(bv_id)};
  if (found == _dimension_bv_events.end())
    throw exceptions::msg()
           << "lua: could not find information on dimension bv event "
           << bv_id;
  return found->second;
}

/**
 *  Write an event into the cache.
 *
 *  @param[in] data  The event to write.
 */
void macro_cache::write(std::shared_ptr<io::data> const& data) {
  if (!data)
    return ;

  if (data->type() == neb::instance::static_type())
    _process_instance(*std::static_pointer_cast<neb::instance const>(data));
  else if (data->type() == neb::host::static_type())
    _process_host(*std::static_pointer_cast<neb::host const>(data));
  else if (data->type() == neb::host_group::static_type())
    _process_host_group(*std::static_pointer_cast<neb::host_group const>(data));
  else if (data->type() == neb::host_group_member::static_type())
    _process_host_group_member(*std::static_pointer_cast<neb::host_group_member const>(data));
  else if (data->type() == neb::service::static_type())
    _process_service(*std::static_pointer_cast<neb::service const>(data));
  else if (data->type() == neb::service_group::static_type())
    _process_service_group(*std::static_pointer_cast<neb::service_group const>(data));
  else if (data->type() == neb::service_group_member::static_type())
    _process_service_group_member(
      *std::static_pointer_cast<neb::service_group_member const>(data));
  else if (data->type() == storage::index_mapping::static_type())
    _process_index_mapping(*std::static_pointer_cast<storage::index_mapping const>(data));
  else if (data->type() == storage::metric_mapping::static_type())
    _process_metric_mapping(*std::static_pointer_cast<storage::metric_mapping const>(data));
  else if (data->type() == bam::dimension_ba_event::static_type())
    _process_dimension_ba_event(*std::static_pointer_cast<bam::dimension_ba_event const>(data));
  else if (data->type() == bam::dimension_ba_bv_relation_event::static_type())
    _process_dimension_ba_bv_relation_event(
      *std::static_pointer_cast<bam::dimension_ba_bv_relation_event const>(data));
  else if (data->type() == bam::dimension_bv_event::static_type())
    _process_dimension_bv_event(*std::static_pointer_cast<bam::dimension_bv_event const>(data));
  else if (data->type() == bam::dimension_truncate_table_signal::static_type())
    _process_dimension_truncate_table_signal(
      *std::static_pointer_cast<bam::dimension_truncate_table_signal const>(data));
}

/**
 *  Process an instance event.
 *
 *  @param in  The event.
 */
void macro_cache::_process_instance(neb::instance const& in) {
  unsigned int poller_id(in.poller_id);

  std::unordered_set<uint64_t> hosts_removed;
  for (std::unordered_map<uint64_t, neb::host>::iterator
         it{_hosts.begin()},
         end{_hosts.end()};
       it != end; ) {
    if (it->second.poller_id == poller_id) {
      hosts_removed.insert(it->second.host_id);
      it = _hosts.erase(it);
    }
    else
      ++it;
  }

  for (uint64_t id : hosts_removed) {
    auto it(_host_group_members.lower_bound({id, 0}));
    while (it != _host_group_members.end() && it->first.first == id) {
      it = _host_group_members.erase(it);
    }
  }

  std::unordered_set<std::pair<uint64_t, uint64_t>> services_removed;
  for (std::unordered_map<std::pair<uint64_t, uint64_t>, neb::service>::iterator
         it(_services.begin()),
         end(_services.end());
       it != end; ) {
    if (hosts_removed.count(it->second.host_id)) {
      services_removed.insert(it->first);
      it = _services.erase(it);
    }
    else
      ++it;
  }

  _instances[poller_id] = in;
}

/**
 *  Process a host event.
 *
 *  @param h  The event.
 */
void macro_cache::_process_host(neb::host const& h) {
  logging::debug(logging::medium)
    << "lua: processing host '" << h.host_name << "' of id " << h.host_id;
  _hosts[h.host_id] = h;
}

/**
 *  Process a host group event.
 *
 *  @param hg  The event.
 */
void macro_cache::_process_host_group(neb::host_group const& hg) {
  logging::debug(logging::medium)
    << "lua: processing host group '" << hg.name << "' of id " << hg.id;
  if (hg.enabled)
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
    << "lua: processing host group member "
    << " (group_name: '" << hgm.group_name << "', group_id: " << hgm.group_id
    << ", host_id: " << hgm.host_id << ")";
  if (hgm.enabled)
    _host_group_members[{hgm.host_id, hgm.group_id}] = hgm;
  else
    _host_group_members.erase({hgm.host_id, hgm.group_id});
}

/**
 *  Process a service event.
 *
 *  @param s  The event.
 */
void macro_cache::_process_service(neb::service const& s) {
  logging::debug(logging::medium)
    << "lua: processing service (" << s.host_id << ", " << s.service_id << ") "
    << "(description: " << s.service_description << ")";
  _services[{s.host_id, s.service_id}] = s;
}

/**
 *  Process a service group event.
 *
 *  @param sg  The event.
 */
void macro_cache::_process_service_group(neb::service_group const& sg) {
  logging::debug(logging::medium)
    << "lua: processing service group '" << sg.name << "' of id " << sg.id;
  if (sg.enabled)
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
    << "lua: processing service group member "
    << " (group_name: '" << sgm.group_name << "', group_id: " << sgm.group_id
    << ", host_id: " << sgm.host_id
    << ", service_id: " << sgm.service_id << ")";
  if (sgm.enabled)
    _service_group_members[std::make_tuple(sgm.host_id, sgm.service_id, sgm.group_id)] = sgm;
  else
    _service_group_members.erase(std::make_tuple(sgm.host_id, sgm.service_id, sgm.group_id));
}

/**
 *  Process an index mapping event.
 *
 *  @param im  The event.
 */
void macro_cache::_process_index_mapping(storage::index_mapping const& im) {
  logging::debug(logging::medium)
    << "lua: processing index mapping (index_id: " << im.index_id
    << ", host_id: " << im.host_id
    << ", service_id: " << im.service_id << ")";
  _index_mappings[im.index_id] = im;
}

/**
 *  Process a metric mapping event.
 *
 *  @param mm  The event.
 */
void macro_cache::_process_metric_mapping(storage::metric_mapping const& mm) {
  logging::debug(logging::medium)
    << "lua: processing metric mapping (metric_id: " << mm.metric_id
    << ", index_id: " << mm.index_id << ")";
  _metric_mappings[mm.metric_id] = mm;
}

/**
 *  Process a dimension ba event
 *
 *  @param dbae  The event.
 */
void macro_cache::_process_dimension_ba_event(
                    bam::dimension_ba_event const& dbae) {
  logging::debug(logging::medium)
    << "lua: processing dimension ba event of id " << dbae.ba_id;
  _dimension_ba_events[dbae.ba_id] = dbae;
}

/**
 *  Process a dimension ba bv relation event
 *
 *  @param rel  The event.
 */
void macro_cache::_process_dimension_ba_bv_relation_event(
                    bam::dimension_ba_bv_relation_event const& rel) {
  logging::debug(logging::medium)
    << "lua: processing dimension ba bv relation event "
    << "(ba_id: " << rel.ba_id << ", " << "bv_id: " << rel.bv_id << ")";
  _dimension_ba_bv_relation_events.insert({rel.ba_id, rel});
}

/**
 *  Process a dimension bv event
 *
 *  @param rel  The event.
 */
void macro_cache::_process_dimension_bv_event(
                    bam::dimension_bv_event const& dbve) {
  logging::debug(logging::medium)
    << "lua: processing dimension bv event of id " << dbve.bv_id;
  _dimension_bv_events[dbve.bv_id] = dbve;
}

/**
 *  Process a dimension truncate table signal
 *
 * @param trunc  The event.
 */
void macro_cache::_process_dimension_truncate_table_signal(
                    bam::dimension_truncate_table_signal const& trunc) {
  logging::debug(logging::medium)
    << "lua: processing dimension truncate table signal";

  if (trunc.update_started) {
    _dimension_ba_events.clear();
    _dimension_ba_bv_relation_events.clear();
    _dimension_bv_events.clear();
  }
}

/**
 *  Save all data to disk.
 */
void macro_cache::_save_to_disk() {
  _cache->transaction();

  for (std::unordered_map<uint64_t, neb::instance>::const_iterator
         it(_instances.begin()),
         end(_instances.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::instance>(it->second));

  for (std::unordered_map<uint64_t, neb::host>::const_iterator
         it(_hosts.begin()),
         end(_hosts.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::host>(it->second));

  for (std::unordered_map<uint64_t, neb::host_group>::const_iterator
         it(_host_groups.begin()),
         end(_host_groups.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::host_group>(it->second));

  for (std::map<std::pair<uint64_t, uint64_t>, neb::host_group_member>::const_iterator
         it(_host_group_members.begin()),
         end(_host_group_members.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::host_group_member>(it->second));

  for (std::unordered_map<std::pair<uint64_t, uint64_t>, neb::service>::const_iterator
         it(_services.begin()),
         end(_services.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::service>(it->second));

  for (std::unordered_map<uint64_t, neb::service_group>::const_iterator
         it(_service_groups.begin()),
         end(_service_groups.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::service_group>(it->second));

  for (std::map<std::tuple<uint64_t, uint64_t, uint64_t>,
             neb::service_group_member>::const_iterator
         it{_service_group_members.begin()},
         end{_service_group_members.end()};
       it != end;
       ++it)
    _cache->add(std::make_shared<neb::service_group_member>(it->second));

  for (std::unordered_map<uint64_t, storage::index_mapping>::const_iterator
         it(_index_mappings.begin()),
         end(_index_mappings.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<storage::index_mapping>(it->second));

  for (std::unordered_map<uint64_t, storage::metric_mapping>::const_iterator
         it(_metric_mappings.begin()),
         end(_metric_mappings.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<storage::metric_mapping>(it->second));

  for (std::unordered_map<uint64_t, bam::dimension_ba_event>::const_iterator
         it(_dimension_ba_events.begin()),
         end(_dimension_ba_events.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<bam::dimension_ba_event>(it->second));

  for (std::unordered_multimap<uint64_t, bam::dimension_ba_bv_relation_event>::const_iterator
         it(_dimension_ba_bv_relation_events.begin()),
         end(_dimension_ba_bv_relation_events.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<bam::dimension_ba_bv_relation_event>(it->second));

  for (std::unordered_map<uint64_t, bam::dimension_bv_event>::const_iterator
         it(_dimension_bv_events.begin()),
         end(_dimension_bv_events.end());
       it != end;
       ++it)
    _cache->add(std::make_shared<bam::dimension_bv_event>(it->second));

  _cache->commit();
}
