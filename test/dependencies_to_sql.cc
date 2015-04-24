/*
** Copyright 2013-2015 Merethis
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include <string>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_dependencies_to_sql"
#define HOST_COUNT 6
#define SERVICES_BY_HOST 1

/**
 *  Check that dependencies are properly inserted in SQL database.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::list<hostdependency> host_deps;
  std::list<servicedependency> service_deps;
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    generate_host_dependencies(host_deps, HOST_COUNT - 2);
    generate_service_dependencies(
      service_deps,
      HOST_COUNT * SERVICES_BY_HOST - 2);
    {
      std::list<hostdependency>::iterator dep(host_deps.begin());
      std::list<host>::iterator
        next(hosts.begin()),
        current(next++),
        previous,
        end(hosts.end());
      ++next;
      while (next != end) {
        previous = current;
        current = next;
        depends_on(*(dep++), *previous, *current);
        ++(++next);
      }
      next = ++hosts.begin();
      current = next++;
      while (next != end) {
        previous = current;
        current = ++next;
        depends_on(*(dep++), *previous, *current);
        ++next;
      }
    }
    {
      for (std::list<host>::iterator
             next(hosts.begin()),
             previous(next++),
             end(hosts.end());
           next != end;
           ) {
        parent_of(*previous, *next);
        previous = next++;
      }
    }
    {
      std::list<servicedependency>::iterator dep(service_deps.begin());
      std::list<service>::iterator
        next(services.begin()),
        current(next++),
        previous,
        end(services.end());
      ++next;
      while (next != end) {
        previous = current;
        current = next;
        depends_on(*(dep++), *previous, *current);
        ++(++next);
      }
      next = ++services.begin();
      current = next++;
      while (next != end) {
        previous = current;
        current = ++next;
        depends_on(*(dep++), *previous, *current);
        ++next;
      }
    }
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " " << PROJECT_SOURCE_DIR
          << "/test/cfg/dependencies_to_sql.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration file.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      NULL,
      &host_deps,
      &service_deps);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(16);

    // Check the 'hosts_hosts_dependencies' table.
    {
      struct {
        unsigned int dependent_host_id;
        unsigned int host_id;
      } const entries[] = {
        { 1, 3 }, { 2, 4 }, { 3, 5 }, { 4, 6 }
      };
      std::ostringstream query;
      query << "SELECT dependent_host_id, host_id"
            << "  FROM rt_hosts_hosts_dependencies"
            << "  ORDER BY dependent_host_id, host_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot read host dependencies from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host dependencies: got " << i
                 << ", expected "
                 << sizeof(entries) / sizeof(*entries));
        if ((q.value(0).toUInt() != entries[i].dependent_host_id)
            || (q.value(1).toUInt() != entries[i].host_id))
          throw (exceptions::msg() << "invalid host dependency entry: "
                 << "got (dependent host id " << q.value(0).toUInt()
                 << ", host id " << q.value(1).toUInt()
                 << "), expected (" << entries[i].dependent_host_id
                 << ", " << entries[i].host_id << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much host dependencies");
    }

    // Check the 'services_services_dependencies' table.
    {
      struct {
        unsigned int dependent_host_id;
        unsigned int dependent_service_id;
        unsigned int host_id;
        unsigned int service_id;
      } const entries[] = {
        { 1, 1, 3, 3 }, { 2, 2, 4, 4 }, { 3, 3, 5, 5 }, { 4, 4, 6, 6 }
      };
      std::ostringstream query;
      query << "SELECT dependent_host_id, dependent_service_id,"
            << "       host_id, service_id"
            << "  FROM rt_services_services_dependencies"
            << "  ORDER BY dependent_host_id, dependent_service_id,"
            << "           host_id, service_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot read service dependencies from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough service dependencies: got " << i
                 << ", expected "
                 << sizeof(entries) / sizeof(*entries));
        if ((q.value(0).toUInt() != entries[i].dependent_host_id)
            || (q.value(1).toUInt() != entries[i].dependent_service_id)
            || (q.value(2).toUInt() != entries[i].host_id)
            || (q.value(3).toUInt() != entries[i].service_id))
          throw (exceptions::msg() << "invalid service dependency "
                 << "entry: got (dependent host id "
                 << q.value(0).toUInt() << ", dependent service id "
                 << q.value(1).toUInt() << ", host id "
                 << q.value(2).toUInt() << ", service id "
                 << q.value(3).toUInt() << "), expected ("
                 << entries[i].dependent_host_id << ", "
                 << entries[i].dependent_service_id << ", "
                 << entries[i].host_id << ", " << entries[i].service_id
                 << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much service dependencies");
    }

    // Check the 'hosts_hosts_parents' table.
    {
      struct {
        unsigned int child_id;
        unsigned int parent_id;
      } const entries[] = {
        { 2, 1 },
        { 3, 2 },
        { 4, 3 },
        { 5, 4 },
        { 6, 5 }
      };
      std::ostringstream query;
      query << "SELECT child_id, parent_id FROM rt_hosts_hosts_parents";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read host parents from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host parents: got " << i
                 << ", expected "
                 << sizeof(entries) / sizeof(*entries));
        if ((q.value(0).toUInt() != entries[i].child_id)
            || (q.value(1).toUInt() != entries[i].parent_id))
          throw (exceptions::msg() << "invalid host parent entry: got ("
                 << "child id " << q.value(0).toUInt() << ", parent id "
                 << q.value(1).toUInt() << "), expected ("
                 << entries[i].child_id << ", " << entries[i].parent_id
                 << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much host parents in DB");
    }

    // Stop the monitoring engine.
    daemon.stop();
    sleep_for(6);

    // Check that host dependencies were deleted.
    {
      QSqlQuery q(*db.storage_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_hosts_hosts_dependencies")
          || !q.next()
          || q.value(0).toUInt())
        throw (exceptions::msg() << "host dependencies were not deleted"
               << " after engine shutdown: "
               << qPrintable(q.lastError().text()));
    }

    // Check that service dependencies were deleted.
    {
      QSqlQuery q(*db.storage_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_services_services_dependencies")
          || !q.next()
          || q.value(0).toUInt())
        throw (exceptions::msg() << "service dependencies were not "
               << "deleted after engine shutdown: "
               << qPrintable(q.lastError().text()));
    }

    // Check that host parents were deleted.
    {
      QSqlQuery q(*db.storage_db());
      if (!q.exec("SELECT COUNT(*) FROM rt_hosts_hosts_parents")
          || !q.next()
          || q.value(0).toUInt())
        throw (exceptions::msg() << "host parents were not deleted "
               << "after engine shutdown: "
               << qPrintable(q.lastError().text()));
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_host_dependencies(host_deps);
  free_service_dependencies(service_deps);

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
