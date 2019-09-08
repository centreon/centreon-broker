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
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_failover_to_tcp"

/**
 *  Check that instance last_alive is up to date.
 *
 *  @param[in] db     Database object.
 *  @param[in] limit  Time limit in the past before now.
 *
 *  @return True if instance is up to date.
 */
static bool instance_is_up_to_date(test_db& db, int limit) {
  time_t min_valid(time(NULL) - limit);
  QSqlQuery q(*db.centreon_db());
  return (q.exec("SELECT last_alive"
                 "  FROM rt_instances"
                 "  WHERE instance_id=42") &&
          q.next() && (q.value(0).toLongLong() >= min_valid));
}

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
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  cbd broker1;
  cbd broker2;
  test_db db;
  test_file cbmod_cfg;
  test_file cbd_cfg1;
  test_file cbd_cfg2;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Write Broker configuration files.
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR
                           "/test/cfg/failover_to_tcp_1.xml.in");
    cbd_cfg1.set_template(PROJECT_SOURCE_DIR
                          "/test/cfg/failover_to_tcp_2.xml.in");
    cbd_cfg1.set("INSTANCE_ID", "43");
    cbd_cfg1.set("INSTANCE_NAME", "my_cbd1");
    cbd_cfg1.set("PORT", "5680");
    cbd_cfg2.set_template(PROJECT_SOURCE_DIR
                          "/test/cfg/failover_to_tcp_2.xml.in");
    cbd_cfg2.set("INSTANCE_ID", "44");
    cbd_cfg2.set("INSTANCE_NAME", "my_cbd2");
    cbd_cfg2.set("PORT", "5690");

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_cfg.generate();
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // STEP #1.
    std::cout << "step #1\n";

    // Launch broker1.
    broker1.set_config_file(cbd_cfg1.generate());
    broker1.start();
    sleep_for(3);

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(10);

    // Check that instance is up to date.
    if (!instance_is_up_to_date(db, 10))
      throw(exceptions::msg() << "instance is not up to date at step #1");

    // STEP #2.
    std::cout << "step #2\n";

    // Stop broker1.
    broker1.stop();

    // Start broker2.
    broker2.set_config_file(cbd_cfg2.generate());
    broker2.start();
    sleep_for(12);

    // Check that instance is up to date.
    if (!instance_is_up_to_date(db, 10))
      throw(exceptions::msg() << "instance is not up to date at step #2");

    // STEP #3.
    std::cout << "step #3\n";

    // Start broker1.
    broker1.start();

    // Stop broker2.
    broker2.stop();
    sleep_for(12);

    // Check that instance is up to date.
    if (!instance_is_up_to_date(db, 10))
      throw(exceptions::msg() << "instance is not up to date at step #3");

    // Terminate monitoring engine.
    daemon.stop();
    sleep_for(2);

    // Terminate Broker daemons.
    broker1.stop();

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  broker1.stop();
  broker2.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
