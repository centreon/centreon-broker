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

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**
 *  Default constructor.
 */
node_cache::node_cache()
  : _mutex(QMutex::NonRecursive) {}

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
 *  @return         A reference tot his object.
 */
node_cache& node_cache::operator=(node_cache const& obj) {
  if (this != &obj) {
    _host_node_states = obj._host_node_states;
    _service_node_states = obj._service_node_states;
  }
  return (*this);
}

/**
 *  Load the node cache from a file.
 *
 *  @param[in] cache_file  The cache file.
 *
 *  @return  True if the node cache was sucessfully loaded.
 */
bool node_cache::load(std::string const& cache_file) {
  logging::debug(logging::low)
    << "notification: loading the node cache " << cache_file;

  // Create the streams.
  misc::shared_ptr<file::stream> file(new file::stream(cache_file));
  misc::shared_ptr<compression::stream> compression(new compression::stream(9));
  misc::shared_ptr<bbdo::stream> bbdo(new bbdo::stream(true, false));

  // Connect the streams.
  file->process(true, false);
  compression->process(true, false);
  compression->read_from(file);
  bbdo->read_from(compression);

  misc::shared_ptr<io::data> data;
  try {
    while (true) {
      bbdo->read(data);
      write(data);
    }
  }
  catch (io::exceptions::shutdown const& s) {
    // Normal termination of the stream (ie nothing to read anymore)
    (void)s;
    logging::debug(logging::low)
      << "notification: finished loading the node cache "
      << cache_file << " succesfully";
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not load the node cache "
      << cache_file << ": " << e.what();
    return (false);
  }

  return (true);
}

/**
 *  Save the node_cache to a file.
 *
 *  @param[in] cache_file  The cache file to save.
 *
 *  @return  True if the node cache was succesfully saved.
 */
bool node_cache::unload(std::string const& cache_file) {
  // Create the streams.
  misc::shared_ptr<file::stream> file(new file::stream(cache_file));
  misc::shared_ptr<compression::stream> compression(new compression::stream(9));
  misc::shared_ptr<bbdo::stream> bbdo(new bbdo::stream(false, true));

  // Connect the streams.
  file->process(false, true);
  compression->process(false, true);
  compression->write_to(file);
  bbdo->write_to(compression);

  // Lock the mutex;
  QMutexLocker lock(&_mutex);

  // Prepare serialization.
  _prepare_serialization();

  misc::shared_ptr<io::data> data;

  try {
    while (true) {
      read(data);
      bbdo->write(data);
    }
  }
  catch (io::exceptions::shutdown const& s) {
    // Normal termination of the stream (ie nothing to write anymore).
    (void)s;
    logging::debug(logging::low)
      << "notification: finished writing the node cache "
      << cache_file << " succesfully";
  }
  catch (std::exception const& e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "notification: could not write the node cache "
      << cache_file << ": " << e.what();
    return (false);
  }

  return (true);
}

void node_cache::process(bool in, bool out) {
  (void)in;
  (void)out;
}

/**
 *  Read events from the node cache.
 *
 *  @param[out] d  An output data event.
 */
void node_cache::read(misc::shared_ptr<io::data>& d) {
  if (_serialized_data.empty())
    throw (io::exceptions::shutdown(true, true)
           << "Node cache is empty");
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
  unsigned int retval(1);

  if (data.isNull())
    return (1);

  unsigned int type = data->type();

  if (type == io::events::data_type<io::events::neb,
                                    neb::de_host_status>::value) {
    misc::shared_ptr<neb::host_status>
        hst = data.staticCast<neb::host_status>();
    QMutexLocker lock(&_mutex);
    //_host_statuses.insert(hst->host_id, hst);
  }
  else if (type == io::events::data_type<io::events::neb,
                                         neb::de_service_status>::value) {
    misc::shared_ptr<neb::service_status>
        sst = data.staticCast<neb::service_status>();
    QMutexLocker lock(&_mutex);
    //_service_statuses.insert(sst->service_id, sst);
  }

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

  _host_node_states[hst.host_id].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hst  The data to update.
 */
void node_cache::update(neb::host_status const& hst) {
  if (hst.host_id == 0)
    return ;

  _host_node_states[hst.host_id].update(hst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] hgm  The data to update.
 */
void node_cache::update(neb::host_group_member const& hgm) {
  if (hgm.host_id == 0)
    return ;

  _host_node_states[hgm.host_id].update(hgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] s  The data to update.
 */
void node_cache::update(neb::service const& s) {
  if (s.service_id == 0)
    return ;

  _service_node_states[s.service_id].update(s);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sst  The data to update.
 */
void node_cache::update(neb::service_status const& sst) {
  if (sst.service_id == 0)
    return ;

  _service_node_states[sst.service_id].update(sst);
}

/**
 *  Update the node cache.
 *
 *  @param[in] sgm  The data to update.
 */
void node_cache::update(neb::service_group_member const& sgm) {
  if (sgm.service_id == 0)
    return ;

  _service_node_states[sgm.service_id].update(sgm);
}

/**
 *  Update the node cache.
 *
 *  @param[in] cv  The data to update.
 */
void node_cache::update(neb::custom_variable const& cv) {
  if (cv.host_id == 0)
    return ;

  if (cv.service_id == 0)
    _host_node_states[cv.host_id].update(cv);
  else
    _service_node_states[cv.service_id].update(cv);
}

/**
 *  Get a host from the node cache.
 *
 *  @param[in] id  The id of the host.
 *
 *  @return        The host from the node cache.
 */
node_cache::host_node_state const& node_cache::get_host(
                                                 unsigned int id) const {
  QHash<unsigned int, host_node_state>::const_iterator found =
    _host_node_states.find(id);
  if (found == _host_node_states.end())
    throw (exceptions::msg()
             << "notification: node_cache: host " << id << "not found.");
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
                                                    unsigned int id) const {
  QHash<unsigned int, service_node_state>::const_iterator found =
    _service_node_states.find(id);
  if (found == _service_node_states.end())
    throw (exceptions::msg()
             << "notification: node_cache: service " << id << "not found.");
  return (*found);
}

/**
 *  Prepare the serialization of the host and service states.
 */
void node_cache::_prepare_serialization() {
  _serialized_data.clear();
  for (QHash<unsigned int, host_node_state>::const_iterator
         it = _host_node_states.begin(),
         end = _host_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
  for (QHash<unsigned int, service_node_state>::const_iterator
         it = _service_node_states.begin(),
         end = _service_node_states.end();
       it != end;
       ++it)
    it->serialize(_serialized_data);
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

  if (is_host_group)
    for (QHash<unsigned int, host_node_state>::const_iterator
           it(_host_node_states.begin()),
           end(_host_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().host_name.toStdString());
  else
    for (QHash<unsigned int, service_node_state>::const_iterator
           it(_service_node_states.begin()),
           end(_service_node_states.end());
        it != end;
        ++it)
      if (it->get_groups().count(group_name) != 0)
        res.push_back(it->get_node().host_name.toStdString());

  return (res);
}

