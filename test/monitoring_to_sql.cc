/*
** Copyright 2012 Merethis
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
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_monitoring_to_sql"

/**
 *  Check that monitoring is properly inserted in SQL database.
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
  external_command commander;
  engine daemon;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/monitoring_to_sql.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep(30 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Base time.
    time_t now(time(NULL));

    // Check 'instances' table.
    {
      std::ostringstream query;
      query << "SELECT last_alive, name"
            << "  FROM instances"
            << "  WHERE instance_id=42";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read instances from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() + 30 < now)
          || (q.value(1).toString() != "MyBroker")
          || q.next())
        throw (exceptions::msg() << "invalid entry in 'instances'");
    }

    // Check 'hosts' table.
    {
      std::ostringstream query;
      query << "SELECT host_id, name, last_check"
            << "  FROM hosts"
            << "  ORDER BY host_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read hosts from DB: "
               << q.lastError().text().toStdString().c_str());
      for (unsigned int i(1); i <= 10; ++i) {
        if (!q.next()
            || (q.value(0).toUInt() != i)
            || (q.value(1).toUInt() != i)
            || (q.value(2).toUInt() + 30 < now))
          throw (exceptions::msg() << "invalid entry in 'hosts' ("
                 << i << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much entries in 'hosts'");
    }

    // Check 'services' table.
    {
      std::ostringstream query;
      query << "SELECT host_id, service_id, description, last_check"
            << "  FROM services"
            << "  ORDER BY host_id ASC, service_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read services from DB: "
               << q.lastError().text().toStdString().c_str());
      for (unsigned int i(1); i <= 10 * 5; ++i) {
        if (!q.next()
            || (q.value(0).toUInt() != ((i - 1) / 5 + 1))
            || (q.value(1).toUInt() != i)
            || (q.value(2).toUInt() != i)
            || (q.value(3).toUInt() + 30 < now))
          throw (exceptions::msg() << "invalid entry in 'services' ("
                 << i << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much entries in 'services'");
    }

    // Get current time.
    time_t t1(time(NULL));

    // Put a service in a critical state
    // to generate logs (checked below).
    {
      commander.execute("ENABLE_PASSIVE_SVC_CHECKS;1;2");
      commander.execute("DISABLE_SVC_CHECK;1;2");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output1");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output2");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output3");
    }

    // Run a while.
    sleep(6 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Current time.
    now = time(NULL);

    // Check generated logs.
    {
      std::ostringstream query;
      query << "SELECT ctime, host_name, output, service_description,"
            << "       status, type"
            << "  FROM logs"
            << "  WHERE host_id=1 AND msg_type=0 AND service_id=2";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get logs from DB: "
               << qPrintable(q.lastError().text()));
      if (!q.next()
          || (q.value(0).toUInt() < t1)
          || (q.value(0).toUInt() > now)
          || (q.value(1).toString() != "1")
          || (q.value(2).toString() != "output3\n")
          || (q.value(3).toString() != "2")
          || (q.value(4).toUInt() != 2)
          || (q.value(5).toUInt() != 1)
          || q.next())
        throw (exceptions::msg() << "invalid entry in the logs table");
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
  config_remove(engine_config_path.c_str());
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
