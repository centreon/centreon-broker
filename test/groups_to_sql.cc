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

    // Group data.
    struct {
      std::string alias;
    } host_groups_entries[] = {
      { "MyHostGroup#1" },
      { "MyHostGroup#2" }
    },
      service_groups_entries[] = {
      { "MyServiceGroup#1" },
      { "MyServiceGroup#2" },
      { "MyServiceGroup#3" },
      { "MyServiceGroup#4" },
      { "MyServiceGroup#5" },
      { "MyServiceGroup#6" }
    };

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    generate_host_groups(host_groups, 2);
    generate_service_groups(service_groups, HOST_COUNT);
    {
      std::list<hostgroup>::iterator
        hg(host_groups.begin()),
        hg_end(host_groups.end());
      unsigned int i(0);
      for (std::list<host>::iterator
             it(hosts.begin()),
             end(hosts.end());
           it != end;
           ++it) {
        hg->alias = new char[host_groups_entries[i].alias.size() + 1];
        strcpy(hg->alias, host_groups_entries[i].alias.c_str());
        link(*it, *hg);
        if (++hg == hg_end)
          hg = host_groups.begin();
        if ((sizeof(host_groups_entries) / sizeof(*host_groups_entries))
            == ++i)
          i = 0;
      }
    }
    {
      std::list<servicegroup>::iterator
        sg(service_groups.begin()),
        sg_end(service_groups.end());
      unsigned int i(0);
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it) {
        sg->alias = new char[service_groups_entries[i].alias.size() + 1];
        strcpy(sg->alias, service_groups_entries[i].alias.c_str());
        link(*it, *sg);
        if (++sg == sg_end)
          sg = service_groups.begin();
        if ((sizeof(service_groups_entries)
             / sizeof(*service_groups_entries))
            == ++i)
          i = 0;
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
      query << "SELECT hostgroup_id, name, instance_id, alias"
            << "  FROM rt_hostgroups"
            << "  ORDER BY name";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read hostgroups from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(host_groups_entries) / sizeof(*host_groups_entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host group entries: got " << i
                 << ", expected "
                 << sizeof(host_groups_entries) / sizeof(*host_groups_entries));
        host_groups_id.push_back(q.value(0).toUInt());
        if ((q.value(1).toUInt() != (i + 1))
            || (q.value(2).toUInt() != 42)
            || (q.value(3).toString()
                != host_groups_entries[i].alias.c_str()))
          throw (exceptions::msg()
                 << "invalid host group entry: got (name "
                 << q.value(1).toUInt() << ", instance_id "
                 << q.value(2).toUInt() << ", alias "
                 << q.value(3).toString() << "), expected ("
                 << (i + 1) << ", 42 "
                 << host_groups_entries[i].alias.c_str() << ")");
      }
      if (q.next())
        throw (exceptions::msg() << "too much host group entries");
    }

    // Check the 'servicegroups' table.
    std::list<unsigned int> service_groups_id;
    {
      std::ostringstream query;
      query << "SELECT servicegroup_id, name, instance_id, alias"
            << "  FROM rt_servicegroups"
            << "  ORDER BY name";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot read servicegroups from DB: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(service_groups_entries) / sizeof(*service_groups_entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough service group entries: got " << i
                 << ", expected "
                 << sizeof(service_groups_entries) / sizeof(*service_groups_entries));
        service_groups_id.push_back(q.value(0).toUInt());
        if ((q.value(1).toUInt() != (i + 1))
            || (q.value(2).toUInt() != 42)
            || (q.value(3).toString()
                != service_groups_entries[i].alias.c_str()))
          throw (exceptions::msg()
                 << "invalid service group entry: got (name "
                 << q.value(1).toUInt() << ", instance_id "
                 << q.value(2).toUInt() << ", alias "
                 << q.value(3).toString() << "), expected ("
                 << (i + 1) << ", 42 "
                 << service_groups_entries[i].alias.c_str() << ")");
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
            << " FROM rt_hosts_hostgroups"
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
            << " FROM rt_services_servicegroups"
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
      if (!q.exec("SELECT COUNT(*) FROM rt_hostgroups WHERE enabled=1")
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
      if (!q.exec("SELECT COUNT(*) FROM rt_servicegroups WHERE enabled=1")
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
