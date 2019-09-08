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

#define TEST_NAME "rtmonitoring_v2_services"
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
 *
 *  This routine applies mostly to status properties.
 */
static void postcheck(test::time_points& tpoints,
                      test::db& db,
                      test::predicate expected[][81]) {
  static std::string check_query(
      "SELECT host_id, service_id, description, acknowledged,"
      "       acknowledgement_type, action_url, active_checks,"
      "       check_attempt, check_command, check_freshness,"
      "       check_interval, check_period, check_type, checked,"
      "       command_line, default_active_checks,"
      "       default_event_handler_enabled,"
      "       default_flap_detection, default_notify,"
      "       default_passive_checks, enabled, event_handler,"
      "       event_handler_enabled, execution_time,"
      "       first_notification_delay,"
      "       flap_detection, flap_detection_on_critical,"
      "       flap_detection_on_ok, flap_detection_on_unknown,"
      "       flap_detection_on_warning, flapping, freshness_threshold,"
      "       high_flap_threshold, icon_image, icon_image_alt,"
      "       last_check, last_hard_state, last_hard_state_change,"
      "       last_notification, last_state_change, last_time_critical,"
      "       last_time_ok, last_time_unknown, last_time_warning,"
      "       last_update, latency, low_flap_threshold,"
      "       max_check_attempts, next_check,"
      "       next_notification, no_more_notifications, notes, notes_url,"
      "       notification_interval, notification_number,"
      "       notification_period, notify, notify_on_critical,"
      "       notify_on_downtime, notify_on_flapping, notify_on_recovery,"
      "       notify_on_unknown, notify_on_warning, obsess_over_service,"
      "       output, passive_checks, percent_state_change, perfdata,"
      "       real_state, retain_nonstatus_information,"
      "       retain_status_information, retry_interval,"
      "       scheduled_downtime_depth, should_be_scheduled,"
      "       stalk_on_critical, stalk_on_ok, stalk_on_unknown,"
      "       stalk_on_warning, state, state_type, volatile"
      "  FROM services"
      "  ORDER BY host_id ASC, service_id ASC");
  expected[0][44] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Postcheck routine.
 *
 *  This routine applies mostly to configuration properties as the
 *  configuration file gets regenerated and the monitoring engine is
 *  reloaded.
 */
static void postcheck(test::centengine& engine,
                      test::time_points& tpoints,
                      test::db& db,
                      test::predicate expected[][81]) {
  engine.reload();
  test::sleep_for(3);
  tpoints.store();
  postcheck(tpoints, db, expected);
  return;
}

/**
 *  Check that the services table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "services", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5575");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:host_check</category>"
                "  <category>neb:host_status</category>"
                "  <category>neb:service</category>"
                "  <category>neb:service_check</category>"
                "  <category>neb:service_status</category>"
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
    cbmod_cfg.set("TCP_PORT", "5575");
    test::centengine_config engine_config;
    engine_config.generate_hosts(5);
    engine_config.generate_services(1);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    {
      for (test::centengine_config::objlist::iterator
               it(engine_config.get_hosts().begin()),
           end(engine_config.get_hosts().end());
           it != end; ++it) {
        it->set("active_checks_enabled", "0");
      }
    }
    {
      int i(0);
      for (test::centengine_config::objlist::iterator
               it(engine_config.get_services().begin()),
           end(engine_config.get_services().end());
           it != end; ++it, ++i) {
        it->set("active_checks_enabled", "0");
        it->set("check_command", "default_command");
        it->set("check_freshness", "0");
        it->set("check_interval", "5");
        it->set("check_period", " ");
        it->set("event_handler_enabled", "0");
        it->set("first_notification_delay", "0");
        it->set("flap_detection_enabled", "0");
        it->set("flap_detection_options", "n");
        it->set("max_check_attempts", "5");
        it->set("notifications_enabled", "0");
        it->set("notification_interval", "5");
        it->set("notification_options", "n");
        it->set("notification_period", " ");
        it->set("obsess_over_service", "0");
        it->set("passive_checks_enabled", "0");
        it->set("retain_nonstatus_information", "0");
        it->set("retain_status_information", "0");
        it->set("retry_interval", "1");
        it->set("stalking_options", "n");
      }
    }
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entries.
    precheck(tpoints, "enabled, last_update");
    engine.start();
    test::sleep_for(1);
    test::predicate expected[][81] = {
        {1,
         1,
         "1",
         false,
         0,
         "",
         false,
         1,
         "default_command",
         false,
         5.0,
         false,
         0,
         false,
         test::predicate(test::predicate::type_null),
         false,
         false,
         false,
         false,
         false,
         true,
         "",
         false,
         0.0,
         0.0,
         false,
         false,
         false,
         false,
         false,
         false,
         0.0,
         0.0,
         "",
         "",
         test::predicate(test::predicate::type_null),
         0,
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         test::predicate(tpoints.last(), tpoints.last() + 2),
         0.0,
         0.0,
         5,
         test::predicate(test::predicate::type_null),
         test::predicate(test::predicate::type_null),
         false,
         "",
         "",
         5,
         0,
         "",
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         "",
         false,
         test::predicate(0.0, 100.0),
         "",
         test::predicate(test::predicate::type_null),
         false,
         false,
         1.0,
         0,
         false,
         false,
         false,
         false,
         false,
         4,
         1,
         false},
        {test::predicate()},
        {test::predicate()},
        {test::predicate()},
        {test::predicate()},
        {test::predicate()}};
    std::string ids_str[5];
    ids_str[0] = "1";
    for (int i(1); i < 5; ++i) {
      memcpy(expected[i], expected[0], sizeof(expected[0]));
      std::ostringstream oss;
      oss << i + 1;
      ids_str[i] = oss.str();
      expected[i][0] = i + 1;
      expected[i][1] = i + 1;
      expected[i][2] = ids_str[i].c_str();
    }
    postcheck(engine, tpoints, db, expected);

    // Tested service.
    test::centengine_object& s(*++engine_config.get_services().begin());

    // Check service_description.
    precheck(tpoints, "service_description");
    s.set("service_description", "renamed");
    expected[0][2] = "renamed";
    postcheck(engine, tpoints, db, expected);

    // Check action_url.
    precheck(tpoints, "action_url");
    s.set("action_url", "http://www.centreon.com");
    expected[0][5] = "http://www.centreon.com";
    postcheck(engine, tpoints, db, expected);

    // Check check_command.
    precheck(tpoints, "check_command");
    {
      test::centengine_object cmd(test::centengine_object::command_type);
      cmd.set("command_name", "test_command");
      cmd.set("command_line", MY_PLUGIN_PATH " $ARG1$ $ARG2$");
      engine_config.get_commands().push_back(cmd);
    }
    s.set("check_command", "test_command!0!");
    expected[0][8] = "test_command!0!";
    postcheck(engine, tpoints, db, expected);

    // Check check_freshness.
    precheck(tpoints, "check_freshness #1");
    s.set("check_freshness", "1");
    expected[0][9] = true;
    postcheck(engine, tpoints, db, expected);
    precheck(tpoints, "check_freshness #2");
    s.set("check_freshness", "0");
    expected[0][9] = false;
    postcheck(engine, tpoints, db, expected);

    // Check check_interval.
    precheck(tpoints, "check_interval");
    s.set("check_interval", "2");
    expected[0][10] = 2;
    postcheck(engine, tpoints, db, expected);

    // Check check_period.
    precheck(tpoints, "check_period #1");
    {
      test::centengine_object tp(test::centengine_object::timeperiod_type);
      tp.set("timeperiod_name", "test_timeperiod");
      tp.set("alias", "test_timeperiod");
      tp.set("monday", "00:00-24:00");
      engine_config.get_timeperiods().push_back(tp);
    }
    s.set("check_period", "test_timeperiod");
    expected[0][11] = "test_timeperiod";
    postcheck(engine, tpoints, db, expected);
    precheck(tpoints, "check_period #2");
    s.set("check_period", "default_timeperiod");
    expected[0][11] = "default_timeperiod";
    postcheck(engine, tpoints, db, expected);

    // Check event_handler.
    precheck(tpoints, "event_handler");
    s.set("event_handler", "default_command");
    expected[0][21] = "default_command";
    postcheck(engine, tpoints, db, expected);

    // Check event_handler_enabled.
    precheck(tpoints,
             "event_handler_enabled, default_event_handler_enabled #1");
    s.set("event_handler_enabled", "1");
    expected[0][16] = true;
    expected[0][22] = true;
    postcheck(engine, tpoints, db, expected);
    precheck(tpoints,
             "event_handler_enabled, default_event_handler_enabled #2");
    s.set("event_handler_enabled", "0");
    expected[0][16] = false;
    expected[0][22] = false;
    postcheck(engine, tpoints, db, expected);

    // Check first_notification_delay.
    precheck(tpoints, "first_notification_delay #1");
    s.set("first_notification_delay", "42");
    expected[0][24] = 42.0;
    postcheck(engine, tpoints, db, expected);
    precheck(tpoints, "first_notification_delay #2");
    s.set("first_notification_delay", "0");
    expected[0][24] = 0.0;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection.
    precheck(tpoints, "flap_detection, default_flap_detection");
    s.set("flap_detection_enabled", "1");
    expected[0][17] = true;
    expected[0][25] = true;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection_on_critical.
    precheck(tpoints, "flap_detection_on_critical");
    s.set("flap_detection_options", "c");
    expected[0][26] = true;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection_on_ok.
    precheck(tpoints, "flap_detection_on_ok");
    s.set("flap_detection_options", "c,o");
    expected[0][27] = true;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection_on_unknown.
    precheck(tpoints, "flap_detection_on_unknown");
    s.set("flap_detection_options", "c,o,u");
    expected[0][28] = true;
    postcheck(engine, tpoints, db, expected);

    // Check flap_detection_on_warning.
    precheck(tpoints, "flap_detection_on_warning");
    s.set("flap_detection_options", "a");
    expected[0][29] = true;
    postcheck(engine, tpoints, db, expected);

    // Check freshness threshold.
    precheck(tpoints, "freshness_threshold");
    s.set("freshness_threshold", "20");
    expected[0][31] = 20.0;
    postcheck(engine, tpoints, db, expected);

    // Check high_flap_threshold.
    precheck(tpoints, "high_flap_threshold");
    s.set("high_flap_threshold", "30");
    expected[0][32] = 30.0;
    postcheck(engine, tpoints, db, expected);

    // Check icon_image.
    precheck(tpoints, "icon_image");
    s.set("icon_image", "my_icon_image");
    expected[0][33] = "my_icon_image";
    postcheck(engine, tpoints, db, expected);

    // Check icon_image_alt.
    precheck(tpoints, "icon_image_alt");
    s.set("icon_image_alt", "my_alternative_icon_image");
    expected[0][34] = "my_alternative_icon_image";
    postcheck(engine, tpoints, db, expected);

    // Check low_flap_threshold.
    precheck(tpoints, "low_flap_threshold");
    s.set("low_flap_threshold", "10");
    expected[0][46] = 10.0;
    postcheck(engine, tpoints, db, expected);

    // Check max_check_attempts.
    precheck(tpoints, "max_check_attempts");
    s.set("max_check_attempts", "2");
    expected[0][47] = 2;
    postcheck(engine, tpoints, db, expected);

    // Check notes.
    precheck(tpoints, "notes");
    s.set("notes", "my personal notes");
    expected[0][51] = "my personal notes";
    postcheck(engine, tpoints, db, expected);

    // Check notes_url.
    precheck(tpoints, "notes_url");
    s.set("notes_url", "http://www.centreon.com/");
    expected[0][52] = "http://www.centreon.com/";
    postcheck(engine, tpoints, db, expected);

    // Check notification_interval.
    precheck(tpoints, "notification_interval");
    s.set("notification_interval", "2");
    expected[0][53] = 2.0;
    postcheck(engine, tpoints, db, expected);

    // Check notification_period.
    precheck(tpoints, "notification_period");
    s.set("notification_period", "default_timeperiod");
    expected[0][55] = "default_timeperiod";
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_critical.
    precheck(tpoints, "notify_on_critical");
    s.set("notification_options", "c");
    expected[0][57] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_downtime.
    precheck(tpoints, "notify_on_downtime");
    s.set("notification_options", "c,s");
    expected[0][58] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_flapping.
    precheck(tpoints, "notify_on_flapping");
    s.set("notification_options", "c,s,f");
    expected[0][59] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_recovery.
    precheck(tpoints, "notify_on_recovery");
    s.set("notification_options", "c,s,f,r");
    expected[0][60] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_unknown.
    precheck(tpoints, "notify_on_unknown");
    s.set("notification_options", "c,s,f,r,u");
    expected[0][61] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify_on_warning.
    precheck(tpoints, "notify_on_warning");
    s.set("notification_options", "a");
    expected[0][62] = true;
    postcheck(engine, tpoints, db, expected);

    // Check notify, default_notify.
    precheck(tpoints, "notify, default_notify");
    s.set("notifications_enabled", "1");
    expected[0][18] = true;
    expected[0][56] = true;
    postcheck(engine, tpoints, db, expected);

    // Check obsess_over_service.
    precheck(tpoints, "obsess_over_service");
    s.set("obsess_over_service", "1");
    expected[0][63] = true;
    postcheck(engine, tpoints, db, expected);

    // Check passive_checks, default_passive_checks.
    precheck(tpoints, "passive_checks, default_passive_checks");
    s.set("passive_checks_enabled", "1");
    expected[0][19] = true;
    expected[0][65] = true;
    postcheck(engine, tpoints, db, expected);

    // Check retain_nonstatus_information.
    precheck(tpoints, "retain_nonstatus_information");
    s.set("retain_nonstatus_information", "1");
    expected[0][69] = true;
    postcheck(engine, tpoints, db, expected);

    // Check retain_status_information.
    precheck(tpoints, "retain_status_information");
    s.set("retain_status_information", "1");
    expected[0][70] = true;
    postcheck(engine, tpoints, db, expected);

    // Check retry_interval.
    precheck(tpoints, "retry_interval");
    s.set("retry_interval", "2");
    expected[0][71] = 2;
    postcheck(engine, tpoints, db, expected);

    // Check stalk_on_critical.
    precheck(tpoints, "stalk_on_critical");
    s.set("stalking_options", "c");
    expected[0][74] = true;
    postcheck(engine, tpoints, db, expected);

    // Check stalk_on_ok.
    precheck(tpoints, "stalk_on_ok");
    s.set("stalking_options", "c,o");
    expected[0][75] = true;
    postcheck(engine, tpoints, db, expected);

    // Check stalk_on_unknown.
    precheck(tpoints, "stalk_on_unknown");
    s.set("stalking_options", "c,o,u");
    expected[0][76] = true;
    postcheck(engine, tpoints, db, expected);

    // Check stalk_on_warning.
    precheck(tpoints, "stalk_on_warning");
    s.set("stalking_options", "a");
    expected[0][77] = true;
    postcheck(engine, tpoints, db, expected);

    // Check volatile.
    precheck(tpoints, "volatile");
    s.set("is_volatile", "1");
    expected[0][80] = true;
    postcheck(engine, tpoints, db, expected);

    // Check active_checks, default_active_checks, should_be_scheduled,
    // checked, command_line, execution_time,
    // last_check, last_time_ok, latency, next_check, output,
    // perfdata, state, state_type.
    precheck(tpoints,
             "active_checks, default_active_checks, should_be_scheduled, "
             "checked, command_line, execution_time, "
             "last_check, last_time_ok, latency, next_check, output, "
             "perfdata, state, state_type");
    s.set("active_checks_enabled", "1");
    s.set("check_command", "test_command!0!mypluginoutput|metric=42v");
    expected[0][6] = true;  // active_checks
    expected[0][8] =
        "test_command!0!mypluginoutput|metric=42v";  // check_command
    expected[0][13] = true;                          // checked
    expected[0][14] =
        MY_PLUGIN_PATH " 0 mypluginoutput|metric=42v";  // command_line
    expected[0][15] = true;                             // default_active_checks
    expected[0][23] = test::predicate(0.0, 1.0);        // execution_time
    expected[0][45] = test::predicate(0.0, 1.0);        // latency
    expected[0][64] = "mypluginoutput\n";               // output
    expected[0][67] = "metric=42v";                     // perfdata
    expected[0][73] = true;                             // should_be_scheduled
    expected[0][78] = 0;                                // state, OK
    expected[0][79] = 1;                                // state_type, HARD
    engine.reload();
    test::sleep_for(3);
    tpoints.store();
    expected[0][35] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_check
    expected[0][37] = test::predicate(
        tpoints.prelast(), tpoints.last() + 1);  // last_hard_state_change
    expected[0][39] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_state_change
    expected[0][41] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_time_ok
    expected[0][48] =
        test::predicate(tpoints.last() - 1,
                        tpoints.last() + 2 * MONITORING_ENGINE_INTERVAL_LENGTH +
                            1);  // next_check
    postcheck(tpoints, db, expected);

    // Check check_type, last_state_change, last_time_warning.
    precheck(tpoints, "check_type, last_state_change, last_time_warning");
    s.set("active_checks_enabled", "0");
    expected[0][6] = false;   // active_checks
    expected[0][15] = false;  // default_active_checks
    expected[0][73] = false;  // should_be_scheduled
    engine.reload();
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;renamed;1;mypluginoutput|metric=42v");
    expected[0][12] = 1;  // check_type, PASSIVE
    expected[0][35] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_check
    expected[0][39] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_state_change
    expected[0][43] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_time_warning
    expected[0][48] = test::predicate(
        tpoints.last(), tpoints.last() + 2 * MONITORING_ENGINE_INTERVAL_LENGTH +
                            1);  // next_check
    expected[0][78] = 1;         // state, WARNING
    expected[0][79] = 0;         // state_type, SOFT
    test::sleep_for(2);
    postcheck(tpoints, db, expected);

    // Check check_attempt, last_hard_state, last_hard_state_change.
    precheck(tpoints, "check_attempt, last_hard_state, last_hard_state_change");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;renamed;1;mypluginoutput|metric=42v");
    test::sleep_for(2);
    tpoints.store();
    expected[0][7] = 2;  // check_attempt
    expected[0][35] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_check
    expected[0][36] = 1;  // last_hard_state, WARNING
    expected[0][37] = test::predicate(
        tpoints.prelast(), tpoints.last() + 1);  // last_hard_state_change
    expected[0][43] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_time_warning
    expected[0][48] = test::predicate(
        tpoints.last(), tpoints.last() + 2 * MONITORING_ENGINE_INTERVAL_LENGTH +
                            1);  // next_check
    expected[0][79] = 1;         // state_type, HARD
    postcheck(tpoints, db, expected);

    // Check last_time_critical.
    precheck(tpoints, "last_time_critical");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;renamed;2;mypluginoutput|metric=42v");
    test::sleep_for(2);
    tpoints.store();
    expected[0][35] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_check
    expected[0][36] = 2;  // last_hard_state, CRITICAL
    expected[0][37] = test::predicate(
        tpoints.prelast(), tpoints.last() + 1);  // last_hard_state_change
    expected[0][39] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_state_change
    expected[0][40] = test::predicate(
        tpoints.prelast(), tpoints.last() + 1);  // last_time_critical
    expected[0][48] = test::predicate(
        tpoints.last(), tpoints.last() + 2 * MONITORING_ENGINE_INTERVAL_LENGTH +
                            1);  // next_check
    expected[0][78] = 2;         // state, CRITICAL
    postcheck(tpoints, db, expected);

    // Check last_time_unknown.
    precheck(tpoints, "last_time_unknown");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;renamed;3;mypluginoutput|metric=42v");
    test::sleep_for(2);
    tpoints.store();
    expected[0][35] =
        test::predicate(tpoints.prelast(), tpoints.last() + 1);  // last_check
    expected[0][36] = 3;  // last_hard_state, UNKNOWN
    expected[0][37] = test::predicate(
        tpoints.prelast(), tpoints.last() + 1);  // last_hard_state_change
    expected[0][39] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_state_change
    expected[0][42] = test::predicate(tpoints.prelast(),
                                      tpoints.last() + 1);  // last_time_unknown
    expected[0][48] = test::predicate(
        tpoints.last(), tpoints.last() + 2 * MONITORING_ENGINE_INTERVAL_LENGTH +
                            1);  // next_check
    expected[0][78] = 3;         // state, UNKNOWN
    postcheck(tpoints, db, expected);

    // Check acknowledged.
    precheck(tpoints, "acknowledged");
    engine.extcmd().execute(
        "ACKNOWLEDGE_SVC_PROBLEM;1;renamed;0;0;1;user;comment");
    test::sleep_for(2);
    expected[0][3] = true;
    expected[0][4] = 1;
    postcheck(tpoints, db, expected);

    // Check acknowledgement_type.
    precheck(tpoints, "acknowledgement_type");
    engine.extcmd().execute(
        "ACKNOWLEDGE_SVC_PROBLEM;1;renamed;2;0;1;user;comment");
    test::sleep_for(2);
    expected[0][4] = 2;
    postcheck(tpoints, db, expected);

    // Check scheduled_downtime_depth.
    precheck(tpoints, "scheduled_downtime_depth");
    {
      time_t now(time(NULL));
      std::ostringstream ss1;
      ss1 << "SCHEDULE_SVC_DOWNTIME;1;renamed;" << now << ";" << now + 3600
          << ";1;0;3600;admin;comment";
      engine.extcmd().execute(ss1.str());
      std::ostringstream ss2;
      ss2 << "SCHEDULE_SVC_DOWNTIME;1;renamed;" << now << ";" << now + 7200
          << ";1;0;7200;admin;comment";
      engine.extcmd().execute(ss2.str());
    }
    test::sleep_for(2);
    expected[0][72] = 2;
    postcheck(tpoints, db, expected);

    // Check flapping.
    precheck(tpoints, "flapping");
    std::cout << "  not tested\n";

    // Check percent_state_change.
    precheck(tpoints, "percent_state_change");
    std::cout << "  not tested\n";

    // Check last_notification.
    precheck(tpoints, "last_notification");
    std::cout << "  not tested\n";

    // Check next_notification.
    precheck(tpoints, "next_notification");
    std::cout << "  not tested\n";

    // Check no_more_notifications.
    precheck(tpoints, "no_more_notifications");
    std::cout << "  not tested\n";

    // Check notification_number.
    precheck(tpoints, "notification_number");
    std::cout << "  not tested\n";

    // Check real_state.
    precheck(tpoints, "real_state");
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
