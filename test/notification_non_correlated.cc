/*
** Copyright 2014-2015 Merethis
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
#include <iostream>
#include <QFile>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_notification_non_correlated"

/**
 *  Check that notification is properly enabled when non-correlation
 *  option is set on services.
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
  std::string flag_file(tmpnam(NULL));
  std::string node_cache_file(tmpnam(NULL));
  external_command commander;
  engine monitoring;
  test_file broker_cfg;
  test_db db;

  try {
    // Log some info.
    std::cout << "flag file: " << flag_file << "\n";
    std::cout << "node cache: " << node_cache_file << "\n";

    // Prepare database.
    db.open(NULL, NULL, DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 2);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it) {
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    set_custom_variable(
      services.back(),
      "FLAGFILE",
      flag_file.c_str());

    // Populate database.
    db.centreon_run(
        "INSERT INTO cfg_organizations (organization_id, name, shortname)"
        "  VALUES (1, '42', '42')",
        "could not create organization");

    db.centreon_run(
         "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
         "  VALUES (1, 'Host1', 1)",
         "could not create host");
    db.centreon_run(
         "INSERT INTO cfg_services (service_id,"
         "            service_description,"
         "            service_notification_options, organization_id)"
         "  VALUES (1, 'Service1', 'ywcu', 1), (2, 'Service2', 'ywcu', 1)",
         "could not create services");
    db.centreon_run(
         "INSERT INTO cfg_hosts_services_relations (host_host_id,"
         "            service_service_id)"
         "  VALUES (1, 1), (1, 2)",
         "could not link host and services");

    // Create contact in DB.
    db.centreon_run(
         "INSERT INTO cfg_contacts (contact_id, description)"
         "  VALUES (1, 'Contact1')",
         "could not create contact");

    // Create notification command in DB.
    db.centreon_run(
         "INSERT INTO cfg_commands (command_id, command_name,"
         "            command_line, organization_id)"
         "  VALUES (1, 'NotificationCommand1', 'cmake -E touch $_SERVICEFLAGFILE$', 1)",
         "could not create notification command");

    // Create notification rules in DB.
    db.centreon_run(
         "INSERT INTO cfg_notification_methods (method_id,"
         "            name, command_id, `interval`, status, types)"
         "  VALUES (1, 'NotificationMethod', 1, 300, 'w,c,u', 'n')",
         "could not create notification method");
    db.centreon_run(
         "INSERT INTO cfg_notification_rules (rule_id, method_id, "
         "            timeperiod_id, owner_id, contact_id, host_id,"
         "            service_id, enabled)"
         "  VALUES (1, 1, NULL, 1, 1, 1, 2, 1)",
         "could not create notification rule (cfg)");
    db.centreon_run(
         "INSERT INTO rt_notification_rules (rule_id, method_id,"
         "            timeperiod_id, contact_id, host_id,"
         "            service_id)"
         "  VALUES (1, 1, NULL, 1, 1, 2)",
         "could not create notification rule (rt)");

    // Generate configuration.
    broker_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/notification_non_correlated.xml.in");
    broker_cfg.set("NODE_CACHE_FILE", node_cache_file);
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << broker_cfg.generate() << "\n";
      additional_config = oss.str();
    }
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // Start monitoring.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;1;0;Submitted by unit test");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;2;0;Submitted by unit test");
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Make services CRITICAL.
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;1;2;Submitted by unit test");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;2;2;Submitted by unit test");
    sleep_for(25 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check file creation.
    error = !QFile::exists(flag_file.c_str());
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  monitoring.stop();
  sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);
  ::remove(flag_file.c_str());
  ::remove(node_cache_file.c_str());
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
