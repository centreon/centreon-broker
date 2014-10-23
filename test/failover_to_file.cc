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
#include <fstream>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_failover_to_file"

/**
 *  Check that encryption works on Broker stream.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string cbmod_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string retention_file_path(tmpnam(NULL));
  external_command commander;
  commander.set_file(tmpnam(NULL));
  engine daemon;
  cbd broker;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Write cbmod configuration file.
    {
      std::ofstream ofs;
      ofs.open(
            cbmod_config_path.c_str(),
            std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw (exceptions::msg()
               << "cannot open cbmod configuration file '"
               << cbmod_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR "/test/cfg/broker_modules.xml</include>\n"
          << "  <instance>42</instance>\n"
          << "  <instance_name>MyBroker</instance_name>\n"
          << "  <!--\n"
          << "  <logger>\n"
          << "    <type>file</type>\n"
          << "    <name>cbmod.log</name>\n"
          << "    <config>1</config>\n"
          << "    <debug>1</debug>\n"
          << "    <error>1</error>\n"
          << "    <info>1</info>\n"
          << "    <level>3</level>\n"
          << "  </logger>\n"
          << "  -->\n"
          << "  <output>\n"
          << "    <name>EngineToTCP</name>\n"
          << "    <failover>ToRetentionFile</failover>\n"
          << "    <type>tcp</type>\n"
          << "    <host>localhost</host>\n"
          << "    <port>5680</port>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "    <retry_interval>1</retry_interval>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>ToRetentionFile</name>\n"
          << "    <type>file</type>\n"
          << "    <path>" << retention_file_path << "</path>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "    <compression>yes</compression>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " " << cbmod_config_path;
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // T1.
    time_t t1(time(NULL));

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(12 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Temporary disable checks.
    commander.execute("STOP_EXECUTING_SVC_CHECKS");
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T2.
    time_t t2(time(NULL));

    // Start Broker daemon.
    broker.set_config_file(
             PROJECT_SOURCE_DIR "/test/cfg/failover_to_file.xml");
    broker.start();
    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that retention was successfully replayed.
    {
      std::ostringstream oss;
      oss << "SELECT last_check FROM services";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(oss.str().c_str()))
        throw (exceptions::msg()
               << "cannot get services' last check time from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0); i < 10 * 5; ++i) {
        if (!q.next())
          throw (exceptions::msg() << "not enough entries in DB: found "
                 << i << " expected 50");
        if ((static_cast<time_t>(q.value(0).toLongLong()) < t1)
            || (static_cast<time_t>(q.value(0).toLongLong()) > t2))
          throw (exceptions::msg()
                 << "invalid last update time from DB: "
                 << q.value(0).toUInt() << " was not between "
                 << t1 << " and " << t2);
      }
      if (q.next())
        throw (exceptions::msg()
               << "invalid entry count in services table");
    }

    // Stop Broker daemon.
    broker.stop();

    // T3.
    time_t t3(time(NULL));

    // Reenable checks.
    commander.execute("START_EXECUTING_SVC_CHECKS");
    sleep_for(12 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Redisable checks.
    commander.execute("STOP_EXECUTING_SVC_CHECKS");
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T4.
    time_t t4(time(NULL));

    // Restart Broker daemon.
    broker.start();
    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that retention was successfully replayed.
    {
      std::ostringstream oss;
      oss << "SELECT last_check FROM services";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(oss.str().c_str()))
        throw (exceptions::msg()
               << "cannot get services' last check time from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0); i < 10 * 5; ++i) {
        if (!q.next())
          throw (exceptions::msg() << "not enough entries in DB: found "
                 << i << " expected 50");
        if ((static_cast<time_t>(q.value(0).toLongLong()) < t3)
            || (static_cast<time_t>(q.value(0).toLongLong()) > t4))
          throw (exceptions::msg()
                 << "invalid last update time from DB: "
                 << q.value(0).toUInt() << " was not between "
                 << t1 << " and " << t2);
      }
      if (q.next())
        throw (exceptions::msg()
               << "invalid entry count in services table");
    }

    // Terminate monitoring engine.
    daemon.stop();

    // Terminate Broker daemon.
    broker.stop();

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
  ::remove(cbmod_config_path.c_str());
  ::remove(retention_file_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
