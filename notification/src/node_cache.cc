/*
** Copyright 2014-2015,2017 Centreon
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

#include "com/centreon/broker/notification/node_cache.hh"
#include <QMutexLocker>
#include <cstdio>
#include <exception>
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**
 *  Constructor.
 *
 *  @param[in] cache  The persistent cache used by the node cache.
 */
node_cache::node_cache(std::shared_ptr<persistent_cache> cache)
    : _mutex(QMutex::NonRecursive), _cache(cache) {
  multiplexing::engine::instance().hook(*this);
}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
node_cache::node_cache(node_cache const& obj) : multiplexing::hooker(obj) {
  node_cache::operator=(obj);
}

/**
 *  Destructor.
 */
node_cache::~node_cache() {
  multiplexing::engine::instance().unhook(*this);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to his object.
 */
node_cache& node_cache::operator=(node_cache const& obj) {
  if (this != &obj) {
    _host_node_states = obj._host_node_states;
    _service_node_states = obj._service_node_states;
    _cache = obj._cache;
  }
  return (*this);
}

/**
 *  Called when the engine starts. Used to load cache file.
 */
void node_cache::starting() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return;

  logging::debug(logging::low)
      << "notification: loading the node cache " << _cache->get_cache_file();

  std::shared_ptr<io::data> data;
  try {
    while (true) {
      _cache->get(data);
      if (!data)
        break;
      write(data);
    }
  } catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
        << "notification: could not load the node cache "
        << _cache->get_cache_file() << ": " << e.what();
    return;
  }

  logging::debug(logging::low)
      << "notification: finished loading the node cache "
      << _cache->get_cache_file() << " succesfully";
}

/**
 *  Called when the engine stops. Used to unload cache file.
 */
void node_cache::stopping() {
  // No cache, nothing to do.
  if (_cache.get() == NULL)
    return;

  logging::debug(logging::low)
      << "notification: writing the node cache " << _cache->get_cache_file();

  // Lock the mutex;
  QMutexLocker lock(&_mutex);

  try {
    // Start a transaction.
    _cache->transaction();
    // Sache into the cache.
    _save_cache();
    logging::debug(logging::low)
        << "notification: finished writing the node cache "
        << _cache->get_cache_file() << " succesfully";
  } catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
        << "notification: could not write the node cache "
        << _cache->get_cache_file() << ": " << e.what();
    return;
  }

  logging::debug(logging::low) << "notification: commiting the node cache '"
                               << _cache->get_cache_file() << "'";

  try {
    _cache->commit();
  } catch (std::exception const& e) {
    logging::error(logging::high)
        << "notification: could not commit the node cache '"
        << _cache->get_cache_file() << "': " << e.what();
  }

  logging::debug(logging::low) << "notification: commited the node cache '"
                               << _cache->get_cache_file() << "' succesfully";
}

/**
 *  Read events from the node cache.
 *
 *  @param[out] d         An output data event.
 *  @param[in]  deadline  Unused.
 *
 *  @return Always return true.
 */
bool node_cache::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  return true;
}

/**
 *  Write event to the node cache.
 *
 *  @param[in] data  The data event.
 *
 *  @return          Number of event acknowledged.
 */
int node_cache::write(std::shared_ptr<io::data> const& data) {
  // Check that data exists.
  if (!validate(data, "node_cache"))
    return (1);

  uint32_t type = data->type();
  if (type == neb::host::static_type())
    update(*std::static_pointer_cast<neb::host>(data));
  else if (type == neb::host_status::static_type())
    update(*std::static_pointer_cast<neb::host_status>(data));
  else if (type == neb::service::static_type())
    update(*std::static_pointer_cast<neb::service>(data));
  else if (type == neb::service_status::static_type())
    update(*std::static_pointer_cast<neb::service_status>(data));
  else if (type == neb::custom_variable::static_type() ||
           type == neb::custom_variable_status::static_type())
    update(*std::static_pointer_cast<neb::custom_variable_status>(data));
  else if (type == neb::acknowledgement::static_type())
    update(*std::static_pointer_cast<neb::acknowledgement const>(data));
  else if (type == neb::downtime::static_type())
    update(*std::static_pointer_cast<neb::downtime const>(data));

  return 1;
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host const& hst) {
  if (hst.host_id == 0)
    return;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hst.host_id)].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host_status const& hst) {
  if (hst.host_id == 0)
    return;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hst.host_id)].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] s  The data to update.
 */
void node_cache::update(neb::service const& s) {
  if (s.service_id == 0)
    return;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(s.host_id, s.service_id)].update(s);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sst  The data to update.
 */
void node_cache::update(neb::service_status const& sst) {
  if (sst.service_id == 0)
    return;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(sst.host_id, sst.service_id)].update(
      sst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] cvs  The data to update.
 */
void node_cache::update(neb::custom_variable_status const& cvs) {
  if (cvs.host_id == 0)
    return;
  QMutexLocker lock(&_mutex);
  if (cvs.service_id == 0)
    _host_node_states[objects::node_id(cvs.host_id)].update(cvs);
  else
    _service_node_states[objects::node_id(cvs.host_id, cvs.service_id)].update(
        cvs);
}

/**
 *  Update the node cache.
 *
 *  @param[in] ack  The data to update.
 */
void node_cache::update(neb::acknowledgement const& ack) {
  if (!ack.deletion_time.is_null())
    _acknowledgements.remove(objects::node_id(ack.host_id, ack.service_id));
  else
    _acknowledgements[objects::node_id(ack.host_id, ack.service_id)] = ack;
}

/**
 *  Update the node cache.
 *
 *  @param[in] dwn  The data to update.
 */
void node_cache::update(neb::downtime const& dwn) {
  if (dwn.actual_end_time.is_null()) {
    _downtimes[dwn.internal_id] = dwn;
    _downtime_id_by_nodes.insert(objects::node_id(dwn.host_id, dwn.service_id),
                                 dwn.internal_id);
  } else {
    _downtimes.remove(dwn.internal_id);
    _downtime_id_by_nodes.remove(objects::node_id(dwn.host_id, dwn.service_id),
                                 dwn.internal_id);
  }
}

/**
 *  Get a host from the node cache.
 *
 *  @param[in] id  The id of the host.
 *
 *  @return        The host from the node cache.
 */
node_cache::host_node_state const& node_cache::get_host(
    objects::node_id id) const {
  objects::node_id host_node_id(id.get_host_id());
  QHash<objects::node_id, host_node_state>::const_iterator found =
      _host_node_states.find(host_node_id);
  if (found == _host_node_states.end())
    throw(exceptions::msg() << "notification: host " << id.get_host_id()
                            << " was not found in cache");
  return (*found);
}

/**
 *  Get a service from the service cache.
 *
 *  @param[in] id  The id of the service.
 *
 *  @return        The service from the node cache.
 */
node_cache::service_node_state const& node_cache::get_service(
    objects::node_id id) const {
  QHash<objects::node_id, service_node_state>::const_iterator found =
      _service_node_states.find(id);
  if (found == _service_node_states.end())
    throw(exceptions::msg()
          << "notification: service (" << id.get_host_id() << ", "
          << id.get_service_id() << " was not found in cache");
  return (*found);
}

/**
 *  Is this node in downtime ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node is in downtime.
 */
bool node_cache::node_in_downtime(objects::node_id node) const {
  return (_downtime_id_by_nodes.contains(node));
}

/**
 *  Return the number of active downtimes associated to a node.
 *
 *  @param[in] node  The node.
 *
 *  @return          Number of active downtimes associated to a node.
 */
uint32_t node_cache::node_downtimes(objects::node_id node) const {
  return (_downtime_id_by_nodes.count(node));
}

/**
 *  Is this node acknowledged ?
 *
 *  @param[in] node  The node.
 *
 *  @return          True if this node was acknowledged.
 */
bool node_cache::node_acknowledged(objects::node_id node) const {
  return (_acknowledgements.contains(node));
}

/**
 *  Prepare the serialization of all the data.
 */
void node_cache::_save_cache() {
  std::deque<std::shared_ptr<io::data> > serialized_data;

  for (QHash<objects::node_id, host_node_state>::const_iterator
           it = _host_node_states.begin(),
           end = _host_node_states.end();
       it != end; ++it)
    it->serialize(serialized_data);
  for (QHash<objects::node_id, service_node_state>::const_iterator
           it = _service_node_states.begin(),
           end = _service_node_states.end();
       it != end; ++it)
    it->serialize(serialized_data);
  for (QHash<objects::node_id, neb::acknowledgement>::const_iterator
           it = _acknowledgements.begin(),
           end = _acknowledgements.end();
       it != end; ++it)
    serialized_data.push_back(std::make_shared<neb::acknowledgement>(*it));
  for (QHash<uint32_t, neb::downtime>::const_iterator
           it = _downtimes.begin(),
           end = _downtimes.end();
       it != end; ++it)
    serialized_data.push_back(std::make_shared<neb::downtime>(*it));

  for (std::deque<std::shared_ptr<io::data> >::const_iterator
           it = serialized_data.begin(),
           end = serialized_data.end();
       it != end; ++it)
    _cache->add(*it);
}
