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

#include <exception>
#include <QMutexLocker>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/neb/internal.hh"

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
    _service_statuses = obj._service_statuses;
    _host_statuses = obj._host_statuses;
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
    << "Notification: loading the node cache " << cache_file <<".";

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
  catch (io::exceptions::shutdown) {
    // Normal termination of the stream (ie nothing to read anymore)
    logging::debug(logging::low)
      << "Notification: finished loading the node cache "
      << cache_file << " succesfully.";
  }
  catch (std::exception e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "Notification: could not load the node cache "
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

  misc::shared_ptr<io::data> data;

  try {
    while (true) {
      bbdo->read(data);
      write(data);
    }
  }
  catch (io::exceptions::shutdown) {
    // Normal termination of the stream (ie nothing to write anymore)
    logging::debug(logging::low)
      << "Notification: finished writing the node cache "
      << cache_file << " succesfully.";
  }
  catch (std::exception e) {
    // Abnormal termination of the stream.
    logging::error(logging::high)
      << "Notification: could not write the node cache "
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
void node_cache::read(misc::shared_ptr<io::data> &d) {

  QMutexLocker lock(&_mutex);

  if (_host_statuses.empty() && _service_statuses.empty())
    throw (io::exceptions::shutdown(true, true)
           << "Node cache is empty");

  // Get host status.
  if (!_host_statuses.empty()) {
    misc::shared_ptr<neb::host_status> hst = _host_statuses.pop();
    d = hst;
    return ;
  }

  // Get service status.
  if (!_service_statuses.empty()) {
    misc::shared_ptr<neb::service_status> sst = _service_statuses.pop();
    d = sst;
    return ;
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
    return 1;

  unsigned int type = data->type();

  QMutexLocker lock(&_mutex);

  if (type == io::events::data_type<io::events::neb,
                                    neb::de_host_status>::value) {
    misc::shared_ptr<neb::host_status>
        hst = data.staticCast<neb::host_status>();
    _host_statuses.insert(hst->host_id, hst);
  }
  else if (type == io::events::data_type<io::events::neb,
                                         neb::de_service_status>::value) {
    misc::shared_ptr<neb::service_status>
        sst = data.staticCast<neb::service_status>();
    _service_statuses.insert(sst->service_id, sst);
  }
}
