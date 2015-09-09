/*
** Copyright 2011-2013 Centreon
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
