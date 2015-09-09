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
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

#define MSG "0123456789abcdef"

/**
 *  Check that multiplexing subscriber works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Subscriber.
  multiplexing::subscriber s("temporary_prefix_name");

  // Return value.
  int retval(0);

  // Check that subscriber is empty.
  misc::shared_ptr<io::data> event;
  s.read(event, 0);
  retval |= !event.isNull();

  // Close subscriber.
  s.process(false, false);

  // Publish data.
  misc::shared_ptr<io::raw> data(new io::raw);
  data = misc::shared_ptr<io::raw>(new io::raw);
  data->append(MSG);
  multiplexing::engine::instance().publish(data.staticCast<io::data>());

  // Fetch event.
  s.read(event, 0);
  retval |= (event.isNull()
             || (event->type() != io::events::data_type<io::events::internal, 1>::value)
             || strncmp(
                  event.staticCast<io::raw>()->QByteArray::data(),
                  MSG,
                  sizeof(MSG) - 1));

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
