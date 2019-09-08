/*
** Copyright 2012-2015 Centreon
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

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_secondary_failovers_to_file"

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
  std::string retention_secondary_file_path(tmpnam(NULL));
  std::string retention_secondary_file2_path(tmpnam(NULL));
  engine_extcmd commander;
  commander.set_file(tmpnam(NULL));
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

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
          << "    <name>SecondaryFailoversToFile-TCP</name>\n"
          << "    <failover>SecondaryFailoversToFile-NodeEvents</failover>\n"
          << "    "
             "<secondary_failover>SecondaryFailoversToFile-File1</"
             "secondary_failover>\n"
          << "    "
             "<secondary_failover>SecondaryFailoversToFile-File2</"
             "secondary_failover>\n"
          << "    <type>tcp</type>\n"
          << "    <host>localhost</host>\n"
          << "    <port>5680</port>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "    <retry_interval>1</retry_interval>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>SecondaryFailoversToFile-NodeEvents</name>\n"
          << "    <type>node_events</type>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>SecondaryFailoversToFile-File1</name>\n"
          << "    <type>file</type>\n"
          << "    <path>" << retention_secondary_file_path << "</path>\n"
          << "    <protocol>bbdo</protocol>\n"
          << "    <compression>yes</compression>\n"
          << "  </output>\n"
          << "  <output>\n"
          << "    <name>SecondaryFailoversToFile-File2</name>\n"
          << "    <type>file</type>\n"
          << "    <path>" << retention_secondary_file2_path << "</path>\n"
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
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << cbmod_config_path;
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(12);

    // Temporary disable checks.
    commander.execute("STOP_EXECUTING_SVC_CHECKS");
    sleep_for(4);

    // Terminate monitoring engine.
    daemon.stop();

    // Check the secondary failovers.
    std::fstream file;
    std::string secondary_file1;
    std::string secondary_file2;
    std::stringstream sstream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(retention_secondary_file_path.c_str());
    sstream.str("");
    sstream << file.rdbuf();
    secondary_file1 = sstream.str();
    file.close();

    file.open(retention_secondary_file2_path.c_str());
    sstream.str("");
    sstream << file.rdbuf();
    secondary_file2 = sstream.str();
    file.close();

    if (secondary_file1.empty() || secondary_file2.empty())
      throw(com::centreon::broker::exceptions::msg()
            << "a retention file was empty");

    if (secondary_file1 != secondary_file2)
      throw(com::centreon::broker::exceptions::msg()
            << "secondary retention files are not the same");

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
  ::remove(retention_secondary_file_path.c_str());
  ::remove(retention_secondary_file2_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
