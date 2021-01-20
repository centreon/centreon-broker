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
#include <iostream>
#include <sstream>
#include "com/centreon/exceptions/msg_fmt.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

#define DB_NAME "broker_tls_to_sql"

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
  cbd broker;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << PROJECT_SOURCE_DIR
          << "/test/cfg/tls_to_sql_1.xml\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Start Broker daemon.
    broker.set_config_file(PROJECT_SOURCE_DIR "/test/cfg/tls_to_sql_2.xml");
    broker.start();
    sleep_for(2);
    broker.update();

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(19);

    // Terminate monitoring engine.
    daemon.stop();

    // Terminate Broker daemon.
    broker.stop();

    // Check host count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM rt_hosts";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw msg_fmt("cannot read host count from DB: {}",
                      q.lastError().text().toStdString().c_str());
      if (!q.next() || (q.value(0).toUInt() != 10) || q.next())
        throw msg_fmt("invalid host count");
    }

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM rt_services";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw msg_fmt("cannot read service count from DB: {}",
                      q.lastError().text().toStdString().c_str());
      if (!q.next() || (q.value(0).toUInt() != 50) || q.next())
        throw msg_fmt("invalid service count");
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
  broker.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
