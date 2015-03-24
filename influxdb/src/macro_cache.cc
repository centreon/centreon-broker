/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb//macro_cache.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

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
        << "influxdb: macro cache couldn't save data to disk: '"
        << e.what() << "'";
    }
  }
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
           << "influxdb: could not find metric mapping for " << metric_id);
  return (*found);
}

/**
 *  Get the status mapping of a status.
 *
 *  @param[in] status_id  The id of this status.
 *
 *  @return               The status mapping.
 */
storage::status_mapping const& macro_cache::get_status_mapping(
                                 unsigned int status_id) const {
  QHash<unsigned int, storage::status_mapping>::const_iterator
    found(_status_mappings.find(status_id));
  if (found == _status_mappings.end())
    throw (exceptions::msg()
           << "influxdb: could not find status mapping for " << status_id);
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
           << "influxdb: could not find host infos for " << host_id);
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
           << "influxdb: could not find service infos for ("
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
           << "influxdb: could not find instance infos for " << instance_id);
  return (found->name);
}

/**
 *  Write an event into the cache.
 *
 *  @param[in] data  The event to write.
 */
void macro_cache::write(misc::shared_ptr<io::data> const& data) {
  if (data.isNull())
    return ;

  if (data->type() == neb::instance::static_type())
    _process_instance(data.ref_as<neb::instance const>());
  else if (data->type() == neb::host::static_type())
    _process_host(data.ref_as<neb::host const>());
  else if (data->type() == neb::service::static_type())
    _process_service(data.ref_as<neb::service const>());
  else if (data->type() == storage::metric_mapping::static_type())
    _process_metric_mapping(data.ref_as<storage::metric_mapping const>());
  else if (data->type() == storage::status_mapping::static_type())
    _process_status_mapping(data.ref_as<storage::status_mapping const>());
}

/**
 *  Process an instance event.
 *
 *  @param in  The event.
 */
void macro_cache::_process_instance(neb::instance const& in) {
  _instances[in.id] = in;
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
 *  Process a metric mapping event.
 *
 *  @param mm  The event.
 */
void macro_cache::_process_metric_mapping(storage::metric_mapping const& mm) {
  _metric_mappings[mm.metric_id] = mm;
}

/**
 *  Process a status mapping event.
 *
 *  @param sm  The event.
 */
void macro_cache::_process_status_mapping(storage::status_mapping const& sm) {
  _status_mappings[sm.index_id] = sm;
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
    _cache->add(misc::shared_ptr<io::data>(new neb::instance(*it)));

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

  for (QHash<unsigned int, storage::metric_mapping>::const_iterator
         it(_metric_mappings.begin()),
         end(_metric_mappings.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new storage::metric_mapping(*it)));

  for (QHash<unsigned int, storage::status_mapping>::const_iterator
         it(_status_mappings.begin()),
         end(_status_mappings.end());
       it != end;
       ++it)
    _cache->add(misc::shared_ptr<io::data>(new storage::status_mapping(*it)));

  _cache->commit();
}
