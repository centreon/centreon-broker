/*
** Copyright 2011-2015,2019 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/macro_cache.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

/**
 *  Construct a macro cache
 *
 *  @param[in] cache  Persistent cache used by the macro cache.
 */
macro_cache::macro_cache(std::shared_ptr<persistent_cache> const& cache)
  : _cache(cache) {
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
    } catch (std::exception e) {
      logging::error(logging::medium)
        << "influxdb: macro cache couldn't save data to disk: '"
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
           << "influxdb: could not find host/service of index "
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
           << "influxdb: could not find index of metric " << metric_id);
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
           << "influxdb: could not find information on host "
           << host_id);
  return (found->host_name);
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
           << "influxdb: could not find information on service ("
           << host_id << ", " << service_id << ")");
  return (found->service_description);
}

/**
 *  Get the name of an instance.
 *
 *  @param[in] instance_id  The id of the instance.
 *
 *  @return   The name of the instance.
 */
QString const& macro_cache::get_instance(unsigned int instance_id) const {
  QHash<unsigned int, neb::instance>::const_iterator
    found(_instances.find(instance_id));
  if (found == _instances.end())
    throw (exceptions::msg()
           << "influxdb: could not find information on instance "
           << instance_id);
  return (found->name);
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
  else if (data->type() == neb::service::static_type())
    _process_service(*std::static_pointer_cast<neb::service const>(data));
  else if (data->type() == storage::index_mapping::static_type())
    _process_index_mapping(*std::static_pointer_cast<storage::index_mapping const>(data));
  else if (data->type() == storage::metric_mapping::static_type())
    _process_metric_mapping(*std::static_pointer_cast<storage::metric_mapping const>(data));
}

/**
 *  Process an instance event.
 *
 *  @param in  The event.
 */
void macro_cache::_process_instance(neb::instance const& in) {
  _instances[in.poller_id] = in;
}

/**
 *  Process a host event.
 *
 *  @param h  The event.
 */
void macro_cache::_process_host(neb::host const& h) {
  _hosts[h.host_id] = h;
}

/**
 *  Process a service event.
 *
 *  @param s  The event.
 */
void macro_cache::_process_service(neb::service const& s) {
  _services[qMakePair(s.host_id, s.service_id)] = s;
}

/**
 *  Process an index mapping event.
 *
 *  @param im  The event.
 */
void macro_cache::_process_index_mapping(storage::index_mapping const& im) {
  _index_mappings[im.index_id] = im;
  return ;
}

/**
 *  Process a metric mapping event.
 *
 *  @param mm  The event.
 */
void macro_cache::_process_metric_mapping(storage::metric_mapping const& mm) {
  _metric_mappings[mm.metric_id] = mm;
}

/**
 *  Save all data to disk.
 */
void macro_cache::_save_to_disk() {
  _cache->transaction();

  for (QHash<unsigned int, neb::instance>::const_iterator
         it(_instances.begin()),
         end(_instances.end());
       it != end;
       ++it)
    _cache->add(std::shared_ptr<io::data>(new neb::instance(*it)));

  for (QHash<unsigned int, neb::host>::const_iterator
         it(_hosts.begin()),
         end(_hosts.end());
       it != end;
       ++it)
    _cache->add(std::shared_ptr<io::data>(new neb::host(*it)));

  for (QHash<QPair<unsigned int, unsigned int>, neb::service>::const_iterator
         it(_services.begin()),
         end(_services.end());
       it != end;
       ++it)
    _cache->add(std::shared_ptr<io::data>(new neb::service(*it)));

  for (QHash<unsigned int, storage::index_mapping>::const_iterator
         it(_index_mappings.begin()),
         end(_index_mappings.end());
       it != end;
       ++it)
    _cache->add(std::shared_ptr<io::data>(new storage::index_mapping(*it)));

  for (QHash<unsigned int, storage::metric_mapping>::const_iterator
         it(_metric_mappings.begin()),
         end(_metric_mappings.end());
       it != end;
       ++it)
    _cache->add(std::shared_ptr<io::data>(new storage::metric_mapping(*it)));

  _cache->commit();
}
