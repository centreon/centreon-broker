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
#include "com/centreon/broker/misc/string.hh"
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

#define TEST_NAME "rtmonitoring_v2_hosts"
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
  return ;
}

/**
 *  Postcheck routine.
 */
static void postcheck(
              test::centengine& engine,
              test::time_points& tpoints,
              test::db& db,
              test::predicate expected[][80]) {
  static std::string check_query(
    "SELECT host_id, name, instance_id, acknowledged,"
    "       acknowledgement_type, action_url, active_checks, address,"
    "       alias, check_attempt, check_command, check_freshness,"
    "       check_interval, check_period, check_type, checked,"
    "       command_line, default_active_checks,"
    "       default_event_handler_enabled, default_failure_prediction,"
    "       default_flap_detection, default_notify,"
    "       default_passive_checks, enabled, event_handler,"
    "       event_handler_enabled, execution_time, failure_prediction,"
    "       first_notification_delay, flap_detection,"
    "       flap_detection_on_down, flap_detection_on_unreachable,"
    "       flap_detection_on_up, flapping, freshness_threshold,"
    "       high_flap_threshold, icon_image, icon_image_alt,"
    "       last_check, last_hard_state, last_hard_state_change,"
    "       last_notification, last_state_change, last_time_down,"
    "       last_time_unreachable, last_time_up, last_update, latency,"
    "       low_flap_threshold, max_check_attempts, next_check,"
    "       next_host_notification, no_more_notifications, notes,"
    "       notes_url, notification_interval, notification_number,"
    "       notification_period, notify, notify_on_down,"
    "       notify_on_downtime, notify_on_flapping, notify_on_recovery,"
    "       notify_on_unreachable, obsess_over_host, output,"
    "       passive_checks, percent_state_change, perfdata, real_state,"
    "       retain_nonstatus_information, retain_status_information,"
    "       retry_interval, scheduled_downtime_depth,"
    "       should_be_scheduled, stalk_on_down, stalk_on_unreachable,"
    "       stalk_on_up, state, state_type, statusmap_image"
    "  FROM hosts"
    "  ORDER BY host_id ASC");
  test::sleep_for(3);
  tpoints.store();
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return ;
}

/**
 *  Check that the hosts table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = { "instances", "hosts", NULL };
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5572");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set(
      "SQL_ADDITIONAL",
      "<write_filters>"
      "  <category>neb:instance</category>"
      "  <category>neb:instance_status</category>"
      "  <category>neb:host</category>"
      "  <category>neb:host_status</category>"
      "</write_filters>");
    test::cbd broker;
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    test::sleep_for(1);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/tcp.xml.in");
    cbmod_cfg.set("BROKER_ID", "83");
    cbmod_cfg.set("BROKER_NAME", TEST_NAME "-cbmod");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("TCP_HOST", "localhost");
    cbmod_cfg.set("TCP_PORT", "5572");
    test::centengine_config engine_config;
    engine_config.generate_hosts(3);
    engine_config.generate_services(5);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entries.
    precheck(tpoints, "default");
    engine.start();
    test::sleep_for(1);
    test::predicate expected[][80] = {
      { 1u, "1", 42, false,
        0, "", true, "",
        "", 0, "", false,
        5, "", 0, false,
        "", true,
        false, false,
        false, false,
        false, true, "",
        false, 0.0, false,
        0, false,
        false, false,
        false, false, false,
        0.0, "", "",
        test::predicate(test::predicate::type_null), 0, test::predicate(test::predicate::type_null),
        test::predicate(test::predicate::type_null), test::predicate(test::predicate::type_null), test::predicate(test::predicate::type_null),
        test::predicate(test::predicate::type_null), test::predicate(test::predicate::type_null), test::predicate(test::predicate::type_null), 0.0,
        0.0, 3, test::predicate(test::predicate::type_null),
        test::predicate(test::predicate::type_null), false, "",
        "", 5, 0,
        "", true, false,
        false, false, false,
        false, false, "",
        true, 0.0, "", test::predicate(test::predicate::type_null),
        true, true,
        5, 0,
        true, false, false,
        false, 0, 1 }, //, ""},
      { test::predicate() },
      { test::predicate() },
      { test::predicate() }
    };
    memcpy(expected[1], expected[0], sizeof(expected[0]));
    memcpy(expected[2], expected[0], sizeof(expected[0]));
    postcheck(engine, tpoints, db, expected);

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
    broker.stop();
  }
  catch (std::exception const& e) {
    std::cout << "  " << e.what() << "\n";
  }
  catch (...) {
    std::cout << "  unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
