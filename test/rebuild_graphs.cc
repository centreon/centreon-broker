/*
** Copyright 2013 Merethis
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
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <QDateTime>
#include <QFileInfo>
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

#define DB_NAME "broker_rebuild_graphs"
#define HOST_COUNT 2
#define SERVICES_BY_HOST 5

/**
 *  Check that graphs can be properly rebuild.
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
          << "    <rebuild_check_interval>" MONITORING_ENGINE_INTERVAL_LENGTH_STR "</rebuild_check_interval>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>StorageToRRDUnitTest</name>\n"
          << "    <type>rrd</type>\n"
          << "    <metrics_path>" << metrics_path << "</metrics_path>\n"
          << "    <status_path>" << status_path << "</status_path>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 1);
    {
      command& cmd(commands.front());
      char const* cmdline;
      cmdline = MY_PLUGIN_PATH " 0 \"output|metric=100\"";
      cmd.command_line = new char[strlen(cmdline) + 1];
      strcpy(cmd.command_line, cmdline);
    }
    generate_hosts(hosts, HOST_COUNT);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end;
         ++it) {
      it->host_check_command = new char[2];
      strcpy(it->host_check_command, "1");
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it) {
      it->service_check_command = new char[2];
      strcpy(it->service_check_command, "1");
    }
    std::string engine_additional;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_config_path;
      engine_additional = oss.str();
    }

    // Insert entries in index_data;
    {
      QSqlQuery q(db);
      for (unsigned int i(1); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        std::ostringstream query;
        query << "INSERT INTO index_data (host_id, service_id)"
              << "  VALUES(" << (i - 1) / SERVICES_BY_HOST + 1 << ", "
              << i << ")";
        if (!q.exec(query.str().c_str()))
          throw (exceptions::msg() << "cannot create index of service ("
                 << (i - 1) / SERVICES_BY_HOST + 1 << ", " << i << ")");
      }
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      engine_additional.c_str(),
      &hosts,
      &services,
      &commands);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(30 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Get index list.
    std::map<unsigned int, time_t> indexes;
    {
      QSqlQuery q(db);
      if (!q.exec("SELECT id FROM index_data"))
        throw (exceptions::msg() << "cannot get index list: "
               << qPrintable(q.lastError().text()));
      while (q.next())
        indexes[q.value(0).toUInt()];
      if (indexes.size() != HOST_COUNT * SERVICES_BY_HOST)
        throw (exceptions::msg()
               << "not enough entries in index_data: got "
               << indexes.size() << ", expected "
               << HOST_COUNT * SERVICES_BY_HOST);
    }

    // For each index, get the first entry time.
    for (std::map<unsigned int, time_t>::iterator
           it(indexes.begin()),
           end(indexes.end());
         it != end;
         ++it) {
      std::ostringstream file_path;
      file_path << status_path << "/" << it->first << ".rrd";
      rrd_file graph;
      graph.load(file_path.str().c_str());
      if (graph.get_rras().empty() || graph.get_rras().front().empty())
        throw (exceptions::msg() << "not enough data in status graph '"
               << file_path.str().c_str() << "'");
      it->second = graph.get_rras().front().begin() -> first;
    }

    // Get metrics list.
    std::map<unsigned int, time_t> metrics;
    {
      std::ostringstream query;
      query << "SELECT metric_id FROM metrics";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get metric list: "
               << qPrintable(q.lastError().text()));
      while (q.next())
        metrics[q.value(0).toUInt()];
      if (metrics.size() != HOST_COUNT * SERVICES_BY_HOST)
        throw (exceptions::msg()
               << "not enough entries in metrics: got "
               << metrics.size() << ", expected "
               << HOST_COUNT * SERVICES_BY_HOST);
    }

    // For each metric, get the first entry time.
    for (std::map<unsigned int, time_t>::iterator
           it(metrics.begin()),
           end(metrics.end());
         it != end;
         ++it) {
      std::ostringstream file_path;
      file_path << metrics_path << "/" << it->first << ".rrd";
      rrd_file graph;
      graph.load(file_path.str().c_str());
      if (graph.get_rras().empty() || graph.get_rras().front().empty())
        throw (exceptions::msg() << "not enough data in metrics graph '"
               << file_path.str().c_str() << "'");
      it->second = graph.get_rras().front().begin()->first;
    }

    // Graphs must have been recreated from this time.
    time_t recreated_limit(time(NULL));
    std::clog << "recreation limit: " << recreated_limit << std::endl;

    // Launch rebuild.
    {
      QSqlQuery q(db);
      if (!q.exec("UPDATE index_data SET must_be_rebuild='1'"))
        throw (exceptions::msg() << "cannot launch rebuild from DB: "
               << qPrintable(q.lastError().text()));
      sleep_for(15 * MONITORING_ENGINE_INTERVAL_LENGTH);
    }

    // Check that rebuild successfully executed.
    {
      QSqlQuery q(db);
      if (!q.exec("SELECT COUNT(*)"
                  " FROM index_data"
                  " WHERE must_be_rebuild!='0'")
          || !q.next())
        throw (exceptions::msg()
               << "cannot check that rebuild successfully executed");
      if (q.value(0).toUInt())
        throw (exceptions::msg() << "rebuild did not succeed, "
               << q.value(0).toUInt()
               << " indexes are still waiting/being rebuild");
    }

    // Check status graphs.
    for (std::map<unsigned int, time_t>::iterator
           it(indexes.begin()),
           end(indexes.end());
         it != end;
         ++it) {
      // Check file properties.
      std::ostringstream file_path;
      file_path << status_path << "/" << it->first << ".rrd";
      QFileInfo info(file_path.str().c_str());
      if (!info.exists())
        throw (exceptions::msg() << "status file '"
               << file_path.str().c_str() << "' does not exist");
      else if (static_cast<time_t>(info.created().toTime_t())
               < recreated_limit)
        throw (exceptions::msg() << "status file '"
               << file_path.str().c_str() << "' was created at "
               << info.created().toTime_t()
               << " whereas recreation limit is " << recreated_limit);

      // Check file content.
      rrd_file graph;
      graph.load(file_path.str().c_str());
      if (graph.get_rras().empty() || graph.get_rras().front().empty())
        throw (exceptions::msg() << "status file '"
               << file_path.str().c_str()
               << "' does not have any data after rebuild");
      else if (graph.get_rras().front().begin()->first != it->second)
        throw (exceptions::msg()
               << "data time mismatch in status file '"
               << file_path.str().c_str() << "': got "
               << graph.get_rras().front().begin()->first
               << ", expected " << it->second);
    }

    // Check metrics graphs.
    for (std::map<unsigned int, time_t>::iterator
           it(metrics.begin()),
           end(metrics.end());
         it != end;
         ++it) {
      // Check file properties.
      std::ostringstream file_path;
      file_path << metrics_path << "/" << it->first << ".rrd";
      QFileInfo info(file_path.str().c_str());
      if (!info.exists())
        throw (exceptions::msg() << "metric file '"
               << file_path.str().c_str() << "' does not exist");
      else if (static_cast<time_t>(info.created().toTime_t())
               < recreated_limit)
        throw (exceptions::msg() << "metric file '"
               << file_path.str().c_str() << "' was created at "
               << info.created().toTime_t()
               << " whereas recreation limit is " << recreated_limit);

      // Check file content.
      rrd_file graph;
      graph.load(file_path.str().c_str());
      if (graph.get_rras().empty() || graph.get_rras().front().empty())
        throw (exceptions::msg() << "metric file '"
               << file_path.str().c_str()
               << "' does not have any data after rebuild");
      else if (graph.get_rras().front().begin()->first != it->second)
        throw (exceptions::msg()
               << "data time mismatch in metric file '"
               << file_path.str().c_str() << "': got "
               << graph.get_rras().front().begin()->first
               << ", expected " << it->second);
    }

    // Success.
    error = false;
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

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
