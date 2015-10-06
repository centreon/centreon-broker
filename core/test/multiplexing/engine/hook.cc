/*
** Copyright 2011-2013,2015 Centreon
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

#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "test/multiplexing/engine/hooker.hh"

using namespace com::centreon::broker;

#define MSG1 "0123456789abcdef"
#define MSG2 "foo bar baz"
#define MSG3 "last message with qux"
#define MSG4 "no this is the last message"

/**
 *  Check that multiplexing engine works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();
  bool error(true);

  try {
    // Subscriber.
    uset<unsigned int> filters;
    filters.insert(io::raw::static_type());
    multiplexing::subscriber
      s("core_multiplexing_engine_hook", "");
    s.get_muxer().set_read_filters(filters);
    s.get_muxer().set_write_filters(filters);

    // Hook.
    hooker h;
    h.hook(true);

    // Send events through engine.
    {
      char const* messages[] = { MSG1, MSG2, NULL };
      for (unsigned int i = 0; messages[i]; ++i) {
        misc::shared_ptr<io::raw> data(new io::raw);
        data->append(messages[i]);
        multiplexing::engine::instance().publish(
          data.staticCast<io::data>());
      }
    }

    // Should read no events from subscriber.
    {
      misc::shared_ptr<io::data> data;
      s.get_muxer().read(data, 0);
      if (!data.isNull())
        throw (exceptions::msg() << "error at step #1");
    }

    // Start multiplexing engine.
    multiplexing::engine::instance().start();

    // Publish a new event.
    {
      misc::shared_ptr<io::raw> data(new io::raw);
      data->append(MSG3);
      multiplexing::engine::instance().publish(
        data.staticCast<io::data>());
    }

    // Stop multiplexing engine.
    multiplexing::engine::instance().stop();

    // Publish a new event.
    {
      misc::shared_ptr<io::raw> data(new io::raw);
      data->append(MSG4);
      multiplexing::engine::instance().publish(
        data.staticCast<io::data>());
    }

    // Check subscriber content.
    {
      char const* messages[] =
        { HOOKMSG1, MSG1, HOOKMSG2, MSG2, HOOKMSG2, MSG3, HOOKMSG2, HOOKMSG3, NULL };
      for (unsigned int i = 0; messages[i]; ++i) {
        misc::shared_ptr<io::data> d;
        s.get_muxer().read(d, 0);
        if (d.isNull()
            || (d->type() != io::raw::static_type()))
          throw (exceptions::msg() << "error at step #2");
        else {
          misc::shared_ptr<io::raw> raw(d.staticCast<io::raw>());
          if (strncmp(
                raw->QByteArray::data(),
                messages[i],
                strlen(messages[i])))
            throw (exceptions::msg() << "error at step #3");
        }
      }
      misc::shared_ptr<io::data> d;
      s.get_muxer().read(d, 0);
      if (!d.isNull())
        throw (exceptions::msg() << "error at step #4");
    }

    // Unhook.
    h.hook(false);

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  catch (...) {
    std::cerr << "unknown exception\n";
  }

  // Cleanup.
  config::applier::deinit();

  // Return.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
