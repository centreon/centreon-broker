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
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include <sys/stat.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/rrd_file.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_disable_status_graphs"
#define HOST_COUNT 2
#define SERVICES_BY_HOST 5

/**
 *  Check that status graphs generation can be disabled.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<command> commands;
  std::list<host> hosts;
  std::list<service> services;
  std::string cbmod_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  std::string status_path(tmpnam(NULL));
  engine daemon;

  // Log.
  std::clog << "status directory: " << status_path << "\n"
            << "metrics directory: " << metrics_path << std::endl;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);
    mkdir(status_path.c_str(), S_IRWXU);

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
          << "    <name>EngineToStorageUnitTest</name>\n"
          << "    <type>storage</type>\n"
          << "    <db_type>" DB_TYPE "</db_type>\n"
          << "    <db_host>" DB_HOST "</db_host>\n"
          << "    <db_port>" DB_PORT "</db_port>\n"
          << "    <db_user>" DB_USER "</db_user>\n"
          << "    <db_password>" DB_PASSWORD "</db_password>\n"
          << "    <db_name>" DB_NAME "</db_name>\n"
          << "    <queries_per_transaction>0</queries_per_transaction>\n"
          << "    <interval>" MONITORING_ENGINE_INTERVAL_LENGTH_STR "</interval>\n"
          << "    <length>2592000</length>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>StorageToRRDUnitTest</name>\n"
          << "    <type>rrd</type>\n"
          << "    <metrics_path>" << metrics_path << "</metrics_path>\n"
          << "    <status_path>" << status_path << "</status_path>\n"
          << "    <write_status>no</write_status>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 1);
    {
      std::list<command>::iterator it(commands.begin());
      char const* cmd;
      cmd = MY_PLUGIN_PATH " 0 \"output|metric=1unit\"";
      it->command_line = new char[strlen(cmd) + 1];
      strcpy(it->command_line, cmd);
    }
    generate_hosts(hosts, HOST_COUNT);
    {
      for (std::list<host>::iterator
             it(hosts.begin()),
             end(hosts.end());
           it != end;
           ++it) {
        char str[2];
        str[0] = '1';
        str[1] = '\0';
        it->host_check_command = new char[sizeof(str)];
        strcpy(it->host_check_command, str);
      }
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it) {
        char str[2];
        str[0] = '1';
        str[1] = '\0';
        it->service_check_command = new char[sizeof(str)];
        strcpy(it->service_check_command, str);
      }
    }
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_config_path;
      cbmod_loading = oss.str();
    }

    // Insert entries in index_data.
    {
      QSqlQuery q(db);
      // Host does not have status graph (yet).
      // for (unsigned int i(0); i < HOST_COUNT; ++i) {
      //   std::ostringstream query;
      //   query << "INSERT INTO index_data (host_id, service_id)"
      //         << "  VALUES (" << i + 1 << ", NULL)";
      //   if (!q.exec(query.str().c_str()))
      //     throw (exceptions::msg() << "cannot create index of host "
      //            << i + 1 << ": " << qPrintable(q.lastError().text()));
      // }
      for (unsigned int i(1); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        std::ostringstream query;
        query << "INSERT INTO index_data (host_id, service_id)"
              << "  VALUES (" << (i - 1) / SERVICES_BY_HOST + 1
              << ", " << i << ")";
        if (!q.exec(query.str().c_str()))
          throw (exceptions::msg() << "cannot create index of service ("
                 << i << ", " << i << ")");
      }
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services,
      &commands);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(25 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that no status RRD file exist.
    if (!QDir(status_path.c_str()).entryList(
                QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty())
      throw (exceptions::msg() << "some status graphs were generated");

    // Check that some metrics RRD files exist.
    if (QDir(metrics_path.c_str()).entryList(
               QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty())
      throw (exceptions::msg() << "no metrics graphs were generated");

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
  ::remove(cbmod_config_path.c_str());
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);
  recursive_remove(metrics_path);
  recursive_remove(status_path);

  return (retval);
}
