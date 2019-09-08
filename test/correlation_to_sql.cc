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

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/broker_extcmd.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
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
  engine_extcmd engine_commander;
  broker_extcmd cbmod_commander;
  engine daemon;
  cbd broker;
  test_db db;
  test_file cbmod_cfg;
  test_file cbd_cfg;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Configuration files templates.
    cbmod_commander.set_file(tmpnam(NULL));
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR
                           "/test/cfg/correlation_to_sql_1.xml.in");
    cbmod_cfg.set("DB_NAME", DB_NAME);
    cbmod_cfg.set("COMMAND_FILE", cbmod_commander.get_file());
    cbd_cfg.set_template(PROJECT_SOURCE_DIR
                         "/test/cfg/correlation_to_sql_2.xml.in");

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end; ++it) {
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it) {
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    engine_commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << engine_commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " " << cbmod_cfg.generate()
          << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // Start broker daemon.
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    sleep_for(2);

    // T0.
    time_t t0(time(NULL));

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();

    // Let the daemon initialize.
    sleep_for(8);

    // T1.
    time_t t1(time(NULL));

    /* Test cases
    ** ----------
    **
    ** 1) UP -> DOWN
    ** 2) OK -> WARNING -> DOWNTIME
    ** 3) OK -> CRITICAL -> ACK (STICKY) -> WARNING
    ** 4) UP -> DOWNTIME -> DOWN -> ACK (STICKY)
    ** 5) OK -> CRITICAL -> ACK (STICKY).
    ** 6) OK -> WARNING -> ACK (NORMAL) -> DOWNTIME
    */

    // Step 1.
    {
      // Set hosts as OK.
      for (unsigned int i(1); i <= HOST_COUNT; ++i) {
        std::ostringstream cmd;
        cmd << "PROCESS_HOST_CHECK_RESULT;" << i << ";0;output1-" << i;
        engine_commander.execute(cmd.str());
      }

      // Set services as OK.
      for (unsigned int i(0); i < HOST_COUNT * SERVICES_BY_HOST; ++i) {
        unsigned int host_id((i / SERVICES_BY_HOST) + 1);
        unsigned int service_id(i + 1);
        std::ostringstream cmd;
        cmd << "PROCESS_SERVICE_CHECK_RESULT;" << host_id << ";" << service_id
            << ";0;output1-" << host_id << ";" << service_id;
        engine_commander.execute(cmd.str());
      }
    }
    sleep_for(3);

    // T2.
    time_t t2(time(NULL));

    // Step 2.
    {
      engine_commander.execute("PROCESS_HOST_CHECK_RESULT;1;1;output2-1");
      engine_commander.execute(
          "PROCESS_SERVICE_CHECK_RESULT;1;1;1;output2-1-1");
      engine_commander.execute(
          "PROCESS_SERVICE_CHECK_RESULT;1;2;2;output2-1-2");
      {
        std::ostringstream oss;
        oss << "EXECUTE;84;correlationtosql1-nodeevents;SCHEDULE_HOST_DOWNTIME;"
               "2;"
            << t2 << ";" << (t2 + 3600)
            << ";1;0;3600;Merethis;Host #2 is going in downtime";
        cbmod_commander.execute(oss.str());
      }
      engine_commander.execute(
          "PROCESS_SERVICE_CHECK_RESULT;2;3;2;output2-2-3");
      engine_commander.execute(
          "PROCESS_SERVICE_CHECK_RESULT;2;4;1;output2-2-4");
    }
    sleep_for(3);

    // T3.
    time_t t3(time(NULL));

    // Step 3.
    {
      {
        std::ostringstream oss;
        oss << "EXECUTE;84;correlationtosql1-nodeevents;SCHEDULE_SVC_DOWNTIME;"
               "1;1;"
            << t3 << ";" << (t3 + 2000)
            << ";1;0;2000;Centreon;Service #1-#1 is going in downtime";
        cbmod_commander.execute(oss.str());
      }
      cbmod_commander.execute(
          "EXECUTE;84;correlationtosql1-nodeevents;ACKNOWLEDGE_SVC_PROBLEM;1;2;"
          "1;0;1;Broker;Ack SVC1-2");
      engine_commander.execute("PROCESS_HOST_CHECK_RESULT;2;1;output3-2");
      cbmod_commander.execute(
          "EXECUTE;84;correlationtosql1-nodeevents;ACKNOWLEDGE_SVC_PROBLEM;2;3;"
          "1;0;1;Engine;Ack SVC2-3");
      cbmod_commander.execute(
          "EXECUTE;84;correlationtosql1-nodeevents;ACKNOWLEDGE_SVC_PROBLEM;2;4;"
          "0;0;1;foo;Ack SVC2-4");
    }
    sleep_for(3);

    // T4.
    time_t t4(time(NULL));

    // Step 4.
    {
      engine_commander.execute(
          "PROCESS_SERVICE_CHECK_RESULT;1;2;1;output4-1-2");
      cbmod_commander.execute(
          "EXECUTE;84;correlationtosql1-nodeevents;ACKNOWLEDGE_HOST_PROBLEM;2;"
          "2;0;1;Centreon Map;Ack HST2");
      {
        std::ostringstream oss;
        oss << "EXECUTE;84;correlationtosql1-nodeevents;SCHEDULE_SVC_DOWNTIME;"
               "2;4;"
            << t4 << ";" << (t4 + 1600)
            << ";0;0;1000;Merethis;Service #2-#4 is going in downtime";
        cbmod_commander.execute(oss.str());
      }
    }
    sleep_for(3);

    // T5.
    time_t t5(time(NULL));

    // Check host state events.
    {
      struct {
        unsigned int host_id;
        time_t start_time_low;
        time_t start_time_high;
        bool end_time_is_null;
        time_t end_time_low;
        time_t end_time_high;
        short state;
        bool ack_time_is_null;
        time_t ack_time_low;
        time_t ack_time_high;
        bool in_downtime;
      } const entries[] = {/*
                           ** Host 1.
                           */
                           // Start = UP.
                           {1, t0, t1, false, t2, t3, 0, true, 0, 0, false},
                           // Step 2 = UNREACHABLE.
                           {1, t2, t3, true, 0, 0, 1, true, 0, 0, false},

                           /*
                           ** Host 2.
                           */
                           // Start = UP.
                           {2, t0, t1, false, t2, t3, 0, true, 0, 0, false},
                           // Step 2 = DOWNTIME.
                           {2, t2, t3, false, t3, t4, 0, true, 0, 0, true},
                           // Step 3 = DOWN, step 4 = ACK (STICKY).
                           {2, t3, t4, true, 0, 0, 1, false, t4, t5, true},

                           /*
                           ** Host 3.
                           */
                           // Start = UP.
                           {3, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 4.
                           */
                           // Start = UP.
                           {4, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 5.
                           */
                           // Start = UP.
                           {5, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 6.
                           */
                           // Start = UP.
                           {6, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 7.
                           */
                           // Start = UP.
                           {7, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 8.
                           */
                           // Start = UP.
                           {8, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 9.
                           */
                           // Start = UP.
                           {9, t0, t1, true, 0, 0, 0, true, 0, 0, false},

                           /*
                           ** Host 10.
                           */
                           // Start = UP.
                           {10, t0, t1, true, 0, 0, 0, true, 0, 0, false}};

      // Get host state events.
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT host_id, start_time, end_time, state,"
                  "       ack_time, in_downtime"
                  " FROM rt_hoststateevents"
                  " ORDER BY host_id, start_time"))
        throw(exceptions::msg()
              << "cannot get host state events: " << q.lastError().text());

      // Compare DB with expected content.
      for (unsigned int i(0); i < sizeof(entries) / sizeof(*entries); ++i) {
        // Get next entry.
        if (!q.next())
          throw(exceptions::msg()
                << "not enough host state events in DB: got " << i
                << ", expected " << (sizeof(entries) / sizeof(*entries)));

        // Match entry.
        if ((q.value(0).toUInt() != entries[i].host_id) ||
            (static_cast<time_t>(q.value(1).toLongLong()) <
             entries[i].start_time_low) ||
            (static_cast<time_t>(q.value(1).toLongLong()) >
             entries[i].start_time_high) ||
            (entries[i].end_time_is_null && !q.value(2).isNull()) ||
            (!entries[i].end_time_is_null &&
             ((static_cast<time_t>(q.value(2).toLongLong()) <
               entries[i].end_time_low) ||
              (static_cast<time_t>(q.value(2).toLongLong()) >
               entries[i].end_time_high))) ||
            (q.value(3).toInt() != entries[i].state) ||
            (entries[i].ack_time_is_null && !q.value(4).isNull()) ||
            (!entries[i].ack_time_is_null &&
             ((static_cast<time_t>(q.value(4).toLongLong()) <
               entries[i].ack_time_low) ||
              (static_cast<time_t>(q.value(4).toLongLong()) >
               entries[i].ack_time_high))) ||
            (static_cast<bool>(q.value(5).toInt()) != entries[i].in_downtime)) {
          exceptions::msg e;
          e << "invalid host state event entry #" << i << ": got (host id "
            << q.value(0).toUInt() << ", start time " << q.value(1).toUInt()
            << ", end time "
            << (q.value(2).isNull() ? "null" : q.value(2).toString())
            << ", state " << q.value(3).toInt() << ", ack time "
            << (q.value(4).isNull() ? "null" : q.value(4).toString())
            << ", in downtime " << q.value(5).toInt() << "), expected ("
            << entries[i].host_id << ", " << entries[i].start_time_low << ":"
            << entries[i].start_time_high << ", ";
          if (entries[i].end_time_is_null)
            e << "null";
          else
            e << entries[i].end_time_low << ":" << entries[i].end_time_high;
          e << ", " << entries[i].state << ", ";
          if (entries[i].ack_time_is_null)
            e << "null";
          else
            e << entries[i].ack_time_low << ":" << entries[i].ack_time_high;
          e << ", " << entries[i].in_downtime << ")";
          throw(e);
        }
      }

      // No more results.
      if (q.next())
        throw(exceptions::msg() << "too much host state events in DB");
    }

    // Check service state events.
    {
      struct {
        unsigned int host_id;
        unsigned int service_id;
        time_t start_time_low;
        time_t start_time_high;
        bool end_time_is_null;
        time_t end_time_low;
        time_t end_time_high;
        short state;
        bool ack_time_is_null;
        time_t ack_time_low;
        time_t ack_time_high;
        bool in_downtime;
      } const entries[] = {
          /*
          ** Service 1-1.
          */
          // Start = OK.
          {1, 1, t0, t1, false, t2, t3, 0, true, 0, 0, false},
          // Step 2 = WARNING.
          {1, 1, t2, t3, false, t3, t4, 1, true, 0, 0, false},
          // Step 3 = DOWNTIME.
          {1, 1, t3, t4, true, 0, 0, 1, true, 0, 0, true},

          /*
          ** Service 1-2.
          */
          // Start = OK.
          {1, 2, t0, t1, false, t2, t3, 0, true, 0, 0, false},
          // Step 2 = CRITICAL, step 3 = ACK (STICKY).
          {1, 2, t2, t3, false, t4, t5, 2, false, t3, t4, false},
          // Step 4 = WARNING.
          {1, 2, t4, t5, true, 0, 0, 1, false, t4, t5, false},

          /*
          ** Service 2-3.
          */
          // Start = OK.
          {2, 3, t0, t1, false, t2, t3, 0, true, 0, 0, false},
          // Step 2 = CRITICAL, step 3 = ACK (STICKY).
          {2, 3, t2, t3, true, 0, 0, 2, false, t3, t4, false},

          /*
          ** Service 2-4.
          */
          // Start = OK.
          {2, 4, t0, t1, false, t2, t3, 0, true, 0, 0, false},
          // Step 2 = WARNING, step 3 = ACK (NORMAL).
          {2, 4, t2, t3, false, t4, t5, 1, false, t3, t4, false},
          // Step 4 = DOWNTIME.
          {2, 4, t4, t5, true, 0, 0, 1, false, t4, t5, true},

          /*
          ** Service 3-5.
          */
          // Start = OK.
          {3, 5, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 3-6.
          */
          // Start = OK.
          {3, 6, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 4-7.
          */
          // Start = OK.
          {4, 7, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 4-8.
          */
          // Start = OK.
          {4, 8, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 5-9.
          */
          // Start = OK.
          {5, 9, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 5-10.
          */
          // Start = OK.
          {5, 10, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 6-11.
          */
          // Start = OK.
          {6, 11, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 6-12.
          */
          // Start = OK.
          {6, 12, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 7-13.
          */
          // Start = OK.
          {7, 13, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 7-14.
          */
          // Start = OK.
          {7, 14, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 8-15.
          */
          // Start = OK.
          {8, 15, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 8-16.
          */
          // Start = OK.
          {8, 16, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 9-17.
          */
          // Start = OK.
          {9, 17, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 9-18.
          */
          // Start = OK.
          {9, 18, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 10-19.
          */
          // Start = OK.
          {10, 19, t0, t1, true, 0, 0, 0, true, 0, 0, false},

          /*
          ** Service 10-20.
          */
          // Start = OK.
          {10, 20, t0, t1, true, 0, 0, 0, true, 0, 0, false}};

      // Get service state events.
      QSqlQuery q(*db.centreon_db());
      if (!q.exec("SELECT host_id, service_id, start_time, end_time,"
                  "       state, ack_time, in_downtime"
                  " FROM rt_servicestateevents"
                  " ORDER BY host_id, service_id, start_time"))
        throw(exceptions::msg()
              << "cannot get service state events: " << q.lastError().text());

      // Compare DB with expected content.
      for (unsigned int i(0); i < sizeof(entries) / sizeof(*entries); ++i) {
        // Get next entry.
        if (!q.next())
          throw(exceptions::msg()
                << "not enough service state events in DB: got " << i
                << ", expected " << (sizeof(entries) / sizeof(*entries)));

        // Match entry.
        if ((q.value(0).toUInt() != entries[i].host_id) ||
            (q.value(1).toUInt() != entries[i].service_id) ||
            (static_cast<time_t>(q.value(2).toLongLong()) <
             entries[i].start_time_low) ||
            (static_cast<time_t>(q.value(2).toLongLong()) >
             entries[i].start_time_high) ||
            (entries[i].end_time_is_null && !q.value(3).isNull()) ||
            (!entries[i].end_time_is_null &&
             ((static_cast<time_t>(q.value(3).toLongLong()) <
               entries[i].end_time_low) ||
              (static_cast<time_t>(q.value(3).toLongLong()) >
               entries[i].end_time_high))) ||
            (q.value(4).toInt() != entries[i].state) ||
            (entries[i].ack_time_is_null && !q.value(5).isNull()) ||
            (!entries[i].ack_time_is_null &&
             ((static_cast<time_t>(q.value(5).toLongLong()) <
               entries[i].ack_time_low) ||
              (static_cast<time_t>(q.value(5).toLongLong()) >
               entries[i].ack_time_high))) ||
            (static_cast<bool>(q.value(6).toInt()) != entries[i].in_downtime)) {
          exceptions::msg e;
          e << "invalid service state event entry #" << i << ": got (host id "
            << q.value(0).toUInt() << ", service id " << q.value(1).toUInt()
            << ", start time " << q.value(2).toUInt() << ", end time "
            << (q.value(3).isNull() ? "null" : q.value(3).toString())
            << ", state " << q.value(4).toInt() << ", ack time "
            << (q.value(5).isNull() ? "null" : q.value(5).toString())
            << ", in downtime " << q.value(6).toInt() << "), expected ("
            << entries[i].host_id << ", " << entries[i].service_id << ", "
            << entries[i].start_time_low << ":" << entries[i].start_time_high
            << ", ";
          if (entries[i].end_time_is_null)
            e << "null";
          else
            e << entries[i].end_time_low << ":" << entries[i].end_time_high;
          e << ", " << entries[i].state << ", ";
          if (entries[i].ack_time_is_null)
            e << "null";
          else
            e << entries[i].ack_time_low << ":" << entries[i].ack_time_high;
          e << ", " << entries[i].in_downtime << ")";
          throw(e);
        }
      }

      // No more results.
      if (q.next())
        throw(exceptions::msg() << "too much service state events in DB");
    }

    // Stop daemons.
    daemon.stop();
    sleep_for(2);
    broker.stop();
    sleep_for(2);

    // Check passive correlation.
    // XXX

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
