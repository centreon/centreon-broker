/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include <exception>
#include <QMutexLocker>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**
 *  Constructor.
 *
 *  @param[in] cache  The persistent cache used by the node cache.
 */
node_cache::node_cache(misc::shared_ptr<persistent_cache> cache)
  : _mutex(QMutex::NonRecursive),
    _cache(cache) {
  multiplexing::engine::instance().hook(*this);
}

/**
 *  Destructor.
 */
node_cache::~node_cache() {
  multiplexing::engine::instance().unhook(*this);
}


/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
node_cache::node_cache(node_cache const& obj) {
  node_cache::operator=(obj);
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
  if (_cache.isNull())
    return ;

  logging::debug(logging::low)
    << "notification: loading the node cache " << _cache->get_cache_file();

  misc::shared_ptr<io::data> data;
  try {
    while (true) {
      _cache->get(data);
      if (data.isNull())
        break ;
      write(data);
    }
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not load the node cache "
      << _cache->get_cache_file() << ": " << e.what();
    return ;
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
  if (_cache.isNull())
    return ;

  logging::debug(logging::low)
    << "notification: writing the node cache " << _cache->get_cache_file();

  // Start a transaction.
  _cache->transaction();

  // Lock the mutex;
  QMutexLocker lock(&_mutex);

  // Prepare serialization.
  _prepare_serialization();

  misc::shared_ptr<io::data> data;

  try {
    while (true) {
      read(data);
      _cache->add(data);
    }
  }
  catch (io::exceptions::shutdown const& s) {
    // Normal termination of the stream (ie nothing to write anymore).
    (void)s;
    logging::debug(logging::low)
      << "notification: finished writing the node cache "
      << _cache->get_cache_file() << " succesfully";
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not write the node cache "
      << _cache->get_cache_file() << ": " << e.what();
    return ;
  }

  logging::debug(logging::low)
    << "notification: commiting the node cache '"
    << _cache->get_cache_file() << "'";

  try {
    _cache->commit();
  } catch (std::exception const& e) {
    logging::error(logging::high)
      << "notification: could not commit the node cache '"
      << _cache->get_cache_file() << "': " << e.what();
  }

  logging::debug(logging::low)
    << "notification: commited the node cache '"
    << _cache->get_cache_file() << "' succesfully";
}

/**
 *  Read events from the node cache.
 *
 *  @param[out] d  An output data event.
 */
void node_cache::read(misc::shared_ptr<io::data>& d) {
  if (_serialized_data.empty())
    throw (io::exceptions::shutdown(true, true)
           << "node cache is empty");
  else {
    d = _serialized_data.front();
    _serialized_data.pop_front();
  }
}

/**
 *  Write event to the node cache.
 *
 *  @param[in] data  The data event.
 *
 *  @return          Number of event acknowledged.
 */
unsigned int node_cache::write(misc::shared_ptr<io::data> const& data) {
  // Check that data exists.
  if (data.isNull())
    return (1);

  unsigned int type = data->type();
  if (type == neb::host::static_type())
    update(*data.staticCast<neb::host>());
  else if (type == neb::host_status::static_type())
    update(*data.staticCast<neb::host_status>());
  else if (type == neb::host_group_member::static_type())
    update(*data.staticCast<neb::host_group_member>());
  else if (type == neb::service::static_type())
    update(*data.staticCast<neb::service>());
  else if (type == neb::service_status::static_type())
    update(*data.staticCast<neb::service_status>());
  else if (type == neb::service_group_member::static_type())
    update(*data.staticCast<neb::service_group_member>());
  else if (type == neb::custom_variable::static_type()
           || type == neb::custom_variable_status::static_type())
    update(*data.staticCast<neb::custom_variable_status>());

  return (1);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host const& hst) {
  if (hst.host_id == 0)
    return ;
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
    return ;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hst.host_id)].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hgm  The data to update.
 */
void node_cache::update(neb::host_group_member const& hgm) {
  if (hgm.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _host_node_states[objects::node_id(hgm.host_id)].update(hgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] s  The data to update.
 */
void node_cache::update(neb::service const& s) {
  if (s.service_id == 0)
    return ;
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
    return ;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(sst.host_id, sst.service_id)].update(sst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sgm  The data to update.
 */
void node_cache::update(neb::service_group_member const& sgm) {
  if (sgm.service_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  _service_node_states[objects::node_id(sgm.host_id, sgm.service_id)].update(sgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] cvs  The data to update.
 */
void node_cache::update(neb::custom_variable_status const& cvs) {
  if (cvs.host_id == 0)
    return ;
  QMutexLocker lock(&_mutex);
  if (cvs.service_id == 0)
    _host_node_states[objects::node_id(cvs.host_id)].update(cvs);
  else
    _service_node_states[objects::node_id(cvs.host_id, cvs.service_id)].update(cvs);
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
    throw (exceptions::msg() << "notification: host "
           << id.get_host_id() << " was not found in cache");
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
    throw (exceptions::msg()
           << "notification: service (" << id.get_host_id() << ", "
           << id.get_service_id() << " was not found in cache");
  return (*found);
}

/**
 *  Get all the node contained in a group.
 *
 *  @param[in] group_name     The name of the group.
 *  @param[in] is_host_group  Is this a host group or a service group?
 *
 *  @return  A list of the name of the node contained in a group.
 */
std::vector<std::string> node_cache::get_all_node_contained_in(
                                       std::string const& group_name,
                                       bool is_host_group) const {
  std::vector<std::string> res;

  if (is_host_group) {
    for (QHash<objects::node_id, host_node_state>::const_iterator
           it(_host_node_states.begin()),
           end(_host_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().host_name.toStdString());
  }
  else {
    for (QHash<objects::node_id, service_node_state>::const_iterator
           it(_service_node_states.begin()),
           end(_service_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().service_description.toStdString());
  }

  return (res);
}

/**
 *  Prepare the serialization of the host and service states.
 */
void node_cache::_prepare_serialization() {
  _serialized_data.clear();
  for (QHash<objects::node_id, host_node_state>::const_iterator
         it = _host_node_states.begin(),
         end = _host_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
  for (QHash<objects::node_id, service_node_state>::const_iterator
         it = _service_node_states.begin(),
         end = _service_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
}
