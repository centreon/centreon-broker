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

#define TEST_NAME "rtmonitoring_v2_acknowledgements"
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
static void postcheck(test::db& db, test::predicate expected[][12]) {
  static std::string check_query(
      "SELECT entry_time, host_id, service_id, author, comment_data,"
      "       deletion_time, instance_id, notify_contacts,"
      "       persistent_comment, state, sticky, type"
      "  FROM acknowledgements"
      "  ORDER BY entry_time ASC, host_id ASC, service_id ASC");
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
    char const* tables[] = {"instances", "hosts", "services",
                            "acknowledgements", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5581");
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
                "  <category>neb:acknowledgement</category>"
                "</write_filters>");
    test::cbd broker;
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    test::sleep_for(2);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/tcp.xml.in");
    cbmod_cfg.set("BROKER_ID", "83");
    cbmod_cfg.set("BROKER_NAME", TEST_NAME "-cbmod");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("TCP_HOST", "localhost");
    cbmod_cfg.set("TCP_PORT", "5581");
    test::centengine_config engine_config;
    engine_config.set_directive("accept_passive_host_checks", "1");
    engine_config.set_directive("accept_passive_service_checks", "1");
    engine_config.set_directive("execute_host_checks", "0");
    engine_config.set_directive("execute_service_checks", "0");
    engine_config.set_directive("log_host_retries", "1");
    engine_config.set_directive("log_service_retries", "1");
    engine_config.generate_hosts(5);
    engine_config.generate_services(1);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_hosts().begin()),
         end(engine_config.get_hosts().end());
         it != end; ++it) {
      it->set("max_check_attempts", "1");
      it->set("passive_checks_enabled", "1");
    }
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_services().begin()),
         end(engine_config.get_services().end());
         it != end; ++it) {
      it->set("max_check_attempts", "1");
      it->set("passive_checks_enabled", "1");
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // #1, default.
    precheck(tpoints,
             "entry_time, host_id, service_id, author, comment_data,"
             " instance_id, state, type");
    engine.start();
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute("PROCESS_HOST_CHECK_RESULT;1;1;myoutput");
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute(
        "ACKNOWLEDGE_HOST_PROBLEM;1;0;0;0;mkermagoret;myowncomment");
    test::sleep_for(2);
    tpoints.store();
    test::predicate expected[5][12] = {
        {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1,
         test::predicate(test::predicate::type_null), "mkermagoret",
         "myowncomment", test::predicate(test::predicate::type_null), 42u,
         false, false, 1, false, 0},
        {test::predicate()}};
    postcheck(db, expected);

    // #2.
    precheck(tpoints, "sticky");
    engine.extcmd().execute("PROCESS_HOST_CHECK_RESULT;3;2;myoutput");
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute(
        "ACKNOWLEDGE_HOST_PROBLEM;3;2;0;0;Merethis;another comment");
    test::sleep_for(2);
    tpoints.store();
    expected[1][0] = test::predicate(tpoints.prelast(), tpoints.last() - 1);
    expected[1][1] = 3;
    expected[1][2] = test::predicate(test::predicate::type_null);
    expected[1][3] = "Merethis";
    expected[1][4] = "another comment";
    expected[1][5] = test::predicate(test::predicate::type_null);
    expected[1][6] = 42u;
    expected[1][7] = false;
    expected[1][8] = false;
    expected[1][9] = 2;
    expected[1][10] = true;
    expected[1][11] = 0;
    expected[2][0] = test::predicate();
    postcheck(db, expected);

    // #3.
    precheck(tpoints, "notify_contacts");
    engine.extcmd().execute("PROCESS_SERVICE_CHECK_RESULT;2;2;2;myoutput");
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute(
        "ACKNOWLEDGE_SVC_PROBLEM;2;2;0;1;0;Centreon;default");
    test::sleep_for(2);
    tpoints.store();
    expected[2][0] = test::predicate(tpoints.prelast(), tpoints.last() - 1);
    expected[2][1] = 2u;
    expected[2][2] = 2u;
    expected[2][3] = "Centreon";
    expected[2][4] = "default";
    expected[2][5] = test::predicate(test::predicate::type_null);
    expected[2][6] = 42u;
    expected[2][7] = true;
    expected[2][8] = false;
    expected[2][9] = 2;
    expected[2][10] = false;
    expected[2][11] = 1;
    expected[3][0] = test::predicate();
    postcheck(db, expected);

    // #4.
    precheck(tpoints, "persistent_comment");
    engine.extcmd().execute("PROCESS_SERVICE_CHECK_RESULT;5;5;1;myoutput");
    test::sleep_for(2);
    tpoints.store();
    engine.extcmd().execute(
        "ACKNOWLEDGE_SVC_PROBLEM;5;5;0;0;1;admin;admin is admin");
    test::sleep_for(2);
    tpoints.store();
    expected[3][0] = test::predicate(tpoints.prelast(), tpoints.last() - 1);
    expected[3][1] = 5u;
    expected[3][2] = 5u;
    expected[3][3] = "admin";
    expected[3][4] = "admin is admin";
    expected[3][5] = test::predicate(test::predicate::type_null);
    expected[3][6] = 42u;
    expected[3][7] = false;
    expected[3][8] = true;
    expected[3][9] = 1;
    expected[3][10] = false;
    expected[3][11] = 1;
    expected[4][0] = test::predicate();
    postcheck(db, expected);

    // #5.
    precheck(tpoints, "deletion_time");
    engine.extcmd().execute("REMOVE_HOST_ACKNOWLEDGEMENT;1");
    engine.extcmd().execute("REMOVE_SVC_ACKNOWLEDGEMENT;5;5");
    test::sleep_for(2);
    tpoints.store();
    expected[0][5] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[3][5] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    postcheck(db, expected);

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
