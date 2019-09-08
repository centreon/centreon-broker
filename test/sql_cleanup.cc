/*
** Copyright 2014-2015 Centreon
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

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_sql_cleanup"

/**
 *  Check that cleanup thread properly works.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine monitoring;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 2);
    std::string additional_config;
    additional_config = "broker_module=" CBMOD_PATH " " PROJECT_SOURCE_DIR
                        "/test/cfg/sql_cleanup.xml\n";

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(8);

    // Flag instance as deleted.
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("UPDATE rt_instances SET deleted=TRUE"))
        throw(exceptions::msg() << "could not flag instances as deleted: "
                                << q.lastError().text());
    }

    // Let cleanup thread work.
    sleep_for(8);

    // Check hosts table.
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_hosts") || !q.next())
        throw(exceptions::msg()
              << "could not fetch host count: " << q.lastError().text());
      if (q.value(0).toInt())
        throw(exceptions::msg()
              << q.value(0).toInt() << " hosts still in database");
    }
    // Check services table.
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_services") || !q.next())
        throw(exceptions::msg()
              << "could not fetch service count: " << q.lastError().text());
      if (q.value(0).toInt())
        throw(exceptions::msg()
              << q.value(0).toInt() << " services still in database");
    }
    // Check modules table.
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_modules") || !q.next())
        throw(exceptions::msg()
              << "could not fetch modules count: " << q.lastError().text());
      if (q.value(0).toInt())
        throw(exceptions::msg()
              << q.value(0).toInt() << " modules still in database");
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  monitoring.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
