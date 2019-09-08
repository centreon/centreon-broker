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

#define TEST_NAME "rtmonitoring_v2_customvariables"
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
static void postcheck(test::db& db, test::predicate expected[][8]) {
  static std::string check_query(
      "SELECT host_id, service_id, name, default_value, modified, type,"
      "       update_time, value"
      "  FROM customvariables"
      "  ORDER BY host_id ASC, COALESCE(service_id, 0) ASC, name ASC");
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the custom variables table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "services", "customvariables",
                            NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5582");
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
                "  <category>neb:custom_variable</category>"
                "  <category>neb:custom_variable_status</category>"
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
    cbmod_cfg.set("TCP_PORT", "5582");
    test::centengine_config engine_config;
    engine_config.generate_hosts(2);
    engine_config.generate_services(1);
    engine_config.get_hosts()[1].set("_MYVAR", "42");
    engine_config.get_hosts()[2].set("_MYVAR", "3612");
    engine_config.get_services()[1].set("_MYSVC1VAR", "isfantastic");
    engine_config.get_services()[2].set("_MYSVC2VAR", "foobar");
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entry.
    precheck(tpoints, "customvariables.insert");
    engine.start();
    test::sleep_for(1);
    tpoints.store();
    test::predicate expected[][8] = {
        {1, test::predicate(test::predicate::type_null), "MYVAR", "42", false,
         0, test::predicate(tpoints.prelast(), tpoints.last() + 1), "42"},
        {1, 1, "MYSVC1VAR", "isfantastic", false, 1,
         test::predicate(tpoints.prelast(), tpoints.last() + 1), "isfantastic"},
        {2, test::predicate(test::predicate::type_null), "MYVAR", "3612", false,
         0, test::predicate(tpoints.prelast(), tpoints.last() + 1), "3612"},
        {2, 2, "MYSVC2VAR", "foobar", false, 1,
         test::predicate(tpoints.prelast(), tpoints.last() + 1), "foobar"},
        {test::predicate()}};
    postcheck(db, expected);

    // Check default_value, modified, update_time, value.
    precheck(tpoints, "customvariables.update");
    engine.extcmd().execute("CHANGE_CUSTOM_HOST_VAR;1;MYVAR;a new value");
    engine.extcmd().execute(
        "CHANGE_CUSTOM_SVC_VAR;2;2;MYSVC2VAR;Brand New Value");
    test::sleep_for(2);
    tpoints.store();
    expected[0][4] = true;
    expected[0][6] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[0][7] = "a new value";
    expected[3][4] = true;
    expected[3][6] = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    expected[3][7] = "Brand New Value";
    postcheck(db, expected);

    // Check deletion #1.
    precheck(tpoints, "customvariables.delete #1");
    engine_config.get_services()[2].set("_MYSVC2VAR", "");
    engine.reload();
    test::sleep_for(3);
    tpoints.store();
    expected[3][0] = test::predicate();
    postcheck(db, expected);

    // Check deletion #2.
    precheck(tpoints, "customvariables.delete #2");
    engine.stop();
    test::sleep_for(2);
    tpoints.store();
    expected[0][0] = test::predicate();
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
