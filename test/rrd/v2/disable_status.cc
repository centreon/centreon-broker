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

#include <sys/stat.h>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "test/cbd.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/db.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/predicate.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "rrd_v2_disable_status"
#define DB_NAME "broker_" TEST_NAME

/**
 *  Check that status graph generation can be disabled.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Temporary directories
  std::string metrics_path;
  std::string status_path;

  try {
    // Database.
    char const* tables[] = {"instances", "index_data", "metrics", "data_bin",
                            NULL};
    test::db db(DB_NAME, tables);
    {
      char const* const queries[] = {
          "INSERT INTO index_data (id, host_id, service_id)"
          "  VALUES (100, 1, NULL),"
          "         (101, 1, 1),"
          "         (102, 1, 2),"
          "         (103, 1, 3),"
          "         (200, 2, NULL),"
          "         (201, 2, 4),"
          "         (202, 2, 5),"
          "         (203, 2, 6)",
          NULL};
      QSqlQuery q(*db.get_db());
      for (int i(0); queries[i]; ++i)
        if (!q.exec(queries[i]))
          throw(exceptions::msg()
                << "cannot populate database: " << q.lastError().text()
                << " (query was " << queries[i] << ")");
    }

    // Temporary paths.
    metrics_path = misc::temp_path();
    mkdir(metrics_path.c_str(), S_IRWXU);
    status_path = misc::temp_path();
    mkdir(status_path.c_str(), S_IRWXU);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/rrd.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5587");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("METRICS_PATH", metrics_path);
    cbd_cfg.set("STATUS_PATH", status_path);
    cbd_cfg.set("RRD_ADDITIONAL", "<write_status>no</write_status>");
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
    cbmod_cfg.set("TCP_PORT", "5587");
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
      it->set("active_checks_enabled", "1");
      it->set("check_command", "test_command!0!output|metric=42v");
      it->set("check_interval", "1");
    }
    engine_config.generate_services(3);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_services().begin()),
         end(engine_config.get_services().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "1");
      it->set("check_command", "test_command!0!output|metric=42v");
      it->set("check_interval", "1");
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("execute_host_checks", "1");
    engine_config.set_directive("execute_service_checks", "1");
    test::centengine engine(&engine_config);

    // Launch and stop Centreon Engine.
    engine.start();
    test::sleep_for(20);
    engine.stop();

    // Check that some metric RRD file exist.
    if (QDir(metrics_path.c_str())
            .entryList(QDir::AllEntries | QDir::NoDotAndDotDot)
            .isEmpty())
      throw(exceptions::msg() << "no metrics graphs were generated");

    // Check that no status RRD files exist.
    if (!QDir(status_path.c_str())
             .entryList(QDir::AllEntries | QDir::NoDotAndDotDot)
             .isEmpty())
      throw(exceptions::msg() << "some status graphs were generated");

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
    broker.stop();
  } catch (std::exception const& e) {
    std::cout << e.what() << "\n";
  } catch (...) {
    std::cout << "unknown exception\n";
  }

  // Cleanup.
  if (!metrics_path.empty())
    test::recursive_remove(metrics_path);
  if (!status_path.empty())
    test::recursive_remove(status_path);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
