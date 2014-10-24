/*
** Copyright 2012-2014 Merethis
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

#define DB_NAME "broker_downtimes_to_sql"

/**
 *  Check that downtimes are properly inserted in SQL database.
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
    {
      char const* cmdline(MY_PLUGIN_PATH " 1");
      commands.front().command_line = new char[strlen(cmdline) + 1];
      strcpy(commands.front().command_line, cmdline);
    }
    generate_hosts(hosts, 10);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end;
         ++it)
      if (!strcmp(it->name, "2")) {
        it->host_check_command = new char[2];
        strcpy(it->host_check_command, "1");
        break ;
      }
    generate_services(services, hosts, 5);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it)
      if (!strcmp(it->host_name, "7")
          && !strcmp(it->description, "31")) {
        it->service_check_command = new char[2];
        strcpy(it->service_check_command, "1");
        break ;
      }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/downtimes_to_sql.xml\n";
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

    // Let the daemon initialize.
    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Set soon-to-be-in-downtime service as passive.
    {
      commander.execute("ENABLE_PASSIVE_SVC_CHECKS;7;31");
      commander.execute("DISABLE_SVC_CHECK;7;31");
    }

    // Run a little while.
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Base time.
    time_t now(time(NULL));

    // Add downtimes on two hosts.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_HOST_DOWNTIME;2;" << now << ";" << now + 3600
          << ";1;0;3600;Merethis;Centreon is beautiful";
      commander.execute(oss.str().c_str());
    }
    {
      std::ostringstream oss;
      oss << "SCHEDULE_HOST_DOWNTIME;1;" << now + 1000 << ";"
          << now + 2000
          << ";0;0;123;Broker;Some random and useless comment.";
      commander.execute(oss.str().c_str());
    }

    // Add downtimes on two services.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_SVC_DOWNTIME;7;31;" << now << ";"
          << now + 8638
          << ";0;0;7129;Default Author; This is a comment !";
      commander.execute(oss.str().c_str());
    }
    {
      std::ostringstream oss;
      oss << "SCHEDULE_SVC_DOWNTIME;10;48;" << now + 2834 << ";"
          << now + 987564 << ";1;0;2;Author;Scheduling downtime";
      commander.execute(oss.str().c_str());
    }

    // Let the monitoring engine run a while.
    sleep_for(20 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // New time.
    time_t t1(now);
    now = time(NULL);

    // Check downtimes.
    {
      std::ostringstream query;
      query << "SELECT internal_id, host_id, service_id, entry_time,"
            << "       actual_end_time, actual_start_time, author,"
            << "       cancelled, comment_data, deletion_time,"
            << "       duration, end_time, fixed, start_time, started,"
            << "       triggered_by, type"
            << "  FROM downtimes"
            << "  ORDER BY internal_id ASC";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get downtimes from DB: "
               << q.lastError().text().toStdString().c_str());

      if (// Host downtime #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || (q.value(1).toUInt() != 2)
          || !q.value(2).isNull()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          || !q.value(4).isNull()
          || (static_cast<time_t>(q.value(5).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(5).toLongLong()) > now)
          || (q.value(6).toString() != "Merethis")
          || q.value(7).toUInt()
          || (q.value(8).toString() != "Centreon is beautiful")
          || !q.value(9).isNull()
          || (q.value(10).toUInt() != 3600)
          || (static_cast<time_t>(q.value(11).toLongLong()) != t1 + 3600)
          || !q.value(12).toUInt()
          || (static_cast<time_t>(q.value(13).toLongLong()) != t1)
          || !q.value(14).toUInt()
          || !q.value(15).isNull()
          || (q.value(16).toUInt() != 2)
          // Host downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || (q.value(1).toUInt() != 1)
          || !q.value(2).isNull()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          || !q.value(4).isNull()
          || !q.value(5).isNull()
          || (q.value(6).toString() != "Broker")
          || q.value(7).toUInt()
          || (q.value(8).toString() != "Some random and useless comment.")
          || !q.value(9).isNull()
          || (q.value(10).toUInt() != 123)
          || (static_cast<time_t>(q.value(11).toLongLong()) != t1 + 2000)
          || q.value(12).toUInt()
          || (static_cast<time_t>(q.value(13).toLongLong()) != t1 + 1000)
          || q.value(14).toUInt()
          || !q.value(15).isNull()
          || (q.value(16).toUInt() != 2)
          // Service downtime #1.
          || !q.next()
          || (q.value(0).toUInt() != 3)
          || (q.value(1).toUInt() != 7)
          || (q.value(2).toUInt() != 31)
          || (static_cast<time_t>(q.value(3).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          || !q.value(4).isNull()
          || (static_cast<time_t>(q.value(5).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(5).toLongLong()) > now)
          || (q.value(6).toString() != "Default Author")
          || q.value(7).toUInt()
          || (q.value(8).toString() != " This is a comment !")
          || !q.value(9).isNull()
          || (q.value(10).toUInt() != 7129)
          || (static_cast<time_t>(q.value(11).toLongLong()) != t1 + 8638)
          || q.value(12).toUInt()
          || (static_cast<time_t>(q.value(13).toLongLong()) != t1)
          || !q.value(14).toUInt()
          || !q.value(15).isNull()
          || (q.value(16).toUInt() != 1)
          // Service downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 4)
          || (q.value(1).toUInt() != 10)
          || (q.value(2).toUInt() != 48)
          || (static_cast<time_t>(q.value(3).toLongLong()) < t1)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          || !q.value(4).isNull()
          || !q.value(5).isNull()
          || (q.value(6).toString() != "Author")
          || q.value(7).toUInt()
          || (q.value(8).toString() != "Scheduling downtime")
          || !q.value(9).isNull()
          || (q.value(10).toUInt() != 984730)
          || (static_cast<time_t>(q.value(11).toLongLong())
              != t1 + 987564)
          || !q.value(12).toUInt()
          || (static_cast<time_t>(q.value(13).toLongLong())
              != t1 + 2834)
          || q.value(14).toUInt()
          || !q.value(15).isNull()
          || (q.value(16).toUInt() != 1)
          // EOF
          || q.next())
        throw (exceptions::msg() << "invalid downtime entry in DB");
    }

    // Check hosts.
    {
      std::ostringstream query;
      query << "SELECT COUNT(*)"
            << "  FROM hosts"
            << "  WHERE scheduled_downtime_depth=0";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get host status from DB: "
               << qPrintable(q.lastError().text()));

      if (!q.next()
          || (q.value(0).toUInt() != (10 - 1))
          || q.next())
        throw (exceptions::msg()
               << "invalid host status during downtime");
    }

    // Check services.
    {
      std::ostringstream query;
      query << "SELECT COUNT(*)"
            << "  FROM services"
            << "  WHERE scheduled_downtime_depth=0";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get service status from DB: "
               << qPrintable(q.lastError().text()));

      if (!q.next()
          || (q.value(0).toUInt() != (10 * 5 - 1))
          || q.next())
        throw (exceptions::msg()
               << "invalid service status during downtime");
    }

    // Delete downtimes.
    commander.execute("DEL_HOST_DOWNTIME;2");
    commander.execute("DEL_SVC_DOWNTIME;4");
    commander.execute("DEL_SVC_DOWNTIME;3");
    commander.execute("DEL_HOST_DOWNTIME;1");

    // Run a while.
    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Update time.
    time_t t2(now);
    now = time(NULL);

    // Check for deletion.
    {
      std::ostringstream query;
      query << "SELECT internal_id, actual_end_time, cancelled, deletion_time"
            << "  FROM downtimes"
            << "  ORDER BY internal_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get deletion_time of downtimes: "
               << q.lastError().text().toStdString().c_str());
      if (// Host downtime #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || (static_cast<time_t>(q.value(1).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(1).toLongLong()) > now)
          || !q.value(2).toUInt()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          // Host downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || !q.value(1).isNull()
          || !q.value(2).toUInt()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          // Service downtime #1.
          || !q.next()
          || (q.value(0).toUInt() != 3)
          || (static_cast<time_t>(q.value(1).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(1).toLongLong()) > now)
          || !q.value(2).toUInt()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          // Service downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 4)
          || !q.value(1).isNull()
          || !q.value(2).toUInt()
          || (static_cast<time_t>(q.value(3).toLongLong()) < t2)
          || (static_cast<time_t>(q.value(3).toLongLong()) > now)
          // EOF
          || q.next())
        throw (exceptions::msg()
               << "invalid actual_end_time or deletion_time in DB");
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
