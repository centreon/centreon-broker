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

#include <sys/stat.h>
#include <unistd.h>
#include <QFile>
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

#define DB_NAME "broker_automatic_retention"
#define HOST_COUNT 5
#define SERVICES_BY_HOST 10

/**
 *  Check that automatic memory retention file exists.
 *
 *  @param[in] cache_dir  Cache directory.
 *
 *  @return True if automatic memory retention file exists.
 */
static bool auto_memory_file_exists(std::string const& cache_dir) {
  std::ostringstream file_path;
  file_path << cache_dir << "/memory-AutomaticRetentionUnitTest";
  return (QFile::exists(file_path.str().c_str()));
}

/**
 *  Check that automatic queue retention file exists.
 *
 *  @param[in] cache_dir  Cache directory.
 *
 *  @return True if automatic queue retention file exists.
 */
static bool auto_queue_file_exists(std::string const& cache_dir) {
  std::ostringstream file_path;
  file_path << cache_dir << "/queue-AutomaticRetentionUnitTest";
  return (QFile::exists(file_path.str().c_str()));
}

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
 *  Check that automatic_retention works.
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
  std::string cache_dir(tmpnam(NULL));
  engine daemon;
  cbd broker;
  test_db db;
  test_file cbmod_cfg;
  test_file cbd_cfg;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Create cache directory.
    if (mkdir(cache_dir.c_str(),
              S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
      throw(exceptions::msg()
            << "cannot create cache directory '" << cache_dir << "'");

    // Write Broker configuration files.
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR
                           "/test/cfg/automatic_retention_1.xml.in");
    cbmod_cfg.set("CACHE_DIRECTORY", cache_dir);
    cbd_cfg.set_template(PROJECT_SOURCE_DIR
                         "/test/cfg/automatic_retention_2.xml.in");
    cbd_cfg.set("CACHE_DIRECTORY", cache_dir);
    cbd_cfg.set("DB_NAME", DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cbmod_cfg.generate();
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // STEP #1.

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(4);

    // Check that automatic retention file exists.
    if (!auto_queue_file_exists(cache_dir))
      throw(exceptions::msg() << "queue file does not exist at step #1");

    // STEP #2.

    // Start Broker daemon.
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    sleep_for(10);

    // Check that automatic retention file does not exist.
    if (auto_queue_file_exists(cache_dir))
      throw(exceptions::msg() << "queue file exists at step #2");

    // Check that instance is up to date.
    if (!instance_is_up_to_date(db, 5 * MONITORING_ENGINE_INTERVAL_LENGTH))
      throw(exceptions::msg() << "instance is not up to date at step #2");

    // STEP #3.

    // Stop Broker daemon.
    broker.stop();
    sleep_for(5);

    // Check that automatic retention file exists.
    if (!auto_queue_file_exists(cache_dir))
      throw(exceptions::msg() << "queue file does not exist at step #3");

    // STEP #4.

    // Stop Centreon Engine.
    daemon.stop();

    // Check that automatic retention file exists.
    if (!auto_queue_file_exists(cache_dir))
      throw(exceptions::msg() << "queue file does not exist at step #4");

    // Check that memory file exists.
    if (!auto_memory_file_exists(cache_dir))
      throw(exceptions::msg() << "memory file does not exist at step #4");

    // STEP #5.

    // Start Broker daemon.
    broker.start();

    // Start Centreon Engine.
    daemon.start();
    sleep_for(10);

    // Check that automatic retention file does not exist."
    if (auto_queue_file_exists(cache_dir))
      throw(exceptions::msg() << "queue file exists at step #5");

    // Check that memory file does not exist.
    if (auto_memory_file_exists(cache_dir))
      throw(exceptions::msg() << "memory file exists at step #5");

    // Check that instance is up to date.
    if (!instance_is_up_to_date(db, 5 * MONITORING_ENGINE_INTERVAL_LENGTH))
      throw(exceptions::msg() << "instance is not up to date at step #5");

    // Stop Centreon Engine.
    daemon.stop();
    sleep_for(2);

    // Stop Centreon Broker.
    broker.stop();

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  recursive_remove(cache_dir);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
