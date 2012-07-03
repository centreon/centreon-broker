/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
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

  // Subscriber.
  multiplexing::subscriber s;

  // Hook.
  hooker h;
  h.process(true, true);

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
  int retval(0);
  {
    misc::shared_ptr<io::data> data;
    s.read(data, 0);
    retval |= !data.isNull();
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
      s.read(d, 0);
      if (d.isNull()
          || (d->type() != "com::centreon::broker::io::raw"))
        retval |= 1;
      else {
        misc::shared_ptr<io::raw> raw(d.staticCast<io::raw>());
        retval |= strncmp(
          raw->QByteArray::data(),
          messages[i],
          strlen(messages[i]));
      }
    }
    misc::shared_ptr<io::data> d;
    s.read(d, 0);
    retval |= !d.isNull();
  }

  // Return.
  return (retval);
}
