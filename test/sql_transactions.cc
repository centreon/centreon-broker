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
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_sql_transactions"
#define TRANSACTION_TIMEOUT 10

/**
 *  Check that transactions work properly with SQL endpoint.
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

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/sql_transactions.xml\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(20 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Base time.
    time_t now(time(NULL));

    // Check 'instances' table.
    {
      std::ostringstream query;
      query << "SELECT last_alive, name"
            << "  FROM rt_instances"
            << "  WHERE instance_id=42";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()) || !q.next())
        throw (exceptions::msg() << "cannot read instances from DB: "
               << q.lastError().text().toStdString().c_str());
      if ((static_cast<time_t>(q.value(0).toLongLong())
           + 7 * MONITORING_ENGINE_INTERVAL_LENGTH
           + TRANSACTION_TIMEOUT
           < now)
          || (q.value(1).toString() != "MyBroker"))
        throw (exceptions::msg()
               << "invalid entry in 'instances': got (last_alive "
               << q.value(0).toLongLong() << ", name "
               << qPrintable(q.value(1).toString()) << "), expected ("
               << now - 7 * MONITORING_ENGINE_INTERVAL_LENGTH - TRANSACTION_TIMEOUT
               << ":, MyBroker)");
      if (q.next())
        throw (exceptions::msg() << "too much entries in 'instances'");
    }

    // Check 'hosts' table.
    {
      std::ostringstream query;
      query << "SELECT host_id, name, last_check"
            << "  FROM rt_hosts"
            << "  ORDER BY host_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read hosts from DB: "
               << q.lastError().text().toStdString().c_str());
      for (unsigned int i(1); i <= 10; ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough entries in 'hosts': got "
                 << i << " expected 10");
        if ((q.value(0).toUInt() != i)
            || (q.value(1).toUInt() != i)
            || (static_cast<time_t>(q.value(2).toLongLong())
                + 7 * MONITORING_ENGINE_INTERVAL_LENGTH
                + TRANSACTION_TIMEOUT
                < now))
          throw (exceptions::msg() << "invalid entry in 'hosts' ("
                 << i << "): got (host_id " << q.value(0).toUInt()
                 << ", name " << qPrintable(q.value(1).toString())
                 << ", last_check " << q.value(2).toLongLong()
                 << "), expected (" << i << ", " << i << ", "
                 << now - 7 * MONITORING_ENGINE_INTERVAL_LENGTH - TRANSACTION_TIMEOUT
                 << ":)");
      }
      if (q.next())
        throw (exceptions::msg() << "too much entries in 'hosts'");
    }

    // Check 'services' table.
    {
      std::ostringstream query;
      query << "SELECT host_id, service_id, description, last_check"
            << "  FROM rt_services"
            << "  ORDER BY host_id ASC, service_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read services from DB: "
               << q.lastError().text().toStdString().c_str());
      for (unsigned int i(1); i <= 10 * 5; ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough entries in 'services': got "
                 << i << " expected 50");
        if ((q.value(0).toUInt() != ((i - 1) / 5 + 1))
            || (q.value(1).toUInt() != i)
            || (q.value(2).toUInt() != i)
            || (static_cast<time_t>(q.value(3).toLongLong())
                + 7 * MONITORING_ENGINE_INTERVAL_LENGTH
                + TRANSACTION_TIMEOUT
                < now))
          throw (exceptions::msg() << "invalid entry in 'services' ("
                 << i << "): got (host_id " << q.value(0).toUInt()
                 << ", service_id " << q.value(1).toUInt()
                 << ", description " << q.value(2).toUInt()
                 << ", last_check " << q.value(3).toLongLong()
                 << "), expected (" << ((i - 1) / 5 + 1) << ", " << i
                 << ", " << i << ", "
                 << now - 7 * MONITORING_ENGINE_INTERVAL_LENGTH - TRANSACTION_TIMEOUT
                 << ":)");
      }
      if (q.next())
        throw (exceptions::msg() << "too much entries in 'services'");
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
