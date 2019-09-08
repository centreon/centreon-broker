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
#include <sstream>
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

#define TEST_NAME "rtmonitoring_v2_downtimes"
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
static void postcheck(test::db& db, test::predicate expected[][18]) {
  static std::string check_query(
      "SELECT entry_time, host_id, service_id, actual_end_time,"
      "       actual_start_time, author, cancelled, comment_data,"
      "       deletion_time, duration, end_time, fixed, instance_id,"
      "       internal_id, start_time, started, triggered_by, type"
      "  FROM downtimes"
      "  ORDER BY entry_time ASC, host_id ASC, service_id ASC");
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the downtimes table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "services", "downtimes",
                            NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5584");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:host_status</category>"
                "  <category>neb:service</category>"
                "  <category>neb:service_status</category>"
                "  <category>neb:downtime</category>"
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
    cbmod_cfg.set("TCP_PORT", "5584");
    test::centengine_config engine_config;
    engine_config.generate_hosts(2);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_hosts().begin()),
         end(engine_config.get_hosts().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("passive_checks_enabled", "1");
      it->set("max_check_attempts", "1");
    }
    engine_config.generate_services(2);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("accept_passive_host_checks", "1");
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entry.
    precheck(tpoints,
             "entry_time, host_id, start_time, end_time, author, comment_data,"
             " duration, instance_id");
    engine.start();
    test::sleep_for(2);
    tpoints.store();
    test::predicate expected[5][18] = {
        {test::predicate(), 1u, 1u, test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null), "admin", false,
         "This is my comment.", test::predicate(test::predicate::type_null), 0,
         test::predicate(), true, 42u, 1, test::predicate(), false,
         test::predicate(test::predicate::type_null), 1},
        {test::predicate()}};
    {
      time_t now(time(NULL));
      time_t start_time(now + 4 * MONITORING_ENGINE_INTERVAL_LENGTH);
      int duration(4 * MONITORING_ENGINE_INTERVAL_LENGTH);
      time_t end_time(start_time + duration);
      std::ostringstream cmd;
      cmd << "SCHEDULE_SVC_DOWNTIME;1;1;" << start_time << ";" << end_time
          << ";1;0;" << duration << ";admin;This is my comment.";
      expected[0][9] = duration;
      expected[0][10] = end_time;
      expected[0][14] = start_time;
      engine.extcmd().execute(cmd.str());
    }
    test::sleep_for(2);
    tpoints.store();
    expected[0][0] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    postcheck(db, expected);

    // Check actual_start_time, started.
    precheck(tpoints, "actual_start_time, started");
    test::sleep_for(3);
    tpoints.store();
    expected[0][4] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[0][15] = true;
    postcheck(db, expected);

    // Check actual_end_time.
    precheck(tpoints, "actual_end_time");
    test::sleep_for(4);
    tpoints.store();
    expected[0][3] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    postcheck(db, expected);

    // Check fixed, internal_id, type.
    precheck(tpoints, "fixed, internal_id, type");
    {
      time_t now(time(NULL));
      time_t start_time(now);
      int duration(3600);
      time_t end_time(start_time + duration);
      std::ostringstream cmd;
      cmd << "SCHEDULE_HOST_DOWNTIME;2;" << start_time << ";" << end_time
          << ";0;0;" << duration << ";Merethis;My Centreon is fantastic !";
      expected[1][9] = duration;
      expected[1][10] = end_time;
      expected[1][14] = start_time;
      engine.extcmd().execute(cmd.str());
    }
    engine.extcmd().execute("PROCESS_HOST_CHECK_RESULT;2;1;output");
    test::sleep_for(2);
    expected[1][0] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][1] = 2u;
    expected[1][2] = test::predicate(test::predicate::type_null);
    expected[1][3] = test::predicate(test::predicate::type_null);
    expected[1][4] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][5] = "Merethis";
    expected[1][6] = false;
    expected[1][7] = "My Centreon is fantastic !";
    expected[1][8] = test::predicate(test::predicate::type_null);
    expected[1][11] = false;
    expected[1][12] = 42u;
    expected[1][13] = 2;
    expected[1][15] = true;
    expected[1][16] = test::predicate(test::predicate::type_null);
    expected[1][17] = 2;
    expected[2][0] = test::predicate();
    postcheck(db, expected);

    // Check cancelled and deletion_time.
    precheck(tpoints, "cancelled, deletion_time");
    engine.extcmd().execute("DEL_HOST_DOWNTIME;2");
    test::sleep_for(2);
    expected[1][3] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][6] = true;
    expected[1][8] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    postcheck(db, expected);

    // Check triggered_by.
    precheck(tpoints, "triggered_by");
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
