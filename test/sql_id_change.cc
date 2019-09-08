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
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_sql_id_change"

/**
 *  Check that broker correctly manage id changes.
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
    db.set_remove_db_on_close(false);

    // Create the config xml file.
    test_file file;
    file.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql_id_change.xml.in");
    file.set("DB_NAME", DB_NAME);
    std::string config_file = file.generate();

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 1);
    generate_services(services, hosts, 1);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << config_file;
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(5);

    //
    // #1 ID change.
    //
    // Simulate a service deletion followed by a service recreation
    // with the same host / description.
    //

    // Change the service ID.
    set_custom_variable(services.back(), "SERVICE_ID", "42");
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Reload daemon.
    daemon.reload();
    sleep_for(4);

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
               "  FROM rt_services"
               "  WHERE enabled = '1'";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg()
              << "cannot read service count from DB at check #1: "
              << q.lastError().text());
      if (!q.next() || (q.value(0).toUInt() != 1) || q.next())
        throw(exceptions::msg() << "invalid service count at check #1: got "
                                << q.value(0).toUInt() << ", expected 1");
    }

    //
    // #2 Name change.
    //
    // Simulate a service renaming.
    //

    // Rename service.
    delete[] services.back().description;
    services.back().description = NULL;
    services.back().description = ::strdup("42");

    // Write configuration.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Reload daemon.
    daemon.reload();
    sleep_for(4);

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
               "  FROM rt_services "
               "  WHERE enabled = '1'";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg()
              << "cannot read service count from DB at check #2: "
              << q.lastError().text());
      if (!q.next() || (q.value(0).toUInt() != 1) || q.next())
        throw(exceptions::msg() << "invalid service count at check #2: got "
                                << q.value(0).toUInt() << ", expected 1");
    }

    //
    // #3 Renaming and new service with previous name.
    //
    // Simulate a service renaming followed by the creation of a new
    // service that has the original name of the service.
    //

    // Rename service.
    delete[] services.back().description;
    services.back().description = NULL;
    services.back().description = ::strdup("84");

    // Create new service.
    generate_services(services, hosts, 1);
    delete[] services.back().description;
    services.back().description = NULL;
    services.back().description = ::strdup("42");
    set_custom_variable(services.back(), "SERVICE_ID", "2");

    // Write configuration.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Reload daemon.
    daemon.reload();
    sleep_for(4);

    // Check service count.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
               "  FROM rt_services "
               "  WHERE enabled = '1'";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg()
              << "cannot read service count from DB at check #3: "
              << q.lastError().text());
      if (!q.next() || (q.value(0).toUInt() != 2) || q.next())
        throw(exceptions::msg() << "invalid service count at check #3: got "
                                << q.value(0).toUInt() << ", expected 2");
    }

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    db.set_remove_db_on_close(false);
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    db.set_remove_db_on_close(false);
  }

  // Cleanup.
  daemon.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
