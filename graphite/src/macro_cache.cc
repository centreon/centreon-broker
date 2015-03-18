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
#include "com/centreon/broker/graphite/macro_cache.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

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
      _write(d);
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
        << "graphite: macro cache couldn't save data to disk: '"
        << e.what() << "'";
    }
  }
}

/**
 *  Write an event into the cache.
 *
 *  @param[in] data  The event to write.
 */
void macro_cache::_write(misc::shared_ptr<io::data> const& data) {
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
