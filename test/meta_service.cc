/*
** Copyright 2014 Merethis
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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

#define DB_NAME_CENTREON "broker_meta_service_centreon"
#define DB_NAME_STORAGE "broker_meta_service_storage"
#define SERVICES_BY_HOST 10

using namespace com::centreon::broker;

/**
 *  Check that meta-services are properly working.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<command> commands;
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine monitoring;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME_STORAGE, NULL, DB_NAME_CENTREON);

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, SERVICES_BY_HOST);
    {
      int i(42);
      for (std::list<command>::iterator
             it(commands.begin()),
             end(commands.end());
           it != end;
           ++it, ++i) {
        std::ostringstream oss;
        oss << MY_PLUGIN_PATH << " 0 \"output " << i
            << "|metric=" << i << "\"";
        std::string cmd(oss.str());
        it->command_line = new char[cmd.size() + 1];
        strcpy(it->command_line, cmd.c_str());
      }
    }
    generate_hosts(hosts, 1);
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      int i(1);
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it, ++i) {
        std::ostringstream oss;
        oss << i;
        std::string cmd(oss.str());
        it->service_check_command = new char[cmd.size() + 1];
        strcpy(it->service_check_command, cmd.c_str());
      }
    }
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/meta_service.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      &commands);

    // List of initialization queries.
    std::list<std::string> queries;
    queries.push_back(
              "INSERT INTO instances (instance_id, name)"
              "  VALUES (42, 'Central')");
    queries.push_back(
              "INSERT INTO hosts (host_id, name, instance_id)"
              "  VALUES (1, '1', 42)");
    for (int i(1); i <= SERVICES_BY_HOST; ++i) {
      std::ostringstream oss1;
      oss1 << "INSERT INTO services (host_id, description, service_id)"
           << "  VALUES (1, '" << i << "', " << i << ")";
      queries.push_back(oss1.str());
      std::ostringstream oss2;
      oss2 << "INSERT INTO index_data (host_id, service_id)"
           << "  VALUES (1, " << i << ")";
      queries.push_back(oss2.str());
      std::ostringstream oss3;
      oss3 << "INSERT INTO metrics (index_id, metric_name)"
           << "  SELECT id, 'metric'"
           << "    FROM index_data"
           << "    WHERE host_id=1 AND service_id=" << i;
      queries.push_back(oss3.str());
    }

    // Execute initialization queries.
    {
      for (std::list<std::string>::const_iterator
             it(queries.begin()),
             end(queries.end());
           it != end;
           ++it) {
        QSqlQuery q(*db.storage_db());
        if (!q.exec(it->c_str()))
          throw (exceptions::msg() << "could not execute a query: "
                 << q.lastError().text() << " (" << *it << ")");
      }
    }
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(
              "INSERT INTO meta_service (meta_name, calcul_type, "
              "            meta_select_mode, regexp_str, metric, "
              "            warning, critical, meta_activate)"
              "  VALUES ('1-AVE', 'AVE', '2', '%', 'metric', 'W', 'C', '1'),"
              "         ('2-MIN', 'MIN', '2', '%', 'metric', 'W', 'C', '1'),"
              "         ('3-MAX', 'MAX', '2', '%', 'metric', 'W', 'C', '1'),"
              "         ('4-SUM', 'SOM', '2', '%', 'metric', 'W', 'C', '1')"))
        throw (exceptions::msg() << "could not create meta-services: "
               << q.lastError().text());
    }

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(15 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check values of meta-services.
    {
      double values[] = {
        (SERVICES_BY_HOST - 1) / 2.0 + 42.0,
        42.0,
        42.0 + SERVICES_BY_HOST - 1.0,
        ((SERVICES_BY_HOST - 1) / 2.0 + 42.0) * SERVICES_BY_HOST
      };
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(
               "SELECT value, meta_name"
               "  FROM meta_service"
               "  ORDER BY meta_name ASC"))
        throw (exceptions::msg()
               << "could not fetch meta-services values: "
               << q.lastError().text());
      for (size_t i(0); i < sizeof(values) / sizeof(*values); ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough entries in meta_service table: got "
                 << i << ", expected "
                 << sizeof(values) / sizeof(*values));
        else if (fabs(values[i] - q.value(0).toDouble()) > 0.1)
          throw (exceptions::msg()
                 << "invalid value for meta-service '"
                 << q.value(1).toString() << "': got "
                 << q.value(0).toDouble() << ", expected "
                 << values[i]);
      }
      if (q.next())
        throw (exceptions::msg()
               << "too much entries in meta_service table");
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  monitoring.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
