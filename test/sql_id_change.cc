/*
** Copyright 2012-2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_sql_id_change"

/**
 *  Check that broker correctly manage id changes.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  cbd broker;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);
    db.set_remove_db_on_close(false);

    // Create the config xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql_id_change.xml.in");
    file.set("DB_NAME", DB_NAME);
    std::string config_file = file.generate();

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 1);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << config_file;
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Change the service id.
    services.back().display_name = ::strdup("2");
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Restart daemon.
    daemon.stop();
    daemon.start();
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
               "  FROM services "
               "  WHERE host_id = 1"
               "    AND description = '1'"
               "    AND enabled = '1'";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read service count from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != 1)
          || q.next())
        throw (exceptions::msg() << "invalid service count");
    }

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
