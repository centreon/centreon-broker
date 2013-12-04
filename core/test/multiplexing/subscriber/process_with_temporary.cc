/*
** Copyright 2011-2013 Merethis
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

#include <cstring>
#include <memory>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/temporary.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "temporary_endpoint.hh"

using namespace com::centreon::broker;

char const* msg[] = {
  "0123456789abcdef-1",
  "0123456789abcdef-2",
  "0123456789abcdef-3"
};

/**
 *  Check that multiplexing subscriber works properly with using
 *  temporary.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Create temporary endpoint.
  misc::shared_ptr<io::endpoint> temporary(new temporary_endpoint);
  io::temporary::instance().set(temporary);

  // Subscriber.
  multiplexing::subscriber::event_queue_max_size(1);
  multiplexing::subscriber s("temporary_prefix_name");

  // Return value.
  int retval(0);

  // Event pointer.
  misc::shared_ptr<io::data> event;

  // Close subscriber.
  s.process(false, false);

  // Publish data.
  for (unsigned int i(0); i < 3; ++i) {
    misc::shared_ptr<io::raw> data(new io::raw);
    data = misc::shared_ptr<io::raw>(new io::raw);
    data->append(msg[i]);
    multiplexing::engine::instance().publish(data.staticCast<io::data>());
  }

  // Fetch event.
  for (unsigned int i(0); i < 3; ++i) {
    s.read(event, 0);
    retval |= (event.isNull()
               || (event->type() != io::events::data_type<io::events::internal, 1>::value)
               || strncmp(
                    event.staticCast<io::raw>()->QByteArray::data(),
                    msg[i],
                    strlen(msg[i])));
  }

  // Subscriber should throw.
  s.process(false, true);

  // Try reading again.
  try {
    s.read(event, 0);
    retval |= 1;
  }
  catch (io::exceptions::shutdown const& s) {
    (void)s;
  }
  catch (...) {
    retval |= 1;
  }

  // Cleanup.
  config::applier::deinit();

  // Return.
  return (retval);
}
