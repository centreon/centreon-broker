/*
** Copyright 2013-2015 Centreon
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

#define DB_NAME "broker_groups_to_sql"
#define HOST_COUNT 6
#define SERVICES_BY_HOST 5

/**
 *  Check that groups and associated memberships are properly inserted
 *  in SQL database.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::list<hostgroup> host_groups;
  std::list<servicegroup> service_groups;
  std::string engine_config_path(tmpnam(NULL));
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    generate_host_groups(host_groups, 2);
    generate_service_groups(service_groups, HOST_COUNT);
    {
      std::list<hostgroup>::iterator
        hg(host_groups.begin()),
        hg_end(host_groups.end());
      for (std::list<host>::iterator
             it(hosts.begin()),
             end(hosts.end());
           it != end;
           ++it) {
        link(*it, *hg);
        if (++hg == hg_end)
          hg = host_groups.begin();
      }
    }
    {
      std::list<servicegroup>::iterator
        sg(service_groups.begin()),
        sg_end(service_groups.end());
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it) {
        link(*it, *sg);
        if (++sg == sg_end)
          sg = service_groups.begin();
      }
    }
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/groups_to_sql.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      NULL,
      &host_groups,
      &service_groups);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(16 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check the 'hostgroups' table.
    std::list<unsigned int> host_groups_id;
    {
      std::ostringstream query;
      query << "SELECT hostgroup_id, name"
            << "  FROM hostgroups"
            << "  ORDER BY name";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read hostgroups from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < 2;
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host group entries: got " << i
                 << ", expected 2");
        host_groups_id.push_back(q.value(0).toUInt());
        if (q.value(1).toUInt() != (i + 1))
          throw (exceptions::msg()
                 << "invalid host group entry: got (name "
                 << q.value(1).toUInt() << "), expected ("
                 << (i + 1) << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much host group entries");
    }

    // Check the 'servicegroups' table.
    std::list<unsigned int> service_groups_id;
    {
      std::ostringstream query;
      query << "SELECT servicegroup_id, name"
            << "  FROM servicegroups"
            << "  ORDER BY name";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read servicegroups from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < HOST_COUNT;
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough service group entries: got " << i
                 << ", expected " << HOST_COUNT);
        service_groups_id.push_back(q.value(0).toUInt());
        if (q.value(1).toUInt() != (i + 1))
          throw (exceptions::msg()
                 << "invalid service group entry: got (name "
                 << q.value(1).toUInt() << "), expected ("
                 << (i + 1) << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much service group entries");
    }

    // Check hosts/hostgroups links.
    {
      unsigned int i(0);
      for (std::list<unsigned int>::iterator
             it(host_groups_id.begin()),
             end(host_groups_id.end());
           it != end;
           ++it, ++i) {
        std::ostringstream oss;
        oss << "SELECT host_id"
            << " FROM hosts_hostgroups"
            << " WHERE hostgroup_id=" << *it
            << " ORDER BY host_id ASC";
        QSqlQuery q(*db.storage_db());
        if (!q.exec(oss.str().c_str()))
          throw (exceptions::msg()
                 << "cannot get hosts/hostgroups links: "
                 << qPrintable(q.lastError().text()));
        for (unsigned int j(0); j < HOST_COUNT / 2; ++j) {
          if (!q.next())
            throw (exceptions::msg()
                   << "not enough hosts linked to host group #" << i
                   << " (id " << *it << "): got " << i
                   << ", expected " << HOST_COUNT / 2);
          if (q.value(0).toUInt() != (j * 2 + i + 1))
            throw (exceptions::msg()
                   << "invalid host link to host group #" << i
                   << " (id " << *it << "): got host id "
                   << q.value(0).toUInt() << ", expected "
                   << (j * 2 + i + 1));
        }
        if (q.next())
          throw (exceptions::msg()
                 << "too much hosts linked to host group #" << i
                 << " (id " << *it << ")");
      }
    }

    // Check services/servicegroups links.
    {
      unsigned int i(0);
      for (std::list<unsigned int>::iterator
             it(service_groups_id.begin()),
             end(service_groups_id.end());
           it != end;
           ++it, ++i) {
        std::ostringstream oss;
        oss << "SELECT service_id"
            << " FROM services_servicegroups"
            << " WHERE servicegroup_id=" << *it
            << " ORDER BY service_id ASC";
        QSqlQuery q(*db.storage_db());
        if (!q.exec(oss.str().c_str()))
          throw (exceptions::msg()
                 << "cannot get services/servicegroups links: "
                 << qPrintable(q.lastError().text()));
        for (unsigned int j(0);
             j < SERVICES_BY_HOST;
             ++j) {
          if (!q.next())
            throw (exceptions::msg()
                   << "not enough services linked to service group #"
                   << i << " (id " << *it << "): got " << i
                   << ", expected " << SERVICES_BY_HOST);
          if (q.value(0).toUInt() != (j * HOST_COUNT + i + 1))
            throw (exceptions::msg()
                   << "invalid host link to host group #" << i
                   << " (id " << *it << "): got host id "
                   << q.value(0).toUInt() << ", expected "
                   << (j * HOST_COUNT + i + 1));
        }
        if (q.next())
          throw (exceptions::msg()
                 << "too much hosts linked to host group #" << i
                 << " (id " << *it << ")");
      }
    }

    // Stop engine.
    daemon.stop();
    sleep_for(8 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that host groups were deleted from DB.
    {
      QSqlQuery q(*db.storage_db());
      if (!q.exec("SELECT COUNT(*) FROM hostgroups")
          || !q.next())
        throw (exceptions::msg()
               << "cannot get host group count from DB: "
               << qPrintable(q.lastError().text()));
      if (q.value(0).toUInt())
        throw (exceptions::msg() << q.value(0).toUInt()
               << " host groups remain in DB after shutdown");
    }

    // Check that service groups were deleted from DB.
    {
      QSqlQuery q(*db.storage_db());
      if (!q.exec("SELECT COUNT(*) FROM servicegroups")
          || !q.next())
        throw (exceptions::msg()
               << "cannot get service group count from DB: "
               << qPrintable(q.lastError().text()));
      if (q.value(0).toUInt())
        throw (exceptions::msg() << q.value(0).toUInt()
               << " service groups remain in DB after shutdown");
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
  free_host_groups(host_groups);
  free_service_groups(service_groups);

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
