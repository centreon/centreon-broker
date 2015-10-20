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
#include <ctime>
#include <iostream>
#include "test/db.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/predicate.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "rtmonitoring_instances_v2"
#define DB_NAME "broker_" TEST_NAME

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
    char const* tables[] = { RTMONITORING_TABLES_V2, NULL };
    test::db db(DB_NAME, tables);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbmod_cfg.set("BROKER_ID", "84");
    cbmod_cfg.set("BROKER_NAME", "my-broker");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("DB_NAME", DB_NAME);
    cbmod_cfg.set(
      "SQL_ADDITIONAL",
      "<write_filters>"
      "  <category>neb:instance</category>"
      "  <category>neb:instance_status</category>"
      "</write_filters>");
    test::centengine_config engine_config;
    engine_config.generate_hosts(3);
    engine_config.generate_services(5);
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(engine_config);
    time_t t0(time(NULL));
    engine.start();
    test::sleep_for(8);

    // #1 Check entry insertion.
    time_t t1(time(NULL));
    {
      std::string query(
        "SELECT instance_id, name, active_host_checks,"
        "       active_service_checks, check_hosts_freshness,"
        "       check_services_freshness, deleted, end_time, engine,"
        "       event_handlers, failure_prediction, flap_detection,"
        "       global_host_event_handler,"
        "       global_service_event_handler, last_alive,"
        "       last_command_check, notifications, obsess_over_hosts,"
        "       obsess_over_services, outdated, passive_host_checks,"
        "       passive_service_checks, pid, running, start_time"
        // XXX : not checked yet       "       version"
        "  FROM instances");
      test::predicate expected[][25] = {
        { 42u, "my-poller", true, true, false, false, false,
          test::predicate(test::predicate::type_null), "Centreon Engine",
          false, false, false, "", "", test::predicate(t0, t1 + 1),
          test::predicate(t0, t1 + 1), false, false, false, false, true,
          true, true,// PID will be converted to boolean, any non-0 will match.
          true, test::predicate(t0, t1 + 1) },
        { test::predicate() }
      };
      db.check_content(query, expected);
    }
    // XXX

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  catch (...) {
    std::cerr << "unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
