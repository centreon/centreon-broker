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

#include <cstdio>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

static char const* valgrind_arg = "--tool=helgrind";
static char const* db_name = "CENTREON_BROKER_TEST_HELGRIND";
static unsigned int service_number = 5;
static unsigned int host_number = 5;
static unsigned int command_number = 10;

using namespace com::centreon::broker;

/**
 *  @brief Check that helgrind does not return a warning.
 *
 *  We launch centreon broker into a the valgrind tool called 'helgrind'.
 *  This tool is used to detect various multi-threading defects, like
 *  potential deadlock and un-locked access of the same memory by two threads.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::list<command> commands;
  std::string engine_config_path(tmpnam(NULL));
  engine monitoring;
  QProcess broker;
  test_file cbmod_cfg;
  test_file broker_cfg;
  test_db db;

  try {
    // Prepare database.
    db.open(db_name, NULL, true);
    generate_commands(commands, command_number);
    generate_hosts(hosts, host_number);
    generate_services(services, hosts, service_number);

    // Launch broker.
    broker_cfg.set_template("/test/cfg/helgrind_1.xml.in");
    QStringList args;
    args.push_back(valgrind_arg);
    args.push_back(CBD_PATH);
    args.push_back(broker_cfg.generate().c_str());
    broker.start("valgrind", args);
    if (!broker.waitForStarted())
      throw(exceptions::msg()
            << "couldn't start valgrind: " << broker.errorString());

    // Generate configuration.
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/helgrind_1.xml.in");
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_cfg.generate()
          << "\n";
      additional_config = oss.str();
    }
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services, &commands);

    // Start monitoring.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    // Wait.
    sleep_for(30);

    // Check for warning.
    QByteArray stderr = broker.readAllStandardError();

    if (!stderr.isEmpty())
      throw(exceptions::msg() << "got helgrind warnings: " << QString(stderr));
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    error = true;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    error = true;
  }

  // Cleanup.
  monitoring.stop();
  broker.kill();
  broker.waitForFinished();
  sleep_for(3);
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_commands(commands);

  return (error ? -1 : 0);
}
