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
#include <iomanip>
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

#define TEST_NAME "rtmonitoring_v2_comments"
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
  /*
  ** entry_type
  **   user            = 1
  **   downtime        = 2
  **   flapping        = 3
  **   acknowledgement = 4
  **
  ** source
  **   internal = 0
  **   external = 1
  **
  ** type
  **   host = 1
  **   service = 2
  */
  static std::string check_query(
      "SELECT host_id, service_id, entry_time, author, data,"
      "       deletion_time, entry_type, expire_time, expires,"
      "       instance_id, internal_id, persistent, source, type"
      "  FROM comments"
      "  ORDER BY host_id ASC,"
      "    COALESCE(service_id, 0) ASC,"
      "    entry_time ASC");
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the comments table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "services", "comments", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5583");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:service</category>"
                "  <category>neb:comment</category>"
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
    cbmod_cfg.set("TCP_PORT", "5583");
    test::centengine_config engine_config;
    engine_config.generate_hosts(2);
    engine_config.generate_services(2);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entry.
    precheck(tpoints,
             "host_id, service_id, entry_time, author, data, instance_id,"
             " internal_id");
    engine.start();
    test::sleep_for(1);
    tpoints.store();
    engine.extcmd().execute("ADD_SVC_COMMENT;1;1;0;Merethis;This Is A Comment");
    test::sleep_for(2);
    tpoints.store();
    test::predicate expected[10][14] = {
        {1, 1, test::predicate(tpoints.prelast(), tpoints.last() + 1),
         "Merethis", "This Is A Comment",
         test::predicate(test::predicate::type_null), 1,
         test::predicate(test::predicate::type_null), false, 42u, 1, false, 1,
         2},
        {test::predicate()}};
    postcheck(db, expected);

    // Check type.
    precheck(tpoints, "type");
    engine.extcmd().execute("ADD_HOST_COMMENT;2;0;admin;My Comment !");
    test::sleep_for(2);
    tpoints.store();
    expected[1][0] = 2;
    expected[1][1] = test::predicate(test::predicate::type_null);
    expected[1][2] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[1][3] = "admin";
    expected[1][4] = "My Comment !";
    expected[1][5] = test::predicate(test::predicate::type_null);
    expected[1][6] = 1;
    expected[1][7] = test::predicate(test::predicate::type_null);
    expected[1][8] = false;
    expected[1][9] = 42u;
    expected[1][10] = 2;
    expected[1][11] = false;
    expected[1][12] = 1;
    expected[1][13] = 1;
    expected[2][0] = test::predicate();
    postcheck(db, expected);

    // Check persistent.
    precheck(tpoints, "persistent");
    engine.extcmd().execute(
        "ADD_SVC_COMMENT;2;3;1;Super Happy User;This product is off the hook "
        "!");
    test::sleep_for(2);
    tpoints.store();
    expected[2][0] = 2;
    expected[2][1] = 3;
    expected[2][2] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[2][3] = "Super Happy User";
    expected[2][4] = "This product is off the hook !";
    expected[2][5] = test::predicate(test::predicate::type_null);
    expected[2][6] = 1;
    expected[2][7] = test::predicate(test::predicate::type_null);
    expected[2][8] = false;
    expected[2][9] = 42u;
    expected[2][10] = 3;
    expected[2][11] = true;
    expected[2][12] = 1;
    expected[2][13] = 2;
    expected[3][0] = test::predicate();
    postcheck(db, expected);

    // Check entry_type.
    precheck(tpoints, "entry_type, source");
    char dt_cmmnt[200];
    {
      time_t start_time(time(NULL));
      time_t end_time(start_time + 3600);
      std::ostringstream cmd;
      cmd << "SCHEDULE_SVC_DOWNTIME;2;3;" << start_time << ";" << end_time
          << ";1;0;3600;root;foobar comment";
      engine.extcmd().execute(cmd.str());
      test::sleep_for(4);
      tpoints.store();
      struct tm start_tm;
      localtime_r(&start_time, &start_tm);
      struct tm end_tm;
      localtime_r(&end_time, &end_tm);
      sprintf(dt_cmmnt,
              "This service has been scheduled for fixed downtime from "
              "%02d-%02d-%04d %02d:%02d:%02d to %02d-%02d-%04d %02d:%02d:%02d "
              "Notifications for the service will not be sent out during that "
              "time period.",
              start_tm.tm_mon + 1, start_tm.tm_mday, start_tm.tm_year + 1900,
              start_tm.tm_hour, start_tm.tm_min, start_tm.tm_sec,
              end_tm.tm_mon + 1, end_tm.tm_mday, end_tm.tm_year + 1900,
              end_tm.tm_hour, end_tm.tm_min, end_tm.tm_sec);
    }
    expected[3][0] = 2;
    expected[3][1] = 3;
    expected[3][2] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[3][3] = "(Centreon Engine Process)";
    expected[3][4] = dt_cmmnt;
    expected[3][5] = test::predicate(test::predicate::type_null);
    expected[3][6] = 2;
    expected[3][7] = test::predicate(test::predicate::type_null);
    expected[3][8] = false;
    expected[3][9] = 42u;
    expected[3][10] = 4;
    expected[3][11] = false;
    expected[3][12] = 0;
    expected[3][13] = 2;
    expected[4][0] = test::predicate();
    postcheck(db, expected);

    // Check expire_time.
    precheck(tpoints, "expire_time");
    std::cout << "  not tested\n";

    // Check expires.
    precheck(tpoints, "expires");
    std::cout << "  not tested\n";

    // Check deletion_time.
    precheck(tpoints, "deletion_time");
    engine.stop();
    test::sleep_for(2);
    test::predicate del_time(tpoints.prelast(), tpoints.last() + 1);
    expected[0][5] = del_time;
    expected[1][5] = del_time;
    expected[3][5] = del_time;
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
