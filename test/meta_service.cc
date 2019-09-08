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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

#define DB_NAME "broker_meta_service"
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
  engine_extcmd commander;
  engine monitoring;
  test_db db;
  test_file cfg;

  try {
    // Prepare database.
    db.open(DB_NAME, NULL, true);

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, SERVICES_BY_HOST);
    {
      int i(42);
      for (std::list<command>::iterator it(commands.begin()),
           end(commands.end());
           it != end; ++it, ++i) {
        std::ostringstream oss;
        oss << MY_PLUGIN_PATH << " 0 \"output " << i << "|metric=" << i << "\"";
        std::string cmd(oss.str());
        it->command_line = new char[cmd.size() + 1];
        strcpy(it->command_line, cmd.c_str());
      }
    }
    generate_hosts(hosts, 1);
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      int i(1);
      for (std::list<service>::iterator it(services.begin()),
           end(services.end());
           it != end; ++it, ++i) {
        std::ostringstream oss;
        oss << i;
        std::string cmd(oss.str());
        it->service_check_command = new char[cmd.size() + 1];
        strcpy(it->service_check_command, cmd.c_str());
      }
    }
    commander.set_file(tmpnam(NULL));
    cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/meta_service.xml.in");
    cfg.set("COMMAND_FILE", commander.get_file());
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << cfg.generate() << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services, &commands);

    // List of initialization queries.
    std::list<std::string> storageq;
    std::list<std::string> centreonq;
    storageq.push_back(
        "INSERT INTO rt_instances (instance_id, name)"
        "  VALUES (42, 'Central')");
    storageq.push_back(
        "INSERT INTO rt_hosts (host_id, name, instance_id)"
        "  VALUES (1, '1', 42)");
    for (int i(1); i <= SERVICES_BY_HOST; ++i) {
      {
        std::ostringstream oss;
        oss << "INSERT INTO rt_services (host_id, description, service_id)"
            << "  VALUES (1, '" << i << "', " << i << ")";
        storageq.push_back(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "INSERT INTO rt_index_data (host_id, service_id)"
            << "  VALUES (1, " << i << ")";
        storageq.push_back(oss.str());
      }
      {
        std::ostringstream oss;
        oss << "INSERT INTO rt_metrics (index_id, metric_name)"
            << "  SELECT index_id, 'metric'"
            << "    FROM rt_index_data"
            << "    WHERE host_id=1 AND service_id=" << i;
        storageq.push_back(oss.str());
      }
    }
    centreonq.push_back(
        "INSERT INTO cfg_organizations (organization_id, name, shortname)"
        "  VALUES (1, '42', '42')");
    centreonq.push_back(
        "INSERT INTO cfg_meta_services (meta_name, calcul_type, "
        "            meta_select_mode, regexp_str, metric, "
        "            warning, critical, meta_activate, organization_id)"
        "  VALUES ('1-AVE', 'AVE', '2', '%', 'metric', 'W', 'C', '1', 1),"
        "         ('2-MIN', 'MIN', '2', '%', 'metric', 'W', 'C', '1', 1),"
        "         ('3-MAX', 'MAX', '2', '%', 'metric', 'W', 'C', '1', 1),"
        "         ('4-SUM', 'SOM', '2', '%', 'metric', 'W', 'C', '1', 1)");
    centreonq.push_back(
        "INSERT INTO cfg_hosts (host_id, host_name, organization_id) VALUES "
        "(1, 'Central', 1)");
    centreonq.push_back(
        "INSERT INTO cfg_services (service_id, service_description, "
        "organization_id)"
        "  VALUES (1, 'meta_1', 1), (2, 'meta_2', 1), (3, 'meta_3', 1),"
        "         (4, 'meta_4', 1)");
    centreonq.push_back(
        "INSERT INTO cfg_hosts_services_relations (host_host_id, "
        "service_service_id)"
        "  VALUES (1, 1), (1, 2), (1, 3), (1, 4)");

    // Execute initialization queries.
    for (std::list<std::string>::const_iterator it(storageq.begin()),
         end(storageq.end());
         it != end; ++it) {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(it->c_str()))
        throw(exceptions::msg() << "could not execute a storage query: "
                                << q.lastError().text() << " (" << *it << ")");
    }
    for (std::list<std::string>::const_iterator it(centreonq.begin()),
         end(centreonq.end());
         it != end; ++it) {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(it->c_str()))
        throw(exceptions::msg() << "could not execute a centreon query: "
                                << q.lastError().text() << " (" << *it << ")");
    }

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(15);

    // Check values of meta-services.
    {
      double values[] = {
          (SERVICES_BY_HOST - 1) / 2.0 + 42.0, 42.0,
          42.0 + SERVICES_BY_HOST - 1.0,
          ((SERVICES_BY_HOST - 1) / 2.0 + 42.0) * SERVICES_BY_HOST};
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT value, meta_name"
                  "  FROM cfg_meta_services"
                  "  ORDER BY meta_name ASC"))
        throw(exceptions::msg() << "could not fetch meta-services values: "
                                << q.lastError().text());
      for (size_t i(0); i < sizeof(values) / sizeof(*values); ++i) {
        if (!q.next())
          throw(exceptions::msg()
                << "not enough entries in meta_service table: got " << i
                << ", expected " << sizeof(values) / sizeof(*values));
        else if (fabs(values[i] - q.value(0).toDouble()) > 0.1)
          throw(exceptions::msg()
                << "invalid value for meta-service '" << q.value(1).toString()
                << "': got " << q.value(0).toDouble() << ", expected "
                << values[i]);
      }
      if (q.next())
        throw(exceptions::msg() << "too much entries in meta_service table");
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
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
