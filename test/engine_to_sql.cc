/*
** Copyright 2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <iostream>
#include <QTemporaryFile>
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/vars.hh"

#define DB_NAME "broker_engine_to_sql"

/**
 *  Check that monitoring is properly inserted in SQL database.
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

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    std::string cbmod_loading;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/engine_to_sql.xml\n";
      cbmod_loading = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      cbmod_loading.c_str(),
      &hosts,
      &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep(20);
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  //config_remove(engine_config_path.c_str());
  //config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
