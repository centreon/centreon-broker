/*
** Copyright 2014 Merethis
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

#include <cmath>
#include <cstdio>
#include <cstdlib>
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

#define CENTREON_DB_NAME "broker_bam_centreon"
#define BI_DB_NAME "broker_bam_bi"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 10

typedef struct {
  double current_level;
  double downtime;
  double acknowledged;
} ba_state;

typedef struct {
  short state_type;
  short state;
  double last_level;
  double downtime;
  double acknowledged;
} kpi_state;

typedef struct {
  bool state;
} boolexp_state;

typedef struct {
  unsigned int ba_id;
  time_t start_time_low;
  time_t start_time_high;
  time_t end_time_low;
  time_t end_time_high;
  short status;
  bool in_downtime;
} ba_event;

typedef struct {
  unsigned int kpi_id;
  time_t start_time_low;
  time_t start_time_high;
  time_t end_time_low;
  time_t end_time_high;
  short status;
  bool in_downtime;
  short impact;
  std::string output;
  std::string perfdata;
} kpi_event;

/**
 *  Compare two double values.
 */
static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
}

/**
 *  Check content of the mod_bam table.
 */
static void check_bas(
              QSqlDatabase& db,
              ba_state const* bas,
              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT ba_id, current_level, downtime, acknowledged"
            "  FROM mod_bam"
            "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch BAs at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BAs at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (!double_equals(q.value(1).toDouble(), bas[i].current_level)
        || !double_equals(q.value(2).toDouble(), bas[i].downtime)
        || !double_equals(q.value(3).toDouble(), bas[i].acknowledged))
      throw (exceptions::msg() << "invalid BA " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (level "
             << q.value(1).toDouble() << ", downtime "
             << q.value(2).toDouble() << ", acknowledged "
             << q.value(3).toDouble() << "), expected ("
             << bas[i].current_level << ", " << bas[i].downtime << ", "
             << bas[i].acknowledged << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BAs at iteration "
           << iteration << ": expected " << count);
  return ;
}

/**
 *  Check content of the mod_bam_kpi table.
 */
static void check_kpis(
              QSqlDatabase& db,
              kpi_state const* kpis,
              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT kpi_id, state_type, current_status, last_level,"
            "       downtime, acknowledged"
            "  FROM mod_bam_kpi"
            "  ORDER BY kpi_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch KPIs at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough KPIs at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if ((q.value(1).toInt() != kpis[i].state_type)
        || (q.value(2).toInt() != kpis[i].state)
        || !double_equals(q.value(3).toDouble(), kpis[i].last_level)
        || !double_equals(q.value(4).toDouble(), kpis[i].downtime)
        || !double_equals(q.value(5).toDouble(), kpis[i].acknowledged))
      throw (exceptions::msg() << "invalid KPI " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (state type "
             << q.value(1).toInt() << ", state "
             << q.value(2).toInt() << ", level "
             << q.value(3).toDouble() << ", downtime "
             << q.value(4).toDouble() << ", acknowledged "
             << q.value(5).toDouble() << "), expected ("
             << kpis[i].state_type << ", " << kpis[i].state
             << ", " << kpis[i].last_level << ", " << kpis[i].downtime
             << ", " << kpis[i].acknowledged << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much KPIs at iteration "
           << iteration << ": expected " << count);
  return ;
}

/**
 *  Check content of the mod_bam_boolean table.
 */
static void check_boolexps(
              QSqlDatabase& db,
              boolexp_state const* boolexps,
              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT boolean_id, current_state"
            "  FROM mod_bam_boolean"
            "  ORDER BY boolean_id ASC");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch boolexps at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough boolexps at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (static_cast<bool>(q.value(1).toInt()) != boolexps[i].state)
      throw (exceptions::msg() << "invalid boolexp "
             << q.value(0).toUInt() << " at iteration " << iteration
             << ": got (state " << static_cast<bool>(q.value(1).toInt())
             << "), expected (" << boolexps[i].state << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much boolexps at iteration "
           << iteration << ": expected " << count);
  return ;
}

/**
 *  Check the content of the ba_events table.
 */
static void check_ba_events(
              QSqlDatabase& db,
              ba_event const* ba_events,
              size_t count) {
  QString query(
            "SELECT ba_id, start_time, end_time, status, in_downtime"
            "  FROM ba_events"
            "  ORDER BY ba_id, start_time");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch BA events: "
           << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA events: got " << i
             << ", expected " << count);
    if ((q.value(0).toUInt() != ba_events[i].ba_id)
        || (q.value(1).toLongLong() < ba_events[i].start_time_low)
        || (q.value(1).toLongLong() > ba_events[i].start_time_high)
        || (q.value(2).toLongLong() > ba_events[i].end_time_low)
        || (q.value(2).toLongLong() < ba_events[i].end_time_high)
        || (q.value(3).toInt() != ba_events[i].status)
        || (static_cast<bool>(q.value(4).toInt())
            != ba_events[i].in_downtime))
      throw (exceptions::msg() << "invalid BA event: got (BA ID "
             << q.value(0).toUInt() << ", start time "
             << q.value(1).toLongLong() << ", end time "
             << q.value(2).toLongLong() << ", status "
             << q.value(3).toInt() << ", in downtime "
             << q.value(4).toInt() << "), expected ("
             << ba_events[i].ba_id << ", "
             << ba_events[i].start_time_low << "-"
             << ba_events[i].start_time_high << ", "
             << ba_events[i].end_time_low << "-"
             << ba_events[i].end_time_high << ", "
             << ba_events[i].status << ", " << ba_events[i].in_downtime
             << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA events: expected "
           << count);
  return ;
}

/**
 *  Check the content of the kpi_events and relations_ba_kpi_events
 *  tables.
 */
static void check_kpi_events(
              QSqlDatabase& db,
              kpi_event const* kpi_events,
              size_t count) {
}

/**
 *  Check functionnally the BAM engine.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  external_command commander;
  engine monitoring;
  test_db db;

  try {
    // Prepare database.
    db.open(CENTREON_DB_NAME, BI_DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
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
          << PROJECT_SOURCE_DIR << "/test/cfg/bam.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // Create instance entry.
    {
      QString query(
                "INSERT INTO instances (instance_id, name)"
                "  VALUES (42, 'MyBroker')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create instance: "
               << q.lastError().text());
    }

    // Create host/service entries.
    {

      for (int i(1); i <= HOST_COUNT; ++i) {
        {
          std::ostringstream oss;
          oss << "INSERT INTO hosts (host_id, name, instance_id)"
              << "  VALUES (" << i << ", '" << i << "', 42)";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw (exceptions::msg() << "could not create host "
                   << i << ": " << q.lastError().text());
        }
        for (int j((i - 1) * SERVICES_BY_HOST + 1), limit(i * SERVICES_BY_HOST);
             j < limit;
             ++j) {
          std::ostringstream oss;
          oss << "INSERT INTO services (host_id, description, service_id)"
              << "  VALUES (" << i << ", '" << j << "', " << j << ")";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw (exceptions::msg() << "could not create service ("
                   << i << ", " << j << "): " << q.lastError().text());
        }
      }
    }

    // Create BAs.
    {
      QString query(
                "INSERT INTO mod_bam (ba_id, name, level_w, level_c)"
                "  VALUES (1, 'BA1', 90, 80),"
                "         (2, 'BA2', 80, 70),"
                "         (3, 'BA3', 70, 60),"
                "         (4, 'BA4', 60, 50),"
                "         (5, 'BA5', 50, 40),"
                "         (6, 'BA6', 40, 30),"
                "         (7, 'BA7', 30, 20),"
                "         (8, 'BA8', 20, 10),"
                "         (9, 'BA9', 10, 0)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BAs: "
               << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query(
                "INSERT INTO mod_bam_kpi (kpi_id, kpi_type, host_id,"
                "            service_id, id_indicator_ba, id_ba,"
                "            meta_id, config_type, drop_warning,"
                "            drop_warning_impact_id, drop_critical,"
                "            drop_critical_impact_id, drop_unknown,"
                "            drop_unknown_impact_id, ignore_downtime,"
                "            ignore_acknowledged)"
                "  VALUES (1, '0', 1, 1, NULL, 2, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0'),"
                "         (2, '0', 1, 2, NULL, 3, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (3, '0', 1, 3, NULL, 3, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (4, '0', 1, 4, NULL, 4, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0'),"
                "         (5, '0', 1, 5, NULL, 4, NULL, '0', 26, NULL, 35, NULL, 99, NULL, '0', '0'),"
                "         (6, '0', 1, 6, NULL, 4, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (7, '0', 1, 7, NULL, 5, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0'),"
                "         (8, '0', 1, 8, NULL, 5, NULL, '0', 26, NULL, 35, NULL, 99, NULL, '0', '0'),"
                "         (9, '0', 1, 9, NULL, 5, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (10, '0', 1, 10, NULL, 5, NULL, '0', 45, NULL, 55, NULL, 99, NULL, '0', '0'),"
                "         (11, '1', NULL, NULL, 2, 6, NULL, '0', 65, NULL, 75, NULL, 99, NULL, '0', '0'),"
                "         (12, '1', NULL, NULL, 3, 7, NULL, '0', 25, NULL, 35, NULL, 99, NULL, '0', '0'),"
                "         (13, '1', NULL, NULL, 4, 7, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (14, '1', NULL, NULL, 5, 7, NULL, '0', 45, NULL, 55, NULL, 99, NULL, '0', '0'),"
                "         (15, '1', NULL, NULL, 6, 8, NULL, '0', 85, NULL, 95, NULL, 99, NULL, '0', '0'),"
                "         (16, '1', NULL, NULL, 7, 8, NULL, '0', 95, NULL, 105, NULL, 99, NULL, '0', '0')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create KPIs: "
               << q.lastError().text());
    }

    // Create boolean expressions.
    {
      QString query(
                "INSERT INTO mod_bam_boolean (boolean_id, name,"
                "            config_type, impact, impact_id,"
                "            expression, bool_state)"
                "  VALUES (1, 'BoolExp1', 0, 75, NULL, '{1 1} {is} {OK}', 0),"
                "         (2, 'BoolExp2', 0, 25, NULL, '{1 2} {not} {CRITICAL} {OR} {1 3} {not} {OK}', 1),"
                "         (3, 'BoolExp3', 0, 6, NULL, '({1 5} {not} {WARNING} {AND} {1 6} {is} {WARNING}) {OR} {1 7} {is} {CRITICAL}', 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create boolexps: "
               << q.lastError().text());
    }
    {
      QString query(
                "INSERT INTO mod_bam_bool_rel (ba_id, boolean_id)"
                "  VALUES (9, 1), (9, 2), (9, 3)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not link boolexps: "
               << q.lastError().text());
    }

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    // Let the daemon initialize.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #0
    {
      ba_state const bas[] = {
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 100.0, 0.0, 0.0 },
        { 75.0, 0.0, 0.0 }
      };
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }
    {
      boolexp_state const boolexps[] = {
        { true },
        { true },
        { false }
      };
      check_boolexps(
        *db.centreon_db(),
        boolexps,
        sizeof(boolexps) / sizeof(*boolexps));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output1 for (1, 1)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output1 for (1, 2)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;4;2;output1 for (1, 4)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;7;2;output1 for (1, 7)");

    // Sleep a while.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #1
    {
      ba_state const bas[] = {
        // Impacted by services.
        { 100.0, 0.0, 0.0 },
        { 75.0, 0.0, 0.0 },  // SVC1 C = 25 => W
        { 55.0, 0.0, 0.0 },  // SVC2 C = 45 => C
        { 75.0, 0.0, 0.0 },  // SVC4 C = 25 => O
        { 75.0, 0.0, 0.0 },  // SVC7 C = 25 => O
        // Impacted by BAs.
        { 35.0, 0.0, 0.0 },  // BA2 W  = 65 => W
        { 65.0, 0.0, 0.0 },  // BA3 C  = 35 => O
        { 15.0, 0.0, 0.0 },  // BA6 W  = 85 => W
        { 19.0, 0.0, 0.0 }   // BE1 F  = 75, BE3 T = 6 => W
      };
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 2, 45.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 1, 65.0, 0.0, 0.0 },
        { 1, 2, 35.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 1, 85.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }
    {
      boolexp_state const boolexps[] = {
        { false },
        { false },
        { true }
      };
      check_boolexps(
        *db.centreon_db(),
        boolexps,
        sizeof(boolexps) / sizeof(*boolexps));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;3;1;output2 for (1, 3)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;5;1;output2 for (1, 5)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;8;1;output2 for (1, 8)");

    // Sleep a while.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #2
    {
      ba_state const bas[] = {
        // Impacted by services.
        { 100.0, 0.0, 0.0 },
        { 75.0, 0.0, 0.0 },  // SVC1 C = 25 => W
        { 20.0, 0.0, 0.0 },  // SVC2 C = 45, SVC3 W = 35 => C
        { 49.0, 0.0, 0.0 },  // SVC4 C = 25, SVC5 W = 26 => C
        { 49.0, 0.0, 0.0 },  // SVC7 C = 25, SVC8 W = 26 => W
        // Impacted by BAs.
        { 35.0, 0.0, 0.0 },  // BA2 W  = 65 => W
        { 0.0, 0.0, 0.0 },   // BA3 C  = 35, BA4 C  = 45, BA5 W = 45 => C
        { 0.0, 0.0, 0.0 },   // BA6 W  = 85 => W, BA7 C = 105 => C
        { 0.0, 0.0, 0.0 }    // BE1 F  = 75, BE2 T = 25, BE3 T = 6 => C
      };
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 2, 45.0, 0.0, 0.0 },
        { 1, 1, 35.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 1, 26.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 1, 26.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 1, 65.0, 0.0, 0.0 },
        { 1, 2, 35.0, 0.0, 0.0 },
        { 1, 2, 45.0, 0.0, 0.0 },
        { 1, 1, 45.0, 0.0, 0.0 },
        { 1, 1, 85.0, 0.0, 0.0 },
        { 1, 2, 105.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }
    {
      boolexp_state const boolexps[] = {
        { false },
        { true },
        { true }
      };
      check_boolexps(
        *db.centreon_db(),
        boolexps,
        sizeof(boolexps) / sizeof(*boolexps));
    }
    {
      ba_event const baevents[] = {
        { 1, 0, 0, 0, 0, 0, false }
      };
      check_ba_events(
        *db.bi_db(),
        baevents,
        sizeof(baevents) / sizeof(*baevents));
    }
    {
      kpi_event const kpievents[] = {
        { 1, 0, 0, 0, 0, 0, false, 0, "", ""}
      };
      check_kpi_events(
        *db.bi_db(),
        kpievents,
        sizeof(kpievents) / sizeof(*kpievents));
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
  monitoring.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
