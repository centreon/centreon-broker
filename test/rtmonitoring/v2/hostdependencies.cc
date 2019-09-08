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

#define TEST_NAME "rtmonitoring_v2_hostdependencies"
#define DB_NAME "broker_" TEST_NAME

// Check count.
static int check_number(0);

/**
 *  Precheck routine.
 */
static void precheck(char const* name) {
  ++check_number;
  std::cout << "check #" << check_number << " (" << name << ")\n";
  return;
}

/**
 *  Postcheck routine.
 */
static void postcheck(test::db& db, test::predicate expected[][6]) {
  static std::string check_query(
      "SELECT dependent_host_id, host_id, dependency_period,"
      "       execution_failure_options, inherits_parent,"
      "       notification_failure_options"
      "  FROM hosts_hosts_dependencies"
      "  ORDER BY dependent_host_id ASC, host_id ASC,"
      "           execution_failure_options ASC");
  db.check_content(check_query, expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the host dependencies table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "hosts_hosts_dependencies",
                            NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5578");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:host_check</category>"
                "  <category>neb:host_status</category>"
                "  <category>neb:host_dependency</category>"
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
    cbmod_cfg.set("TCP_PORT", "5578");
    test::centengine_config engine_config;
    engine_config.generate_hosts(5);
    engine_config.generate_services(2);
    engine_config.host_depends_on("1", "2");
    engine_config.host_depends_on("4", "5");
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Check default entry.
    precheck("dependent_host_id, host_id");
    engine.start();
    test::sleep_for(2);
    test::predicate expected[][6] = {
        {1, 2, "default_timeperiod", "", false, "dopu"},
        {1, 2, "default_timeperiod", "dopu", false, ""},
        {4, 5, "default_timeperiod", "", false, "dopu"},
        {4, 5, "default_timeperiod", "dopu", false, ""},
        {test::predicate()}};
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
