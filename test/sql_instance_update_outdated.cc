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

#define DB_NAME "broker_sql_instance_update_outdated"
#define INSTANCE_TIMEOUT 30
#define INSTANCE_TIMEOUT_STR "30"

/**
 *  Check that instance outdated status and
 *  service/host status are properly updated.
 *
 *  @return EXIT_SUCCESS on success.
 **/

int main() {
  int retval(EXIT_FAILURE);

  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  cbd broker;
  test_db db;
  test_file cfg_cbmod;
  test_file cfg_cbd;

  try {
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    cfg_cbmod.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql_instance_update_outdated_1.xml.in");
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << cfg_cbmod.generate() << "\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Start Broker daemon.
    cfg_cbd.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/sql_instance_update_outdated_2.xml.in");
    cfg_cbd.set("INSTANCE_TIMEOUT", INSTANCE_TIMEOUT_STR);
    broker.set_config_file(cfg_cbd.generate());
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    broker.update();
    sleep_for((INSTANCE_TIMEOUT + 2) * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Terminate monitoring engine.
    daemon.stop();

    sleep_for((INSTANCE_TIMEOUT + 2) * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Check for outdated instance
    {
      std::ostringstream query;
      query << "SELECT COUNT(instance_id) from instances where outdated = TRUE";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check outdated instances from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != 1)
          || q.next())
        throw (exceptions::msg() << "instance not outdated");
    }

    // Check for outdated services.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM services WHERE state = " << STATE_UNKNOWN;
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check outdated services from DB: "
               << q.lastError().text().toStdString().c_str());

      if (!q.next()
          || (q.value(0).toUInt() != 50)
          || q.next())
        throw (exceptions::msg() << "services not outdated");
    }

    // Check for outdated hosts.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM hosts WHERE state = " << HOST_UNREACHABLE;
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check outdated hosts from DB: "
               << q.lastError().text().toStdString().c_str());

      if (!q.next()
          || (q.value(0).toUInt() != 10)
          || q.next())
        throw (exceptions::msg() << "hosts not outdated");
    }

    daemon.start();

    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check for living instance
    {
      std::ostringstream query;
      query
        << "SELECT COUNT(instance_id) FROM instances WHERE outdated=0";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check living instances from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != 1)
          || q.next())
        throw (exceptions::msg() << "living instance not updated");
    }

    // Check for living services.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM services WHERE state != " << STATE_UNKNOWN;
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check living services from DB: "
               << q.lastError().text().toStdString().c_str());

      if (!q.next()
          || (q.value(0).toUInt() != 50)
          || q.next())
        throw (exceptions::msg() << "living services not updated");
    }

    // Check for living hosts.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM hosts WHERE state != " << HOST_UNREACHABLE;
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot check living hosts from DB: "
               << q.lastError().text().toStdString().c_str());

      if (!q.next()
          || (q.value(0).toUInt() != 10)
          || q.next())
        throw (exceptions::msg() << "living hosts not updated");
    }

    // Success
    retval = EXIT_SUCCESS;

  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // cleanup
  daemon.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return retval;
}
