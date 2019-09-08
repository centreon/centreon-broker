/*
** Copyright 2014-2015 Centreon
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
#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextCodec>
#include <QVariant>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/rrd_file.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_stats_events"
#define INSTANCE_ID "42"

/**
 *  Check that statistics events are properly generated and write
 *  associated RRD files.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Return value.
  int retval(EXIT_FAILURE);

  // Initialize Qt.
  QCoreApplication app(argc, argv);
  QTextCodec* utf8_codec(QTextCodec::codecForName("UTF-8"));
  if (utf8_codec)
    QTextCodec::setCodecForCStrings(utf8_codec);
  else
    std::cout << "could not set the UTF-8 codec\n";

  // Variables that need cleaning.
  std::string cbmod_config_path(tmpnam(NULL));
  std::string engine_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  engine daemon;
  test_db db;

  // Log.
  std::clog << "metrics directory: " << metrics_path << std::endl;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);

    // Write cbmod configuration file.
    {
      std::ofstream ofs;
      ofs.open(cbmod_config_path.c_str(),
               std::ios_base::out | std::ios_base::trunc);
      if (ofs.fail())
        throw(exceptions::msg() << "cannot open cbmod configuration file '"
                                << cbmod_config_path.c_str() << "'");
      ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
          << "<centreonbroker>\n"
          << "  <include>" PROJECT_SOURCE_DIR
             "/test/cfg/broker_modules.xml</include>\n"
          << "  <instance>" << INSTANCE_ID << "</instance>\n"
          << "  <instance_name>MyBroker</instance_name>\n"
          // << "  <!--\n"
          << "  <logger>\n"
          << "    <type>file</type>\n"
          << "    <name>cbmod.log</name>\n"
          << "    <config>1</config>\n"
          << "    <debug>1</debug>\n"
          << "    <error>1</error>\n"
          << "    <info>1</info>\n"
          << "    <level>3</level>\n"
          << "  </logger>\n"
          // << "  -->\n"
          << "  <stats>\n"
          << "    <remote>\n"
          << "      <dumper_tag>CentralBroker</dumper_tag>\n"
          << "      <metrics>\n"
          << "        <host>1</host>\n"
          << "        <service>\n"
          << "          <id>1</id>\n"
          << "          <name>active_host_execution_time</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>2</id>\n"
          << "          <name>active_host_latency</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>3</id>\n"
          << "          <name>active_hosts_last</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>4</id>\n"
          << "          <name>active_host_state_change</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>5</id>\n"
          << "          <name>active_service_execution_time</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>6</id>\n"
          << "          <name>active_service_latency</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>7</id>\n"
          << "          <name>active_services_last</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>8</id>\n"
          << "          <name>active_service_state_change</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>9</id>\n"
          << "          <name>command_buffers</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>10</id>\n"
          << "          <name>hosts_actively_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>11</id>\n"
          << "          <name>hosts_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>12</id>\n"
          << "          <name>hosts_flapping</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>13</id>\n"
          << "          <name>hosts</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>14</id>\n"
          << "          <name>hosts_in_downtime</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>15</id>\n"
          << "          <name>hosts_passively_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>16</id>\n"
          << "          <name>hosts_scheduled</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>17</id>\n"
          << "          <name>passive_host_latency</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>18</id>\n"
          << "          <name>passive_hosts_last</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>19</id>\n"
          << "          <name>passive_host_state_change</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>20</id>\n"
          << "          <name>passive_service_latency</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>21</id>\n"
          << "          <name>passive_services_last</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>22</id>\n"
          << "          <name>passive_service_state_change</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>23</id>\n"
          << "          <name>services_actively_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>24</id>\n"
          << "          <name>services_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>25</id>\n"
          << "          <name>services_flapping</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>26</id>\n"
          << "          <name>services</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>27</id>\n"
          << "          <name>services_in_downtime</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>28</id>\n"
          << "          <name>services_passively_checked</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>29</id>\n"
          << "          <name>services_scheduled</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>30</id>\n"
          << "          <name>total_hosts</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>31</id>\n"
          << "          <name>total_host_state_change</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>32</id>\n"
          << "          <name>total_services</name>\n"
          << "        </service>\n"
          << "        <service>\n"
          << "          <id>33</id>\n"
          << "          <name>total_service_state_change</name>\n"
          << "        </service>\n"
          << "      </metrics>\n"
          << "      <interval>" << MONITORING_ENGINE_INTERVAL_LENGTH
          << "</interval>\n"
          << "    </remote>\n"
          << "  </stats>\n"
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
          << "    <length>86400</length>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>StorageToRRDUnitTest</name>\n"
          << "    <type>rrd</type>\n"
          << "    <metrics_path>" << metrics_path << "</metrics_path>\n"
          << "    <write_status>no</write_status>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <type>dumper</type>\n"
          << "    <tagname>CentralBroker</tagname>\n"
          << "    <path>" << metrics_path << "/$INSTANCEID$.stats"
          << "</path>\n"
          << "  </output>\n"
          << "</centreonbroker>\n";
      ofs.close();
    }

    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_config_path << "\n";
      additional_config = oss.str();
    }

    // Insert entries in index_data.
    {
      QSqlQuery q(*db.centreon_db());
      for (unsigned int i(1); i <= 33; ++i) {
        std::ostringstream query;
        query << "INSERT INTO rt_index_data (host_id, service_id)"
              << "  VALUES (" << 1 << ", " << i << ")";
        if (!q.exec(query.str().c_str()))
          throw(exceptions::msg()
                << "cannot create index of service (" << 1 << ", " << i << ")");
      }
    }

    // Get index list.
    std::list<unsigned int> indexes;
    {
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT index_id FROM rt_index_data ORDER BY service_id ASC"))
        throw(exceptions::msg()
              << "cannot get index list: " << qPrintable(q.lastError().text()));
      while (q.next())
        indexes.push_back(q.value(0).toUInt());
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str());

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(15);

    // Check metrics table.
    std::list<unsigned int> metrics;
    {
      std::ostringstream query;
      query << "SELECT metric_id"
            << "  FROM rt_metrics";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot get metric list: "
                                << qPrintable(q.lastError().text()));
      while (q.next())
        metrics.push_back(q.value(0).toUInt());
      if (metrics.size() !=
          64)  // XXX: this should be 75 if passive hosts/services are available
        throw(exceptions::msg() << "invalid metrics count (got "
                                << metrics.size() << ", expected 64)");
    }

    // Check data_bin table.
    {
      for (std::list<unsigned int>::const_iterator it(metrics.begin()),
           end(metrics.end());
           it != end; ++it) {
        std::ostringstream query;
        query << "SELECT COUNT(*)"
              << "  FROM log_data_bin"
              << "  WHERE id_metric=" << *it;
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query.str().c_str()) || !q.next() || !q.value(0).toUInt())
          throw(exceptions::msg() << "data_bin is invalid for metric " << *it
                                  << ": " << qPrintable(q.lastError().text()));
      }
    }

    // Check that metrics RRD files exist.
    for (std::list<unsigned int>::const_iterator it(metrics.begin()),
         end(metrics.end());
         it != end; ++it) {
      std::ostringstream path;
      path << metrics_path << "/" << *it << ".rrd";
      if (access(path.str().c_str(), F_OK))
        throw(exceptions::msg() << "metrics RRD file '" << path.str().c_str()
                                << "' does not exist");
      rrd_file f;
      f.load(path.str().c_str());

      // Check content.
      if (f.get_rras().size() != 2)
        throw(exceptions::msg() << "metrics RRD file '" << path.str().c_str()
                                << "' does not have two RRAs");
      if (f.get_rras().front().size() < 2)
        throw(exceptions::msg()
              << "metrics RRD file '" << path.str().c_str()
              << "' does not have enough entries in its first RRA");
    }

    // Check data from dumper.
    {
      std::string path(metrics_path + "/" INSTANCE_ID ".stats");
      std::ifstream ifs(path.c_str());
      if (ifs.fail() || ifs.eof())
        throw(exceptions::msg()
              << "cannot open statistics file '" << path << "'");
      if (!ifs.get() || !ifs.good())
        throw(exceptions::msg() << "the statistics file is empty");
    }

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  config_remove(engine_config_path.c_str());
  ::remove(cbmod_config_path.c_str());
  recursive_remove(metrics_path);

  return (retval);
}
