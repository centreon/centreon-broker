/*
** Copyright 2013 Merethis
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

#define DB_NAME "broker_correlation_to_sql"
#define HOST_COUNT 10
#define SERVICES_BY_HOST 2

/**
 *  Check that correlation is properly inserted in SQL database.
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
    generate_hosts(hosts, HOST_COUNT);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end;
         ++it) {
      it->accept_passive_host_checks = 1;
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it) {
      it->accept_passive_service_checks = 1;
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/correlation_to_sql.xml\n";
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

    // T1.
    time_t t1(time(NULL));

    // Let the daemon initialize.
    sleep_for(10 * MONITORING_ENGINE_INTERVAL_LENGTH);

    /* Test cases
    ** ----------
    **
    ** 1) OK -> CRITICAL
    ** 2) OK -> WARNING -> DOWNTIME
    ** 3) OK -> CRITICAL -> ACK -> WARNING
    ** 4) OK -> DOWNTIME -> WARNING -> ACK -> CRITICAL
    ** 5) OK -> CRITICAL -> ACK
    ** 6) OK -> WARNING -> ACK -> DOWNTIME
    */

    // Step 1.
    {
      // Set hosts as OK.
      for (unsigned int i(1); i <= HOST_COUNT; ++i) {
        std::ostringstream cmd;
        cmd << "PROCESS_HOST_CHECK_RESULT;" << i << ";0;output1-" << i;
        commander.execute(cmd.str());
      }

      // Set services as OK.
      for (unsigned int i(0); i <= HOST_COUNT * SERVICES_BY_HOST; ++i) {
        unsigned int host_id((i % HOST_COUNT) + 1);
        unsigned int service_id(i + 1);
        std::ostringstream cmd;
        cmd << "PROCESS_SERVICE_CHECK_RESULT;" << host_id << ";"
            << service_id << ";0;output1-" << host_id << ";"
            << service_id;
        commander.execute(cmd.str());
      }
    }
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T2.
    time_t t2(time(NULL));

    // Step 2.
    {
      commander.execute("PROCESS_HOST_CHECK_RESULT;1;2;output2-1");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;1;output2-1-1");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output2-1-2");
      {
        std::ostringstream oss;
        oss << "SCHEDULE_HOST_DOWNTIME;2;" << t2 << ";" << (t2 + 3600)
            << ";1;0;3600;Merethis;Host #2 is going in downtime";
        commander.execute(oss.str());
      }
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;2;1;2;output2-2-1");
      commander.execute("PROCESS_SERVICE_CHECK_RESULT;2;2;1;output2-2-2");
    }
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T3.
    time_t t3(time(NULL));

    // Step 3.
    {
      {
        std::ostringstream oss;
        oss << "SCHEDULE_SVC_DOWNTIME;1;1;" << t3 << ";" << (t3 + 2000)
            << ";1;0;2000;Centreon;Service #1-#1 is going in downtime";
        commander.execute(oss.str());
      }
      commander.execute("ACKNOWLEDGE_SVC_PROBLEM;1;2;0;0;1;Broker;Ack SVC1-2");
      commander.execute("PROCESS_HOST_CHECK_RESULT;2;1;output3-2");
      commander.execute("ACKNOWLEDGE_SVC_PROBLEM;2;1;1;0;1;Engine;Ack SVC2-1");
      commander.execute("ACKNOWLEDGE_SVC_PROBLEM;2;2;0;0;1;foo;Ack SVC2-2");
    }
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T4.
    time_t t4(time(NULL));

    // Step 4.
    {
      commander.execute("PROCESS_SVC_CHECK_RESULT;1;2;1;output4-1-2");
      commander.execute("ACKNOWLEDGE_HOST_PROBLEM;2;1;0;1;Centreon Map;Ack HST2");
      {
        std::ostringstream oss;
        oss << "SCHEDULE_SVC_DOWNTIME;2;2;" << t4 << ";" << (t4 + 1600)
            << ";0;0;1000;Merethis;Service #2-#2 is going in downtime";
        commander.execute(oss.str());
      }
    }
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // T5.
    time_t t5(time(NULL));

    // Step 5.
    {
      commander.execute("PROCESS_HOST_CHECK_RESULT;2;2;output5-2");
    }
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check host state events.
    {
      struct {
        unsigned int host_id;
        time_t       start_time_low;
        time_t       start_time_high;
        bool         end_time_is_null;
        time_t       end_time_low;
        time_t       end_time_high;
        short        state;
        bool         ack_time_is_null;
        time_t       ack_time_low;
        time_t       ack_time_high;
        bool         in_downtime;
      } const          entries[] = {
        { 1, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 1, t1, t2, false, t2, t3, 0, true, 0, 0, false },
        { 1, t2, t3, true, 0, 0, 2, true, 0, 0, false },
        { 2, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 2, t1, t2, false, t2, t3, 0, true, 0, 0, true },
        { 2, t2, t3, false, t3, t4, 1, true, 0, 0, true },
        { 2, t3, t4, false, t4, t5, 1, false, t3, t4, true },
        { 2, t4, t5, true, 0, 0, 2, false, t4, t5, true },
        { 3, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 3, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 4, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 4, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 5, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 5, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 6, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 6, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 7, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 7, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 8, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 8, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 9, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 9, t1, t2, true, 0, 0, 0, true, 0, 0, false },
        { 10, 0, 1, false, t1, t2, 4, true, 0, 0, false },
        { 10, t1, t2, true, 0, 0, 0, true, 0, 0, false }
      };

      // Get host state events.
      QSqlQuery q(db);
      if (!q.exec("SELECT host_id, start_time, end_time, state,"
                  "       ack_time, in_downtime"
                  " FROM hoststateevents"
                  " ORDER BY host_id, start_time"))
        throw (exceptions::msg() << "cannot get host state events: "
               << q.lastError().text());

      // Compare DB with expected content.
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        // Get next entry.
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host state events in DB: got " << i
                 << ", expected "
                 << (sizeof(entries) / sizeof(*entries)));

        // Match entry.
        if ((q.value(0).toUInt() != entries[i].host_id)
            || (static_cast<time_t>(q.value(1).toLongLong())
                < entries[i].start_time_low)
            || (static_cast<time_t>(q.value(1).toLongLong())
                > entries[i].start_time_high)
            || (entries[i].end_time_is_null && !q.value(2).isNull())
            || (!entries[i].end_time_is_null
                && ((static_cast<time_t>(q.value(2).toLongLong())
                     < entries[i].end_time_low)
                    || (static_cast<time_t>(q.value(2).toLongLong())
                        > entries[i].end_time_high)))
            || (q.value(3).toInt() != entries[i].state)
            || (entries[i].ack_time_is_null && !q.value(4).isNull())
            || (!entries[i].ack_time_is_null
                && ((static_cast<time_t>(q.value(4).toLongLong())
                     < entries[i].ack_time_low)
                    || (static_cast<time_t>(q.value(4).toLongLong())
                        > entries[i].ack_time_high)))
            || (static_cast<bool>(q.value(5).toInt())
                != entries[i].in_downtime)) {
          exceptions::msg e;
          e << "invalid host state event entry #" << i
            << ": got (host id " << q.value(0).toUInt()
            << ", start time " << q.value(1).toUInt() << ", end time "
            << (q.value(2).isNull() ? "null" : q.value(2).toString())
            << ", state " << q.value(3).toInt() << ", ack time "
            << (q.value(4).isNull() ? "null" : q.value(4).toString())
            << ", in downtime " << q.value(5).toInt() << "), expected ("
            << entries[i].host_id << ", " << entries[i].start_time_low
            << ":" << entries[i].start_time_high << ", ";
          if (entries[i].end_time_is_null)
            e << "null";
          else
            e << entries[i].end_time_low << ":"
              << entries[i].end_time_high;
          e << ", " << entries[i].state << ", ";
          if (entries[i].ack_time_is_null)
            e << "null";
          else
            e << entries[i].ack_time_low << ":"
              << entries[i].ack_time_high;
          e << ", " << entries[i].in_downtime << ")";
          throw (e);
        }
      }

      // No more results.
      if (q.next())
        throw (exceptions::msg() << "too much host state events in DB");
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
  //config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
