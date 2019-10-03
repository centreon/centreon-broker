/*
** Copyright 2013-2015 Centreon
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

#include <sys/stat.h>
#include <unistd.h>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_delete_graphs"
#define HOST_COUNT 2
#define SERVICES_BY_HOST 5

/**
 *  Check that graphs can be properly deleted.
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
  std::string cbd_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  std::string status_path(tmpnam(NULL));
  engine monitoring;
  cbd broker;
  test_db db;

  // Log.
  std::clog << "status directory: " << status_path << "\n"
            << "metrics directory: " << metrics_path << std::endl;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);
    mkdir(status_path.c_str(), S_IRWXU);

    // Write cbd configuration file.
    {
      std::ofstream ofs;
      ofs.open(cbd_config_path.c_str(),
               std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw(exceptions::msg() << "cannot open cbd configuration file '"
                                << cbd_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR
             "/test/cfg/broker_modules.xml</include>\n"
          << "  <instance>42</instance>\n"
          << "  <instance_name>MyBroker</instance_name>\n"
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
          << "    <name>EngineToTCP</name>\n"
          << "    <type>tcp</type>\n"
          << "    <port>5681</port>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "  </input>\n"
          << "  <output>\n"
          << "    <name>TCPToStorage</name>\n"
          << "    <type>storage</type>\n"
          << "    <db_type>" DB_TYPE "</db_type>\n"
          << "    <db_host>" DB_HOST "</db_host>\n"
          << "    <db_port>" DB_PORT "</db_port>\n"
          << "    <db_user>" DB_USER "</db_user>\n"
          << "    <db_password>" DB_PASSWORD "</db_password>\n"
          << "    <db_name>" DB_NAME "</db_name>\n"
          << "    <queries_per_transaction>0</queries_per_transaction>\n"
          << "    <length>" << 51840 * MONITORING_ENGINE_INTERVAL_LENGTH
          << "</length>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>StorageToRRD</name>\n"
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
         it != end; ++it) {
      it->host_check_command = new char[2];
      strcpy(it->host_check_command, "1");
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it) {
      it->service_check_command = new char[2];
      strcpy(it->service_check_command, "1");
    }
    std::string engine_additional;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR "/test/cfg/delete_graphs.xml";
      engine_additional = oss.str();
    }

    // Insert entries in index_data.
    {
      QSqlQuery q(*db.centreon_db());
      for (uint32_t i(1); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        uint32_t host_id((i - 1) / SERVICES_BY_HOST + 1);
        std::ostringstream query;
        query << "INSERT INTO rt_index_data (host_id, service_id)"
              << "  VALUES(" << host_id << ", " << i << ")";
        if (!q.exec(query.str().c_str()))
          throw(exceptions::msg() << "cannot create index of service ("
                                  << host_id << ", " << i << ")");
      }
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), engine_additional.c_str(), &hosts,
                 &services, &commands);

    // Start cbd.
    broker.set_config_file(cbd_config_path);
    broker.start();
    sleep_for(2);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(25);

    // Check and get index_data entries.
    std::map<std::pair<unsigned long, unsigned long>, unsigned long> indexes;
    {
      QSqlQuery q(*db.centreon_db());
      std::string query(
          "SELECT host_id, service_id, index_id"
          "  FROM rt_index_data"
          "  ORDER BY host_id, service_id");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg()
              << "cannot get index list: " << qPrintable(q.lastError().text()));
      for (unsigned long i(0); i < HOST_COUNT; ++i)
        for (unsigned long j(0); j < SERVICES_BY_HOST; ++j) {
          unsigned long host_id(i + 1);
          unsigned long service_id(i * SERVICES_BY_HOST + j + 1);
          if (!q.next())
            throw(exceptions::msg() << "not enough index entries in DB");
          else if ((q.value(0).toUInt() != host_id) ||
                   (q.value(1).toUInt() != service_id))
            throw(exceptions::msg()
                  << "index entry mismatch: got (" << q.value(0).toUInt()
                  << ", " << q.value(1).toUInt() << "), expected (" << host_id
                  << ", " << service_id << ")");
          indexes[std::make_pair(host_id, service_id)] = q.value(2).toUInt();
        }
    }

    // Get metrics entries.
    std::map<unsigned long, std::list<unsigned long> > metrics;
    {
      QSqlQuery q(*db.centreon_db());
      std::string query("SELECT index_id, metric_id FROM rt_metrics");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg() << "cannot get metrics list: "
                                << qPrintable(q.lastError().text()));
      while (q.next())
        metrics[q.value(0).toUInt()].push_back(q.value(1).toUInt());
    }

    // Check index_data and metrics entries along with their RRD files.
    {
      if (indexes.size() != metrics.size())
        throw(exceptions::msg()
              << "size mismatch between index_data and metrics: got "
              << indexes.size() << " indexes for " << metrics.size()
              << " referenced indexes");
      for (std::map<unsigned long, std::list<unsigned long> >::const_iterator
               it1(metrics.begin()),
           end1(metrics.end());
           it1 != end1; ++it1) {
        if (it1->second.size() != 1)
          throw(exceptions::msg()
                << "too much metrics for index " << it1->first << ": got "
                << it1->second.size() << ", expected 1");
        {
          std::ostringstream status_file;
          status_file << status_path << "/" << it1->first << ".rrd";
          if (access(status_file.str().c_str(), F_OK))
            throw(exceptions::msg()
                  << "status RRD file '" << status_file.str().c_str()
                  << "' does not exist");
        }
        {
          std::ostringstream metrics_file;
          metrics_file << metrics_path << "/" << it1->second.front() << ".rrd";
          if (access(metrics_file.str().c_str(), F_OK))
            throw(exceptions::msg()
                  << "metrics RRD file '" << metrics_file.str().c_str()
                  << "' does not exist");
        }
      }
    }

    // Terminate monitoring engine (no more performance data).
    monitoring.stop();
    sleep_for(2);

    // Flag an index to delete.
    {
      QSqlQuery q(*db.centreon_db());
      std::string query(
          "UPDATE rt_index_data"
          "  SET to_delete=1"
          "  WHERE host_id=2");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg() << "cannot flag index_data to delete: "
                                << qPrintable(q.lastError().text()));
    }

    // Flag metrics to delete.
    {
      QSqlQuery q(*db.centreon_db());
      std::string query(
          "UPDATE rt_metrics AS m JOIN rt_index_data AS i"
          "  ON m.index_id=i.index_id"
          "  SET m.to_delete=1"
          "  WHERE i.host_id=1 AND i.service_id<>1");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg() << "cannot flag metrics to delete: "
                                << qPrintable(q.lastError().text()));
    }

    // Signal entries to delete to cbd and wait a little.
    broker.update();
    sleep_for(7);

    // Terminate cbd.
    broker.stop();
    sleep_for(2);

    // Check that only one entry remains in index_data (1, 1).
    {
      QSqlQuery q(*db.centreon_db());
      std::string query("SELECT host_id, service_id FROM rt_index_data");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg() << "cannot read index_data: "
                                << qPrintable(q.lastError().text()));
      for (unsigned long i(0); i < SERVICES_BY_HOST; ++i) {
        if (!q.next())
          throw(exceptions::msg()
                << "not enough entries in index_data after deletion: "
                << "got " << i << ", expected " << SERVICES_BY_HOST);
        else if ((q.value(0).toUInt() != 1) || (q.value(1).toUInt() != (i + 1)))
          throw(exceptions::msg()
                << "last index_data entry is (" << q.value(0).toUInt() << ", "
                << q.value(1).toUInt() << "), expected (1, " << (i + 1) << ")");
      }
      if (q.next())
        throw(exceptions::msg()
              << "too much index_data entries after deletion");
    }

    // Check that only one entry remains in metrics.
    {
      QSqlQuery q(*db.centreon_db());
      std::string query(
          "SELECT i.host_id, i.service_id"
          "  FROM rt_metrics AS m JOIN rt_index_data AS i"
          "  ON m.index_id=i.index_id");
      if (!q.exec(query.c_str()))
        throw(exceptions::msg()
              << "cannot read metrics: " << qPrintable(q.lastError().text()));
      else if (!q.next())
        throw(exceptions::msg()
              << "no entry remains in index_data after deletion");
      else if ((q.value(0).toUInt() != 1) || (q.value(1).toUInt() != 1))
        throw(exceptions::msg()
              << "last metrics entry is for service (" << q.value(0).toUInt()
              << ", " << q.value(1).toUInt() << "), expected (1, 1)");
      else if (q.next())
        throw(exceptions::msg() << "too much metrics entries after deletion");
    }

    // Remove entries that are present.
    metrics.erase(indexes[std::make_pair(1ul, 1ul)]);
    for (unsigned long i(1); i <= SERVICES_BY_HOST; ++i)
      indexes.erase(std::make_pair(1ul, i));

    // Check that deleted indexes got their RRD files deleted.
    for (std::map<std::pair<unsigned long, unsigned long>,
                  unsigned long>::const_iterator it(indexes.begin()),
         end(indexes.end());
         it != end; ++it) {
      std::ostringstream status_file;
      status_file << status_path << "/" << it->second << ".rrd";
      if (!access(status_file.str().c_str(), F_OK))
        throw(exceptions::msg()
              << "status RRD file '" << status_file.str().c_str()
              << "' still exists");
    }

    // Check that deleted metrics got their RRD files deleted.
    for (std::map<unsigned long, std::list<unsigned long> >::const_iterator
             it(metrics.begin()),
         end(metrics.end());
         it != end; ++it) {
      {
        std::ostringstream metrics_file;
        metrics_file << metrics_path << "/" << it->second.front() << ".rrd";
        if (!access(metrics_file.str().c_str(), F_OK))
          throw(exceptions::msg()
                << "metrics RRD file '" << metrics_file.str().c_str()
                << "' still exists");
      }
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  monitoring.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  ::remove(cbd_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_commands(commands);
  recursive_remove(metrics_path);
  recursive_remove(status_path);

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
