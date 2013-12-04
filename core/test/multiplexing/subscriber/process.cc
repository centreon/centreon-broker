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
