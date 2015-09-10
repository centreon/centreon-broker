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
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

#define MSG1 "0123456789abcdef"
#define MSG2 "foo bar baz qux"

/**
 *  We should not be able to read from publisher.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Publisher.
  multiplexing::publisher p;

  // Subscriber.
  multiplexing::subscriber s("temporary_prefix_name");

  // Publish event.
  {
    misc::shared_ptr<io::raw> raw(new io::raw);
    raw->append(MSG1);
    p.write(raw.staticCast<io::data>());
  }

  // Launch multiplexing.
  multiplexing::engine::instance().start();

  // Publish another event.
  {
    misc::shared_ptr<io::raw> raw(new io::raw);
    raw->append(MSG2);
    p.write(raw.staticCast<io::data>());
  }

  // Check data.
  int retval(0);
  char const* messages[] = { MSG1, MSG2, NULL };
  for (unsigned int i = 0; messages[i]; ++i) {
    misc::shared_ptr<io::data> data;
    s.read(data, 0);
    if (data.isNull()
        || (data->type() != io::events::data_type<io::events::internal, 1>::value))
      retval |= 1;
    else {
      misc::shared_ptr<io::raw> raw(data.staticCast<io::raw>());
      retval |= strncmp(
        raw->QByteArray::data(),
        messages[i],
        strlen(messages[i]));
    }
  }

  // Cleanup.
  config::applier::deinit();

  // Return.
  return (retval);
}
