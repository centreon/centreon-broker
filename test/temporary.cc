/*
** Copyright 2012-2013 Merethis
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
#include <QDir>
#include <QFile>
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

#define DB_NAME "broker_temporary"
#define HOST_COUNT 5
#define SERVICES_BY_HOST 10

/**
 *  Check that temporary work.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string cbd_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string temporary_path(tmpnam(NULL));
  engine daemon;
  cbd broker;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Write cbd configuration file.
    {
      std::ofstream ofs;
      ofs.open(
            cbd_config_path.c_str(),
            std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw (exceptions::msg()
               << "cannot open cbd configuration file '"
               << cbd_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR "/test/cfg/broker_modules.xml</include>\n"
          << "  <instance>84</instance>\n"
          << "  <instance_name>TheTemporaryCBD</instance_name>\n"
          << "  <event_queue_max_size>10</event_queue_max_size>\n"
          << "  <temporary>\n"
          << "    <name>Temporary</name>\n"
          << "    <type>file</type>\n"
          << "    <path>" << temporary_path << "</path>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "  </temporary>\n"
          << "  <!--\n"
          << "  <logger>\n"
          << "    <type>file</type>\n"
          << "    <name>cbd.log</name>\n"
          << "    <config>1</config>\n"
          << "    <debug>1</debug>\n"
          << "    <error>1</error>\n"
          << "    <info>1</info>\n"
          << "    <level>3</level>\n"
          << "  </logger>\n"
          << "  -->\n"
          << "  <input>\n"
          << "    <name>FromEngine</name>\n"
          << "    <type>tcp</type>\n"
          << "    <port>5683</port>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "  </input>\n"
          << "  <output>\n"
          << "    <name>ToSQL</name>\n"
          << "    <type>sql</type>\n"
          << "    <db_type>" DB_TYPE "</db_type>\n"
          << "    <db_host>" DB_HOST "</db_host>\n"
          << "    <db_port>" DB_PORT "</db_port>\n"
          << "    <db_user>" DB_USER "</db_user>\n"
          << "    <db_password>" DB_PASSWORD "</db_password>\n"
          << "    <db_name>" DB_NAME "</db_name>\n"
          << "    <queries_per_transaction>0</queries_per_transaction>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/temporary.xml\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Lock tables.
    {
      QSqlQuery q(db);
      std::string query("LOCK TABLES logs WRITE, instances WRITE");
      if (!q.exec(query.c_str()))
        throw (exceptions::msg() << "cannot lock tables: "
               << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(cbd_config_path);
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Send SIGHUP to Broker daemon.
    broker.update();
    sleep_for(1 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();
    sleep_for(1 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that temporary file exists.
    std::string real_file_path;
    {
      // Split directory components.
      size_t offset(temporary_path.find_last_of('/'));
      std::string temporary_dir;
      std::string temporary_file;
      if (offset == std::string::npos) {
        temporary_dir = ".";
        temporary_file = temporary_path;
      }
      else {
        temporary_dir = temporary_path.substr(0, offset);
        temporary_file = temporary_path.substr(offset + 1);
      }

      // Get existing file.
      QDir dir(temporary_dir.c_str());
      temporary_file.append("*");
      QStringList filters;
      filters.push_back(temporary_file.c_str());
      QStringList entries;
      entries = dir.entryList(filters);
      if (entries.isEmpty())
        throw (exceptions::msg() << "no temporary file exists");

      // Check file properties.
      real_file_path = temporary_dir;
      real_file_path.append("/");
      real_file_path.append(entries.begin()->toStdString());
      QFile f(real_file_path.c_str());
      if (f.size() <= 0)
        throw (exceptions::msg() << "temporary file '"
               << real_file_path.c_str() << "' is empty");
    }

    // Unlock tables.
    {
      QSqlQuery q(db);
      if (!q.exec("UNLOCK TABLES"))
        throw (exceptions::msg() << "cannot unlock tables: "
               << q.lastError().text());
    }
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Terminate monitoring engine.
    daemon.stop();

    // Terminate Broker daemon.
    sleep_for(16 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.stop();
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that temporary file got removed.
    if (QFile::exists(real_file_path.c_str()))
      throw (exceptions::msg() << "temporary file '"
             << real_file_path.c_str() << "' still exists");

    // Check host count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM hosts";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read host count from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != HOST_COUNT)
          || q.next())
        throw (exceptions::msg() << "invalid host count");
    }

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM services";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot read service count from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != HOST_COUNT * SERVICES_BY_HOST)
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
  ::remove(cbd_config_path.c_str());
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
