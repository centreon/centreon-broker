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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QFile>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
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
  external_command commander;
  engine monitoring;
  cbd broker;
  test_file broker_cfg;
  test_db db;

  try {
    // Prepare database.
    db.open(NULL, NULL, DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 2);
    // XXX : add FLAGFILE custom macro

    // Populate database.
    db.centreon_run(
         "INSERT INTO cfg_hosts (host_id, host_name)"
         "  VALUES (1, 'Host1')",
         "could not create host");
    db.centreon_run(
         "INSERT INTO cfg_services (service_id,"
         "            service_description)"
         "  VALUES (1, 'Service1'), (2, 'Service2')",
         "could not create services");
    db.centreon_run(
         "INSERT INTO cfg_hosts_services_relations (host_host_id,"
         "            service_service_id)"
         "  VALUES (1, 1), (1, 2)",
         "could not link host and services");

    // Create contact in DB.
    db.centreon_run(
         "INSERT INTO cfg_contacts (contact_id, contact_name)"
         "  VALUES (1, 'Contact1')",
         "could not create contact");
    db.centreon_run(
         "INSERT INTO cfg_contacts_services_relations (contact_id,"
         "            service_service_id)"
         "  VALUES (1, 1), (1, 2)",
         "could not link services and contact");

    // Create notification command in DB.
    db.centreon_run(
         "INSERT INTO cfg_commands (command_id, command_name,"
         "            command_line)"
         "  VALUES (1, 'NotificationCommand1', 'cmake -E touch $_SERVICEFLAGFILE$')",
         "could not create notification command");

    // Create notification rules in DB.
    db.centreon_run(
         "INSERT INTO cfg_notification_method (method_id, name,"
         "            command_id, `interval`)"
         "  VALUES (1, 'NotificationMethod', 1, 300)",
         "could not create notification method");
    db.centreon_run(
         "INSERT INTO rt_notification_rules (method_id,"
         "            timeperiod_id, contact_id, host_id,"
         "            service_id)"
         "  VALUES (1, NULL, 1, 1, 2)",
         "could not create notification rule");

    // Start monitoring.
    broker_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/notification_non_correlated.xml.in");
    broker.set_config_file(broker_cfg.generate());
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

// external cmd CRITICAL svc1 + svc2

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
  broker.stop();
  ::remove(flag_file.c_str());
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
