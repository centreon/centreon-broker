/*
** Copyright 2015 Centreon
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
#include "test/cbd.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/db.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/predicate.hh"
#include "test/time_points.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "rtmonitoring_v2_eventhandlers"
#define DB_NAME "broker_" TEST_NAME

// Check count.
static int check_number(0);

/**
 *  Precheck routine.
 */
static void precheck(test::time_points& tpoints, char const* name) {
  ++check_number;
  std::cout << "check #" << check_number << " (" << name << ")\n";
  tpoints.store();
  return;
}

/**
 *  Postcheck routine.
 */
static void postcheck(test::db& db, test::predicate expected[][14]) {
  static std::string check_query(
      "SELECT host_id, service_id, start_time, command_args,"
      "       command_line, early_timeout, end_time, execution_time,"
      "       output, return_code, state, state_type, timeout, type"
      "  FROM eventhandlers"
      "  ORDER BY start_time ASC, host_id ASC,"
      "           COALESCE(service_id, 0) ASC");
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the event handlers table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "services", "eventhandlers",
                            NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5585");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:host_status</category>"
                "  <category>neb:service</category>"
                "  <category>neb:service_status</category>"
                "  <category>neb:event_handler</category>"
                "</write_filters>");
    test::cbd broker;
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    test::sleep_for(1);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/tcp.xml.in");
    cbmod_cfg.set("BROKER_ID", "83");
    cbmod_cfg.set("BROKER_NAME", TEST_NAME "-cbmod");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("TCP_HOST", "localhost");
    cbmod_cfg.set("TCP_PORT", "5585");
    test::centengine_config engine_config;
    {
      test::centengine_object cmd(test::centengine_object::command_type);
      cmd.set("command_name", "test_command");
      cmd.set("command_line", MY_PLUGIN_PATH " $ARG1$ '$ARG2$'");
      engine_config.get_commands().push_back(cmd);
    }
    engine_config.generate_hosts(2);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_hosts().begin()),
         end(engine_config.get_hosts().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("passive_checks_enabled", "1");
      it->set("max_check_attempts", "2");
      it->set("event_handler_enabled", "1");
      it->set("event_handler", "test_command!0!host event handler");
    }
    engine_config.generate_services(2);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_services().begin()),
         end(engine_config.get_services().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("passive_checks_enabled", "1");
      it->set("max_check_attempts", "2");
      it->set("event_handler_enabled", "1");
      it->set("event_handler", "test_command!0!service event handler");
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("execute_host_checks", "0");
    engine_config.set_directive("execute_service_checks", "0");
    engine_config.set_directive("accept_passive_host_checks", "1");
    engine_config.set_directive("accept_passive_service_checks", "1");
    engine_config.set_directive("enable_event_handlers", "1");
    engine_config.set_directive("event_handler_timeout", "5");
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entry.
    precheck(tpoints,
             "host_id, service_id, start_time, command_args, command_line,"
             " end_time, execution_time, output, return_code, state");
    engine.start();
    test::sleep_for(1);
    tpoints.store();
    engine.extcmd().execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output");
    test::sleep_for(2);
    tpoints.store();
    test::predicate expected[5][14] = {
        {1u, 1u, test::predicate(tpoints.prelast(), tpoints.last() + 1),
         "0!service event handler", MY_PLUGIN_PATH " 0 'service event handler'",
         false, test::predicate(tpoints.prelast(), tpoints.last() + 1),
         test::predicate(0.0, 1.0), "service event handler", 0, 2, 0, 5, 1},
        {test::predicate()}};
    postcheck(db, expected);

    // Check state_type.
    precheck(tpoints, "state_type");
    engine.extcmd().execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output");
    test::sleep_for(2);
    tpoints.store();
    expected[1][0] = 1u;
    expected[1][1] = 1u;
    expected[1][2] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][3] = "0!service event handler";
    expected[1][4] = MY_PLUGIN_PATH " 0 'service event handler'";
    expected[1][5] = false;
    expected[1][6] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][7] = test::predicate(0.0, 1.0);
    expected[1][8] = "service event handler";
    expected[1][9] = 0;
    expected[1][10] = 2;
    expected[1][11] = 1;
    expected[1][12] = 5;
    expected[1][13] = 1;
    expected[2][0] = test::predicate();
    postcheck(db, expected);

    // Check type.
    precheck(tpoints, "type");
    engine.extcmd().execute("PROCESS_HOST_CHECK_RESULT;2;1;output");
    test::sleep_for(2);
    tpoints.store();
    expected[2][0] = 2u;
    expected[2][1] = test::predicate(test::predicate::type_null);
    expected[2][2] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[2][3] = "0!host event handler";
    expected[2][4] = MY_PLUGIN_PATH " 0 'host event handler'";
    expected[2][5] = false;
    expected[2][6] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[2][7] = test::predicate(0.0, 1.0);
    expected[2][8] = "host event handler";
    expected[2][9] = 0;
    expected[2][10] = 1;
    expected[2][11] = 0;
    expected[2][12] = 5;
    expected[2][13] = 0;
    expected[3][0] = test::predicate();
    postcheck(db, expected);

    // Check early_timeout.
    precheck(tpoints, "early_timeout");
    std::cout << "  not tested\n";

    // Check timeout.
    precheck(tpoints, "timeout");
    std::cout << "  not tested\n";

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
    broker.stop();
  } catch (std::exception const& e) {
    std::cout << "  " << e.what() << "\n";
  } catch (...) {
    std::cout << "  unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
