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
  std::string engine_config_path(tmpnam(NULL));
  external_command commander;
  engine daemon;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
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
      &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();

    // Let the daemon initialize.
    sleep(10);

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
    sleep(40);

    // New time.
    time_t t1(now);
    now = time(NULL);

    // Check downtimes.
    {
      std::ostringstream query;
      query << "SELECT internal_id, host_id, service_id, entry_time,"
            << "       author, cancelled, comment_data, deletion_time,"
            << "       duration, end_time, fixed, start_time,"
            << "       started, triggered_by, type"
            << "  FROM downtimes"
            << "  ORDER BY internal_id ASC";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get downtimes from DB: "
               << q.lastError().text().toStdString().c_str());

      if (// Host downtime #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || (q.value(1).toUInt() != 2)
          || !q.value(2).isNull()
          || (q.value(3).toUInt() < t1)
          || (q.value(3).toUInt() > now)
          || (q.value(4).toString() != "Merethis")
          || q.value(5).toUInt()
          || (q.value(6).toString() != "Centreon is beautiful")
          || !(q.value(7).isNull() || !q.value(7).toUInt())
          || (q.value(8).toUInt() != 3600)
          || (q.value(9).toUInt() != t1 + 3600)
          || !q.value(10).toUInt()
          || (q.value(11).toUInt() != t1)
          || !q.value(12).toUInt()
          || !(q.value(13).isNull() || !q.value(13).toUInt())
          || (q.value(14).toUInt() != 2)
          // Host downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || (q.value(1).toUInt() != 1)
          || !q.value(2).isNull()
          || (q.value(3).toUInt() < t1)
          || (q.value(3).toUInt() > now)
          || (q.value(4).toString() != "Broker")
          || q.value(5).toUInt()
          || (q.value(6).toString() != "Some random and useless comment.")
          || !(q.value(7).isNull() || !q.value(7).toUInt())
          || (q.value(8).toUInt() != 123)
          || (q.value(9).toUInt() != t1 + 2000)
          || q.value(10).toUInt()
          || (q.value(11).toUInt() != t1 + 1000)
          || q.value(12).toUInt()
          || !(q.value(13).isNull() || !q.value(13).toUInt())
          || (q.value(14).toUInt() != 2)
          // Service downtime #1.
          || !q.next()
          || (q.value(0).toUInt() != 3)
          || (q.value(1).toUInt() != 7)
          || (q.value(2).toUInt() != 31)
          || (q.value(3).toUInt() < t1)
          || (q.value(3).toUInt() > now)
          || (q.value(4).toString() != "Default Author")
          || q.value(5).toUInt()
          || (q.value(6).toString() != " This is a comment !")
          || !(q.value(7).isNull() || !q.value(7).toUInt())
          || (q.value(8).toUInt() != 7129)
          || (q.value(9).toUInt() != t1 + 8638)
          || q.value(10).toUInt()
          || (q.value(11).toUInt() != t1)
          // XXX : downtime will start if service command returns non-OK
          || q.value(12).toUInt()
          || !(q.value(13).isNull() || !q.value(13).toUInt())
          || (q.value(14).toUInt() != 1)
          // Service downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 4)
          || (q.value(1).toUInt() != 10)
          || (q.value(2).toUInt() != 48)
          || (q.value(3).toUInt() < t1)
          || (q.value(3).toUInt() > now)
          || (q.value(4).toString() != "Author")
          || q.value(5).toUInt()
          || (q.value(6).toString() != "Scheduling downtime")
          || !(q.value(7).isNull() || !q.value(7).toUInt())
          || (q.value(8).toUInt() != 984730)
          || (q.value(9).toUInt() != t1 + 987564)
          || !q.value(10).toUInt()
          || (q.value(11).toUInt() != t1 + 2834)
          || q.value(12).toUInt()
          || !(q.value(13).isNull() || !q.value(13).toUInt())
          || (q.value(14).toUInt() != 1)
          // EOF
          || q.next())
        throw (exceptions::msg() << "invalid downtime entry in DB");
    }

    // Delete downtimes.
    commander.execute("DEL_HOST_DOWNTIME;2");
    commander.execute("DEL_SVC_DOWNTIME;4");
    commander.execute("DEL_SVC_DOWNTIME;3");
    commander.execute("DEL_HOST_DOWNTIME;1");

    // Run a while.
    sleep(10);

    // Update time.
    time_t t2(now);
    now = time(NULL);

    // Check for deletion.
    {
      std::ostringstream query;
      query << "SELECT internal_id, cancelled, deletion_time"
            << "  FROM downtimes"
            << "  ORDER BY internal_id";
      QSqlQuery q(db);
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get deletion_time of downtimes: "
               << q.lastError().text().toStdString().c_str());
      if (// Host downtime #1.
          !q.next()
          || (q.value(0).toUInt() != 1)
          || !q.value(1).toUInt()
          || (q.value(2).toUInt() < t2)
          || (q.value(2).toUInt() > now)
          // Host downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 2)
          || !q.value(1).toUInt()
          || (q.value(2).toUInt() < t2)
          || (q.value(2).toUInt() > now)
          // Service downtime #1.
          || !q.next()
          || (q.value(0).toUInt() != 3)
          || !q.value(1).toUInt()
          || (q.value(2).toUInt() < t2)
          || (q.value(2).toUInt() > now)
          // Service downtime #2.
          || !q.next()
          || (q.value(0).toUInt() != 4)
          || !q.value(1).toUInt()
          || (q.value(2).toUInt() < t2)
          || (q.value(2).toUInt() > now)
          // EOF
          || q.next())
        throw (exceptions::msg() << "invalid deletion_time in DB");
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
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
