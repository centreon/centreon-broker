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

#define DB_NAME "broker_sql_instance_update_outdated"
#define INSTANCE_TIMEOUT 30
#define INSTANCE_TIMEOUT_STR "30"

/**
 *  Check that instance outdated status and
 *  service/host status are properly updated.
 *
 *  @return EXIT_SUCCESS on success.
 **/

int main() {
  int retval(EXIT_FAILURE);

  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  cbd broker;
  test_db db;
  test_file cfg_cbmod;
  test_file cfg_cbd;

  try {
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    cfg_cbmod.set_template(PROJECT_SOURCE_DIR
                           "/test/cfg/sql_instance_update_outdated_1.xml.in");
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << cfg_cbmod.generate()
          << "\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), cbmod_loading.c_str(), &hosts,
                 &services);

    // Start Broker daemon.
    cfg_cbd.set_template(PROJECT_SOURCE_DIR
                         "/test/cfg/sql_instance_update_outdated_2.xml.in");
    cfg_cbd.set("INSTANCE_TIMEOUT", INSTANCE_TIMEOUT_STR);
    broker.set_config_file(cfg_cbd.generate());
    broker.start();
    sleep_for(2);
    broker.update();

    // Start engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    broker.update();
    sleep_for(INSTANCE_TIMEOUT + 2);

    // Terminate monitoring engine.
    daemon.stop();

    sleep_for(INSTANCE_TIMEOUT + 2);
    broker.update();

    // Check for outdated instance
    {
      std::ostringstream query;
      query << "SELECT COUNT(instance_id) from rt_instances where outdated = "
               "TRUE";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check outdated instances from DB: "
                                << q.lastError().text().toStdString().c_str());
      if (!q.next() || (q.value(0).toUInt() != 1) || q.next())
        throw(exceptions::msg() << "instance not outdated");
    }

    // Check for outdated services.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM rt_services WHERE state = " << STATE_UNKNOWN;
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check outdated services from DB: "
                                << q.lastError().text().toStdString().c_str());

      if (!q.next() || (q.value(0).toUInt() != 50) || q.next())
        throw(exceptions::msg() << "services not outdated");
    }

    // Check for outdated hosts.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM rt_hosts WHERE state = " << HOST_UNREACHABLE;
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check outdated hosts from DB: "
                                << q.lastError().text().toStdString().c_str());

      if (!q.next() || (q.value(0).toUInt() != 10) || q.next())
        throw(exceptions::msg() << "hosts not outdated");
    }

    daemon.start();
    sleep_for(3);

    // Check for living instance
    {
      std::ostringstream query;
      query << "SELECT COUNT(instance_id) FROM rt_instances WHERE outdated=0";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check living instances from DB: "
                                << q.lastError().text().toStdString().c_str());
      if (!q.next() || (q.value(0).toUInt() != 1) || q.next())
        throw(exceptions::msg() << "living instance not updated");
    }

    // Check for living services.
    {
      std::ostringstream query;
      query << "SELECT COUNT(service_id)"
            << "  FROM rt_services WHERE state != " << STATE_UNKNOWN;
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check living services from DB: "
                                << q.lastError().text().toStdString().c_str());

      if (!q.next() || (q.value(0).toUInt() != 50) || q.next())
        throw(exceptions::msg() << "living services not updated");
    }

    // Check for living hosts.
    {
      std::ostringstream query;
      query << "SELECT COUNT(host_id)"
            << "  FROM rt_hosts WHERE state != " << HOST_UNREACHABLE;
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query.str().c_str()))
        throw(exceptions::msg() << "cannot check living hosts from DB: "
                                << q.lastError().text().toStdString().c_str());

      if (!q.next() || (q.value(0).toUInt() != 10) || q.next())
        throw(exceptions::msg() << "living hosts not updated");
    }

    // Success
    retval = EXIT_SUCCESS;

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    db.set_remove_db_on_close(false);
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    db.set_remove_db_on_close(false);
  }

  // cleanup
  daemon.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return retval;
}
