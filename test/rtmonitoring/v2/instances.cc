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

#define TEST_NAME "rtmonitoring_v2_instances"
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
static void postcheck(test::centengine& engine,
                      test::time_points& tpoints,
                      test::db& db,
                      test::predicate expected[][24]) {
  static std::string check_query(
      "SELECT instance_id, name, active_host_checks,"
      "       active_service_checks, check_hosts_freshness,"
      "       check_services_freshness, deleted, end_time, engine,"
      "       event_handlers, flap_detection, global_host_event_handler,"
      "       global_service_event_handler, last_alive,"
      "       last_command_check, notifications, obsess_over_hosts,"
      "       obsess_over_services, outdated, passive_host_checks,"
      "       passive_service_checks, pid, running, start_time"
      // XXX : not checked yet       "       version"
      "  FROM instances");
  engine.reload();
  test::sleep_for(3);
  tpoints.store();
  expected[0][13] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
  expected[0][14] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the instances table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5571");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
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
    cbmod_cfg.set("TCP_PORT", "5571");
    test::centengine_config engine_config;
    engine_config.generate_hosts(3);
    engine_config.generate_services(5);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("accept_passive_host_checks", "0");
    engine_config.set_directive("accept_passive_service_checks", "0");
    engine_config.set_directive("check_host_freshness", "0");
    engine_config.set_directive("check_service_freshness", "0");
    engine_config.set_directive("enable_event_handlers", "0");
    engine_config.set_directive("enable_flap_detection", "0");
    engine_config.set_directive("enable_notifications", "0");
    engine_config.set_directive("execute_host_checks", "0");
    engine_config.set_directive("execute_service_checks", "0");
    engine_config.set_directive("obsess_over_hosts", "0");
    engine_config.set_directive("obsess_over_services", "0");
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entry.
    precheck(
        tpoints,
        "engine, last_alive, last_command_check, pid, running, start_time");
    engine.start();
    test::sleep_for(1);
    tpoints.store();
    test::predicate expected[][24] = {
        {42u,
         "my-poller",
         false,
         false,
         false,
         false,
         false,
         test::predicate(test::predicate::type_null),
         "Centreon Engine",
         false,
         false,
         "",
         "",
         test::predicate(tpoints.prelast(), tpoints.last() + 1),
         test::predicate(test::predicate::type_null),
         false,
         false,
         false,
         false,
         false,
         false,
         true,  // PID will be converted to boolean, any non-0 will match.
         true,
         test::predicate(tpoints.prelast(), tpoints.last() + 1)},
        {test::predicate()}};
    postcheck(engine, tpoints, db, expected);

    // Check name.
    precheck(tpoints, "name");
    // XXX
    // postcheck(engine, tpoints, db, expected);
    std::cout << "  not tested\n";

    // Check active_host_checks.
    precheck(tpoints, "active_host_checks");
    engine_config.set_directive("execute_host_checks", "1");
    expected[0][2] = true;
    postcheck(engine, tpoints, db, expected);

    // Check active_service_checks.
    precheck(tpoints, "active_service_checks");
    engine_config.set_directive("execute_service_checks", "1");
    expected[0][3] = true;
    postcheck(engine, tpoints, db, expected);

    // Check check_hosts_freshness.
    precheck(tpoints, "check_hosts_freshness");
    engine_config.set_directive("check_host_freshness", "1");
    expected[0][4] = true;
    postcheck(engine, tpoints, db, expected);

    // Check check_service_freshness.
    precheck(tpoints, "check_service_freshness");
    engine_config.set_directive("check_service_freshness", "1");
    expected[0][5] = true;
    postcheck(engine, tpoints, db, expected);

    // Check event_handlers.
    precheck(tpoints, "event_handlers");
    engine_config.set_directive("enable_event_handlers", "1");
    expected[0][9] = true;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection.
    precheck(tpoints, "flap_detection");
    engine_config.set_directive("enable_flap_detection", "1");
    expected[0][10] = true;
    postcheck(engine, tpoints, db, expected);

    // Check global_host_event_handler.
    precheck(tpoints, "global_host_event_handler");
    engine_config.set_directive("global_host_event_handler", "default_command");
    expected[0][11] = "default_command";
    postcheck(engine, tpoints, db, expected);

    // Check global_service_event_handler.
    precheck(tpoints, "global_service_event_handler");
    engine_config.set_directive("global_service_event_handler",
                                "default_command");
    expected[0][12] = "default_command";
    postcheck(engine, tpoints, db, expected);

    // Check notifications.
    precheck(tpoints, "enable_notifications");
    engine_config.set_directive("enable_notifications", "1");
    expected[0][15] = true;
    postcheck(engine, tpoints, db, expected);

    // Check obsess_over_hosts.
    precheck(tpoints, "obsess_over_hosts");
    engine_config.set_directive("obsess_over_hosts", "1");
    expected[0][16] = true;
    postcheck(engine, tpoints, db, expected);

    // Check obsess_over_services.
    precheck(tpoints, "obsess_over_services");
    engine_config.set_directive("obsess_over_services", "1");
    expected[0][17] = true;
    postcheck(engine, tpoints, db, expected);

    // Check passive_host_checks.
    precheck(tpoints, "passive_host_checks");
    engine_config.set_directive("accept_passive_host_checks", "1");
    expected[0][19] = true;
    postcheck(engine, tpoints, db, expected);

    // Check passive_service_checks.
    precheck(tpoints, "passive_service_checks");
    engine_config.set_directive("accept_passive_service_checks", "1");
    expected[0][20] = true;
    postcheck(engine, tpoints, db, expected);

    // Check version.
    precheck(tpoints, "version");
    // XXX
    // postcheck(engine, tpoints, db, expected);
    std::cout << "  not tested\n";

    // Check end_time.
    precheck(tpoints, "end_time");
    engine.stop();
    test::sleep_for(2);
    tpoints.store();
    {
      test::predicate expected_end_time[][3] = {
          {42u, test::predicate(tpoints.prelast(), tpoints.last() + 1), false},
          {test::predicate()}};
      db.check_content("SELECT instance_id, end_time, running FROM instances",
                       expected_end_time);
    }
    std::cout << "  passed\n";

    // Check outdated.
    precheck(tpoints, "outdated");
    // XXX
    // postcheck(engine, tpoints, db, expected);
    std::cout << "  not tested\n";

    // Check deleted.
    precheck(tpoints, "deleted");
    // XXX
    // postcheck(engine, tpoints, db, expected);
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
