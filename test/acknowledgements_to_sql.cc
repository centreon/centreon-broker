/*
** Copyright 2012-2015 Merethis
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
#include <cstring>
#include <ctime>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_acknowledgements_to_sql"

/**
 *  Check that acknowledgements are properly inserted in SQL database.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::list<command> commands;
  std::string engine_config_path(tmpnam(NULL));
  external_command commander;
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 1);
    for (std::list<command>::iterator
           it(commands.begin()),
           end(commands.end());
         it != end;
         ++it) {
      char const* cmdline(MY_PLUGIN_PATH " 2");
      it->command_line = new char[strlen(cmdline) + 1];
      strcpy(it->command_line, cmdline);
    }
    generate_hosts(hosts, 2);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end;
         ++it) {
      it->host_check_command = new char[2];
      strcpy(it->host_check_command, "1");
    }
    generate_services(services, hosts, 1);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it) {
      it->service_check_command = new char[2];
      strcpy(it->service_check_command, "1");
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "use_aggressive_host_checking=1\n"
          << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/acknowledgements_to_sql.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      &commands);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();

    // Let the daemon initialize and set checkpoints as non-OK.
    sleep_for(15);

    // Base time.
    time_t now(time(NULL));

    // Acknowledge the hosts.
    {
      std::ostringstream oss;
      oss << "ACKNOWLEDGE_HOST_PROBLEM;1;1;0;0;Merethis;Random comment";
      commander.execute(oss.str().c_str());
    }
    {
      std::ostringstream oss;
      oss << "ACKNOWLEDGE_HOST_PROBLEM;2;1;0;0;Centreon;Comment text.";
      commander.execute(oss.str().c_str());
    }

    // Acknowledge the services.
    {
      std::ostringstream oss;
      oss << "ACKNOWLEDGE_SVC_PROBLEM;1;1;1;0;0;Broker;Monitoring";
      commander.execute(oss.str().c_str());
    }
    {
      std::ostringstream oss;
      oss << "ACKNOWLEDGE_SVC_PROBLEM;2;2;1;0;0;Author;Just a comment!";
      commander.execute(oss.str().c_str());
    }

    // Let the monitoring engine process commands.
    sleep_for(10);

    // New time.
    time_t t1(now);
    now = time(NULL);

    // Check acknowledgements.
    {
      std::ostringstream query;
      query << "SELECT host_id, service_id, entry_time, author,"
            << "       comment_data, deletion_time, notify_contacts,"
            << "       persistent_comment, sticky, type"
            << "  FROM rt_acknowledgements"
            << "  ORDER BY service_id ASC, host_id ASC";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get acknowledgements from DB: "
               << q.lastError().text().toStdString().c_str());

      if (// Host acknowledgement #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || !q.value(1).isNull()
          || (static_cast<time_t>(q.value(2).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          || (q.value(3).toString() != "Merethis")
          || (q.value(4).toString() != "Random comment")
          || !(q.value(5).isNull() || !q.value(5).toUInt())
          || q.value(6).toUInt()
          || q.value(7).toUInt()
          // XXX: sticky not set || !q.value(8).toUInt()
          || q.value(9).toUInt()
          // Host acknowledgement #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || !q.value(1).isNull()
          || (static_cast<time_t>(q.value(2).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          || (q.value(3).toString() != "Centreon")
          || (q.value(4).toString() != "Comment text.")
          || !(q.value(5).isNull() || !q.value(5).toUInt())
          || q.value(6).toUInt()
          || q.value(7).toUInt()
          // XXX: sticky not set || !q.value(8).toUInt()
          || q.value(9).toUInt()
          // Service acknowledgement #1.
          || !q.next()
          || (q.value(0).toUInt() != 1)
          || (q.value(1).toUInt() != 1)
          || (static_cast<time_t>(q.value(2).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          || (q.value(3).toString() != "Broker")
          || (q.value(4).toString() != "Monitoring")
          || !(q.value(5).isNull() || !q.value(5).toUInt())
          || q.value(6).toUInt()
          || q.value(7).toUInt()
          // XXX: sticky not set || !q.value(8).toUInt()
          || (q.value(9).toUInt() != 1)
          // Service acknowledgement #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || (q.value(1).toUInt() != 2)
          || (static_cast<time_t>(q.value(2).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          || (q.value(3).toString() != "Author")
          || (q.value(4).toString() != "Just a comment!")
          || !(q.value(5).isNull() || !q.value(5).toUInt())
          || q.value(6).toUInt()
          || q.value(7).toUInt()
          // XXX: sticky not set || !q.value(8).toUInt()
          || (q.value(9).toUInt() != 1)
          // EOF.
          || q.next())
        throw (exceptions::msg()
               << "invalid acknowledgement entry in DB");
    }

    // Disable acknowledgements on host #1 and service #1.
    commander.execute("REMOVE_HOST_ACKNOWLEDGEMENT;1");
    commander.execute("REMOVE_SVC_ACKNOWLEDGEMENT;1;1");

    // Disable active checks on host #2.
    commander.execute("DISABLE_HOST_CHECK;2");
    commander.execute(
      "PROCESS_HOST_CHECK_RESULT;2;0;Submitted by unit test");
    commander.execute(
      "PROCESS_HOST_CHECK_RESULT;2;0;Submitted by unit test");
    commander.execute(
      "PROCESS_HOST_CHECK_RESULT;2;0;Submitted by unit test");

    // Disable active check on service #2.
    commander.execute("DISABLE_SVC_CHECK;2;2");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;2;2;0;Submitted by unit test");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;2;2;0;Submitted by unit test");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;2;2;0;Submitted by unit test");

    // Run a while.
    sleep_for(12);

    // Update time.
    time_t t2(now);
    now = time(NULL);

    // Check hosts.
    {
      std::ostringstream query;
      query << "SELECT host_id, acknowledged"
            << "  FROM rt_hosts"
            << "  ORDER BY host_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get host list from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != 1)
          || q.value(1).toUInt()
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || q.value(1).toUInt()
          || q.next())
        throw (exceptions::msg()
               << "invalid host entry after deletion");
    }

    // Check services.
    {
      std::ostringstream query;
      query << "SELECT host_id, service_id, acknowledged"
            << "  FROM rt_services"
            << "  ORDER BY host_id ASC, service_id ASC";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get service list from DB: "
               << q.lastError().text().toStdString().c_str());
      if (!q.next()
          || (q.value(0).toUInt() != 1)
          || (q.value(1).toUInt() != 1)
          || q.value(2).toUInt()
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || (q.value(1).toUInt() != 2)
          || q.value(2).toUInt()
          || q.next())
        throw (exceptions::msg()
               << "invalid service entry after deletion");
    }

    // Check acknowledgements.
    {
      std::ostringstream query;
      query << "SELECT host_id, service_id, deletion_time"
            << "  FROM rt_acknowledgements"
            << "  ORDER BY service_id ASC, host_id ASC";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get acknowledgement list from DB: "
               << q.lastError().text().toStdString().c_str());
      if (// Host acknowledgement #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || !q.value(1).isNull()
          || (static_cast<time_t>(q.value(2).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          // Host acknowledgement #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || !q.value(1).isNull()
          || !(q.value(2).isNull() || !q.value(2).toUInt())
          // Service acknowledgement #1.
          || !q.next()
          || (q.value(0).toUInt() != 1)
          || (q.value(1).toUInt() != 1)
          || (static_cast<time_t>(q.value(2).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(2).toLongLong()) > now)
          // Service acknowledgement #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || (q.value(1).toUInt() != 2)
          || !(q.value(2).isNull() || !q.value(2).toUInt())
          // EOF
          || q.next())
        throw (exceptions::msg()
               << "invalid acknowledgement entry after deletion");
    }

    // Success.
    retval = EXIT_SUCCESS;
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

  return (retval);
}
