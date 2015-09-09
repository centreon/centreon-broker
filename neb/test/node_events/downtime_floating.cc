/*
** Copyright 2011-2015 Centreon
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

#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <QMap>
#include <QPair>
#include <QThread>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "common.hh"
#include "vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Check that node events floating downtimes properly work.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Initialization.
  config::applier::init();
  multiplexing::engine::load();
  // Start the multiplexing engine.
  test_stream t;
  multiplexing::engine::instance().hook(t);
  multiplexing::engine::instance().start();

  try {
    // Get time.
    time_t now = ::time(NULL);

    // Create node event stream.
    node_events_stream test(
      "1",
      misc::shared_ptr<persistent_cache>(NULL),
      PROJECT_SOURCE_DIR "/neb/test/node_events/cfg/downtime.cfg");

    // Send initial service status.
    {
      misc::shared_ptr<neb::service> sst(new neb::service);
      sst->host_id = 42;
      sst->service_id = 24;
      sst->last_hard_state = 0;
      sst->last_hard_state_change = 123456789;
      sst->host_name = "42";
      sst->service_description = "24";
      test.write(sst);
    }
    {
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456789;
      test.write(ss);
    }

    // Send external command.
    {
      misc::shared_ptr<extcmd::command_request>
        cmd(new extcmd::command_request);
      cmd->cmd = format_command(
        "SCHEDULE_SVC_DOWNTIME;42;24;$TIMESTAMP$;$TIMESTAMP2$;0;0;3;TEST;A test for you;0;",
        now,
        now + 3);
      test.write(cmd);
    }

    // Send error service status.
    {
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 1;
      ss->last_hard_state_change = now - 1;
      test.write(ss);
    }
    {
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 1;
      ss->last_hard_state_change = now + 1;
      test.write(ss);
    }

    ::sleep(5);

    // Check content.
    multiplexing::engine::instance().stop();
    t.finalize();

    QList<misc::shared_ptr<io::data> > content;
    add_downtime(content, now, now + 3, 3, false, 42, 24, 1, 1, -1, -1);
    add_downtime(content, now, now + 3, 3, false, 42, 24, 1, 1, now + 1, -1);
    add_downtime(content, now, now + 3, 3, false, 42, 24, 1, 1, now + 1, now + 4);

    // Check.
    check_content(t, content);

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  return (retval);
}
