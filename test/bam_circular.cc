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
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_bam_circular"

/**
 *  Check that BAM engine detect circular paths in BA graph.
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
  test_file cfg;

  try {
    // Prepare database.
    db.open(DB_NAME, NULL, true);

    //
    // The following test uses multiple nodes that will be manipulated
    // by ID. They are listed below for reference.
    //
    //   - service (1, 1)           Normal service, impacts KPI 200.
    //   - service (1, 2)           Normal service, impacts KPI 201.
    //   - service (1, 3)           BA 600 service, used in boolexp 1000.
    //   - service (1, 4)           BA 601 service.
    //   - boolean expression 1000  Use service (1, 3), impacts KPI 203.
    //   - KPI 200                  Service (1, 1), impacts BA 600.
    //   - KPI 201                  Service (1, 2), impacts BA 600.
    //   - KPI 202                  BA 601, impacts BA 600.
    //   - KPI 203                  Boolexp 1000, impacts BA 601.
    //   - BA 600                   BA.
    //   - BA 601                   BA.
    //

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 4);
    {
      // Service (1, 3) is a virtual BA service.
      std::list<service>::iterator s(services.begin());
      ++s;
      ++s;
      delete[] s->description;
      s->description = NULL;
      std::string str("ba_600");
      s->description = new char[str.size() + 1];
      strcpy(s->description, str.c_str());
      // Service (1, 4) is a virtual BA service.
      ++s;
      delete[] s->description;
      s->description = NULL;
      str = "ba_601";
      s->description = new char[str.size() + 1];
      strcpy(s->description, str.c_str());
    }
    cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    cfg.set("DB_NAME_CENTREON", DB_NAME);
    cfg.set("DB_NAME_BI", "");
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cfg.generate() << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // Populate database.
    {
      char const* queries[] = {
          "INSERT INTO cfg_organizations (organization_id, name,"
          "            shortname)"
          "  VALUES (1, '42', '42')",
          "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
          "  VALUES (1, '1', 1)",
          "INSERT INTO cfg_services (service_id, service_description,"
          "            organization_id)"
          "  VALUES (1, 'svc1', 1),"
          "         (2, 'svc2', 1),"
          "         (3, 'ba_600', 1),"
          "         (4, 'ba_601', 1)",
          "INSERT INTO cfg_hosts_services_relations (host_host_id,"
          "            service_service_id)"
          "  VALUES (1, 1), (1, 2), (1, 3), (1, 4)",
          "INSERT INTO cfg_bam_ba_types (ba_type_id, name, slug,"
          "            description)"
          "  VALUES (1, 'Default', 'default', 'Default type')",
          "INSERT INTO cfg_bam (ba_id, name, level_w, level_c, activate,"
          "            ba_type_id, organization_id)"
          "  VALUES (600, 'BA600', 80, 90, 1, 1, 1),"
          "         (601, 'BA601', 50, 60, 1, 1, 1)",
          "INSERT INTO cfg_bam_poller_relations (ba_id, poller_id)"
          "  VALUES (600, 42), (601, 42)",
          "INSERT INTO cfg_bam_boolean (boolean_id, name, expression,"
          "            bool_state, activate)"
          "  VALUES (1000, 'boolexp1000', '{1 ba_600} {is} {CRITICAL}', 1, 1)",
          "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
          "            service_id, id_indicator_ba, id_ba, meta_id,"
          "            boolean_id, config_type, drop_warning,"
          "            drop_critical, drop_unknown, state_type, activate)"
          "  VALUES (200, '0', 1, 1, NULL, 600, NULL, NULL, '0', 10, 20, 30, "
          "'1', '1'),"
          "         (201, '0', 1, 2, NULL, 600, NULL, NULL, '0', 30, 40, 60, "
          "'1', '1'),"
          "         (202, '1', NULL, NULL, 601, 600, NULL, NULL, '0', 50, 60, "
          "70, '1', '1'),"
          "         (203, '3', NULL, NULL, NULL, 601, NULL, 1000, '0', 60, 80, "
          "90, '1', '1')",
          NULL};
      for (int i(0); queries[i]; ++i) {
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(queries[i]))
          throw(exceptions::msg()
                << "could not execute query: " << q.lastError().text() << " ("
                << queries[i] << ")");
      }
    }

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    // Let the daemon initialize.
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that no BA was computed.
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT current_level"
                  "  FROM cfg_bam"
                  " WHERE current_level IS NOT NULL"))
        throw(exceptions::msg() << "could not retrieve current BA level: "
                                << q.lastError().text());
      if (q.next())
        throw(exceptions::msg() << "some BA was computed");
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    db.set_remove_db_on_close(false);
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    db.set_remove_db_on_close(false);
  }

  // Cleanup.
  monitoring.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
