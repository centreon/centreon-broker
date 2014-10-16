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
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/notification/node_cache.hh"

using namespace com::centreon::broker::notification;

node_cache::node_cache() {}

node_cache::node_cache(node_cache const& obj) {
  node_cache::operator=(obj);
}

node_cache& node_cache::operator=(node_cache const& obj) {
  if (this != &obj) {
    _service_statuses = obj._service_statuses;
    _host_statuses = obj._host_statuses;
  }
  return (*this);
}

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

void node_cache::read(misc::shared_ptr<io::data> &d) {
  //if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
           << "Node cache is empty");

}

unsigned int node_cache::write(const misc::shared_ptr<io::data> &data) {
  // Check that data exists.
  unsigned int retval(1);

  if (data.isNull())
    return 1;

  unsigned int type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));
}
