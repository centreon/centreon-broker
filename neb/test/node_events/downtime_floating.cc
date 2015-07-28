/*
** Copyright 2011-2015 Merethis
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
