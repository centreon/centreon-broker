/*
** Copyright 2012-2013 Centreon
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
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  Generate a configuration file with.
 */
static void generate_cbd_conf(std::string const& cbd_config_path,
                              uint32_t services,
                              uint32_t requests_per_service,
                              std::string const& metrics_path,
                              std::string const& status_path) {
  std::ofstream ofs;
  ofs.open(cbd_config_path.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg()
          << "cannot open cbd configuration file '" << cbd_config_path << "'");
  ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
      << "<centreonbroker>\n"
      << "  <include>" PROJECT_SOURCE_DIR
         "/test/cfg/broker_modules.xml</include>\n"
      << "  <module>" BENCH_GENERATE_RRD_MOD_PATH "</module>\n"
      << "  <instance>42</instance>\n"
      << "  <instance_name>MyBroker</instance_name>\n"
      << "  <bench_services>" << services << "</bench_services>\n"
      << "  <bench_requests_per_service>" << requests_per_service
      << "</bench_requests_per_service>\n"
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
      << "  <output>\n"
      << "    <name>ToRRD</name>\n"
      << "    <type>rrd</type>\n"
      << "    <metrics_path>" << metrics_path << "</metrics_path>\n"
      << "    <status_path>" << status_path << "</status_path>\n"
      << "  </output>\n"
      << "</centreonbroker>\n";
  ofs.close();
  return;
}

/**
 *  Benchmark RRD file creation and update.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::string cbd_config_path(tmpnam(NULL));
  std::string metrics_path(tmpnam(NULL));
  std::string status_path(tmpnam(NULL));
  cbd broker;

  try {
    // Check arguments.
    if (argc < 3)
      throw(exceptions::msg()
            << "USAGE: " << argv[0] << " <metrics> <updates_per_metric>");

    // Create RRD paths.
    mkdir(metrics_path.c_str(), S_IRWXU);
    mkdir(status_path.c_str(), S_IRWXU);

    // Write cbd configuration file for creation.
    generate_cbd_conf(cbd_config_path, strtoul(argv[1], NULL, 0), 1,
                      metrics_path, status_path);

    // T1.
    timeval tv1;
    gettimeofday(&tv1, NULL);

    // Start Broker daemon.
    broker.set_config_file(cbd_config_path);
    broker.start();
    broker.wait();

    // T2.
    timeval tv2;
    gettimeofday(&tv2, NULL);

    // Write cbd configuration file for update.
    generate_cbd_conf(cbd_config_path, strtoul(argv[1], NULL, 0),
                      strtoul(argv[2], NULL, 0), metrics_path, status_path);

    // T3.
    timeval tv3;
    gettimeofday(&tv3, NULL);

    // Restart Broker daemon.
    broker.start();
    broker.wait();

    // T4.
    timeval tv4;
    gettimeofday(&tv4, NULL);

    // Print timing information.
    timeval creation_time;
    timersub(&tv2, &tv1, &creation_time);
    timeval update_time;
    timersub(&tv4, &tv3, &update_time);
    std::cout << "creation time: "
              << creation_time.tv_sec + creation_time.tv_usec / 1000000.0
              << "\n"
              << "update time: "
              << update_time.tv_sec + update_time.tv_usec / 1000000.0
              << std::endl;

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();
  ::remove(cbd_config_path.c_str());
  recursive_remove(metrics_path);
  recursive_remove(status_path);

  return (retval);
}
