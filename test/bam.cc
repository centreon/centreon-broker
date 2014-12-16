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
#define BA_COUNT 9
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
  unsigned int ba_id;
  time_t start_time_low;
  time_t start_time_high;
  bool end_time_null;
  time_t end_time_low;
  time_t end_time_high;
  short status;
  bool in_downtime;
} ba_event;

typedef struct {
  bool optional;
  unsigned int kpi_id;
  time_t start_time_low;
  time_t start_time_high;
  bool end_time_null;
  time_t end_time_low;
  time_t end_time_high;
  short status;
  bool in_downtime;
  short impact;
  std::string output;
  std::string perfdata;
} kpi_event;

typedef struct {
  unsigned int ba_id;
  time_t start_time_low;
  time_t start_time_high;
  time_t end_time_low;
  time_t end_time_high;
  long duration_low;
  long duration_high;
  long sla_duration_low;
  long sla_duration_high;
  bool timeperiod_is_default;
} ba_event_duration;

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
 *  Check the content of the ba_events table.
 */
static void check_ba_events(
              QSqlDatabase& db,
              ba_event const* ba_events,
              size_t count) {
  QString query(
            "SELECT ba_id, start_time, end_time, status, in_downtime"
            "  FROM mod_bam_reporting_ba_events"
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
        || (q.value(2).isNull() && !ba_events[i].end_time_null)
        || (!q.value(2).isNull()
            && (ba_events[i].end_time_null
                || (q.value(2).toLongLong() < ba_events[i].end_time_low)
                || (q.value(2).toLongLong() > ba_events[i].end_time_high)))
        || (q.value(3).toInt() != ba_events[i].status)
        || (static_cast<bool>(q.value(4).toInt())
            != ba_events[i].in_downtime))
      throw (exceptions::msg() << "invalid BA event: got (BA ID "
             << q.value(0).toUInt() << ", start time "
             << q.value(1).toLongLong() << ", end time "
             << q.value(2).toLongLong() << ", status "
             << q.value(3).toInt() << ", in downtime "
             << q.value(4).toBool() << "), expected ("
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
  QString query(
            "SELECT kpi_id, start_time, end_time, status, in_downtime,"
            "       impact_level, first_output, first_perfdata"
            "  FROM mod_bam_reporting_kpi_events"
            "  ORDER BY kpi_id, start_time");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch KPI events: "
           << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough KPI events: got " << i
             << ", expected " << count);
    while ((q.value(0).toUInt() != kpi_events[i].kpi_id)
           || (q.value(1).toLongLong() < kpi_events[i].start_time_low)
           || (q.value(1).toLongLong() > kpi_events[i].start_time_high)
           || (q.value(2).isNull() && !kpi_events[i].end_time_null)
           || (!q.value(2).isNull()
               && (kpi_events[i].end_time_null
                   || (q.value(2).toLongLong() < kpi_events[i].end_time_low)
                   || (q.value(2).toLongLong() > kpi_events[i].end_time_high)))
           || (q.value(3).toInt() != kpi_events[i].status)
           || (q.value(4).toBool() != kpi_events[i].in_downtime)
           || (q.value(5).toInt() != kpi_events[i].impact)
           || (q.value(6).toString().toStdString() != kpi_events[i].output)
           || (q.value(7).toString().toStdString() != kpi_events[i].perfdata)) {
      if (kpi_events[i].optional)
        ++i;
      else
        throw (exceptions::msg() << "invalid KPI event: got (KPI ID "
               << q.value(0).toUInt() << ", start time "
               << q.value(1).toLongLong() << ", end time "
               << q.value(2).toLongLong() << ", status "
               << q.value(3).toInt() << ", in downtime "
               << q.value(4).toBool() << ", impact " << q.value(5).toInt()
               << ", output '" << q.value(6).toString().toStdString()
               << "', perfdata '" << q.value(7).toString().toStdString()
               << "'), expected (" << kpi_events[i].kpi_id << ", "
               << kpi_events[i].start_time_low << "-"
               << kpi_events[i].start_time_high << ", "
               << kpi_events[i].end_time_low << "-"
               << kpi_events[i].end_time_high << ", "
               << kpi_events[i].status << ", "
               << kpi_events[i].in_downtime << ", "
               << kpi_events[i].impact << ", '"
               << kpi_events[i].output << "', '"
               << kpi_events[i].perfdata << "')");
    }
  }
  if (q.next())
    throw (exceptions::msg() << "too much KPI events: expected "
           << count);
  // XXX : check relations
  return ;
}

/**
 *  Check the content of the mod_bam_reporting_ba_events_durations
 *  table.
 */
static void check_ba_events_durations(
              QSqlDatabase& db,
              ba_event_duration const* ba_durations,
              size_t count) {
  QString query(
            "SELECT e.ba_id, d.start_time, d.end_time, d.duration,"
            "       d.sla_duration, d.timeperiod_is_default"
            "  FROM mod_bam_reporting_ba_events_durations AS d"
            "  LEFT JOIN mod_bam_reporting_ba_events AS e"
            "    ON d.ba_event_id=e.ba_event_id"
            "  ORDER BY e.ba_id ASC, d.start_time ASC");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch BA events durations: "
           << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA events durations: got "
             << i << ", expected " << count);
    if ((q.value(0).toUInt() != ba_durations[i].ba_id)
        || (q.value(1).toLongLong() < ba_durations[i].start_time_low)
        || (q.value(1).toLongLong() > ba_durations[i].start_time_high)
        || (q.value(2).toLongLong() < ba_durations[i].end_time_low)
        || (q.value(2).toLongLong() > ba_durations[i].end_time_high)
        || (q.value(3).toLongLong() < ba_durations[i].duration_low)
        || (q.value(3).toLongLong() > ba_durations[i].duration_high)
        || (q.value(4).toLongLong() < ba_durations[i].sla_duration_low)
        || (q.value(4).toLongLong() > ba_durations[i].sla_duration_high)
        || (q.value(5).toBool() != ba_durations[i].timeperiod_is_default))
      throw (exceptions::msg()
             << "invalid BA event duration: got (BA ID "
             << q.value(0).toUInt() << ", start time "
             << q.value(1).toLongLong() << ", end time "
             << q.value(2).toLongLong() << ", duration "
             << q.value(3).toLongLong() << ", SLA duration "
             << q.value(4).toLongLong() << ", timeperiod is default "
             << q.value(5).toBool() << "), expected ("
             << ba_durations[i].ba_id << ", "
             << ba_durations[i].start_time_low << "-"
             << ba_durations[i].start_time_high << ", "
             << ba_durations[i].end_time_low << "-"
             << ba_durations[i].end_time_high << ", "
             << ba_durations[i].duration_low << "-"
             << ba_durations[i].duration_high << ", "
             << ba_durations[i].sla_duration_low << "-"
             << ba_durations[i].sla_duration_high << ", "
             << ba_durations[i].timeperiod_is_default << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA events duration: expected "
           << count);
  return ;
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
  std::list<command> commands;
  std::string engine_config_path(tmpnam(NULL));
  external_command commander;
  engine monitoring;
  test_db db;
  test_file cfg;

  try {
    // Prepare database.
    db.open(NULL, BI_DB_NAME, CENTREON_DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    {
      host h;
      memset(&h, 0, sizeof(h));
      char const* str("virtual_ba_host");
      h.name = new char[strlen(str) + 1];
      strcpy(h.name, str);
      str = "1001";
      h.display_name = new char[strlen(str) + 1];
      strcpy(h.display_name, str);
      h.accept_passive_host_checks = 0;
      h.checks_enabled = 0;
      hosts.push_back(h);
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
    for (int i(1); i <= BA_COUNT; ++i) {
      service s;
      memset(&s, 0, sizeof(s));
      {
        char const* host_name("virtual_ba_host");
        s.host_name = new char[strlen(host_name) + 1];
        strcpy(s.host_name, host_name);
      }
      {
        std::ostringstream oss;
        oss << "ba_" << i;
        std::string str(oss.str());
        s.description = new char[str.size() + 1];
        strcpy(s.description, str.c_str());
      }
      {
        std::ostringstream oss;
        oss << i + 1000;
        std::string str(oss.str());
        s.display_name = new char[str.size() + 1];
        strcpy(s.display_name, str.c_str());
      }
      {
        std::ostringstream oss;
        oss << "1!" << i;
        std::string str(oss.str());
        s.service_check_command = new char[str.size() + 1];
        strcpy(s.service_check_command, str.c_str());
      }
      s.accept_passive_service_checks = 1;
      s.checks_enabled = 0;
      s.max_attempts = 1;
      services.push_back(s);
    }
    generate_commands(commands, 1);
    {
      char const* cmdline;
      cmdline = MY_PLUGIN_BAM_PATH " " CENTREON_DB_NAME " $ARG1$";
      command& cmd(commands.front());
      cmd.command_line = new char[strlen(cmdline) + 1];
      strcpy(cmd.command_line, cmdline);
    }

    commander.set_file(tmpnam(NULL));
    cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    cfg.set("COMMAND_FILE", commander.get_file());
    cfg.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    cfg.set("DB_NAME_BI", BI_DB_NAME);
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << cfg.generate() << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      &commands);

    // Create timeperiods.
    {
      QString query;
      query = "INSERT INTO timeperiod (tp_id, tp_name, tp_alias, "
              "            tp_sunday, tp_monday, tp_tuesday,"
              "            tp_wednesday, tp_thursday, tp_friday, "
              "            tp_saturday)"
              "  VALUES (1, '24x7', '24x7', '00:00-24:00',"
              "          '00:00-24:00', '00:00-24:00', '00:00-24:00',"
              "          '00:00-24:00', '00:00-24:00', '00:00-24:00')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create timeperiods: "
               << q.lastError().text());
    }

    // Create host/service entries.
    {

      for (int i(1); i <= HOST_COUNT; ++i) {
        {
          std::ostringstream oss;
          oss << "INSERT INTO host (host_id, host_name)"
              << "  VALUES (" << i << ", '" << i << "')";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw (exceptions::msg() << "could not create host "
                   << i << ": " << q.lastError().text());
        }
        for (int j((i - 1) * SERVICES_BY_HOST + 1), limit(i * SERVICES_BY_HOST);
             j < limit;
             ++j) {
          {
            std::ostringstream oss;
            oss << "INSERT INTO service (service_id, service_description)"
                << "  VALUES (" << j << ", '" << j << "')";
            QSqlQuery q(*db.centreon_db());
            if (!q.exec(oss.str().c_str()))
              throw (exceptions::msg() << "could not create service ("
                     << i << ", " << j << "): "
                     << q.lastError().text());
          }
          {
            std::ostringstream oss;
            oss << "INSERT INTO host_service_relation (host_host_id, service_service_id)"
                << "  VALUES (" << i << ", " << j << ")";
            QSqlQuery q(*db.centreon_db());
            if (!q.exec(oss.str().c_str()))
              throw (exceptions::msg() << "could not link service "
                     << j << " to host " << i << ": "
                     << q.lastError().text());
          }
        }
      }
    }

    // Create BAs.
    {
      {
        QString query(
                  "INSERT INTO mod_bam (ba_id, name, level_w, level_c,"
                  "            activate, id_reporting_period)"
                  "  VALUES (1, 'BA1', 90, 80, '1', 1),"
                  "         (2, 'BA2', 80, 70, '1', 1),"
                  "         (3, 'BA3', 70, 60, '1', 1),"
                  "         (4, 'BA4', 60, 50, '1', 1),"
                  "         (5, 'BA5', 50, 40, '1', 1),"
                  "         (6, 'BA6', 40, 30, '1', 1),"
                  "         (7, 'BA7', 30, 21, '1', 1),"
                  "         (8, 'BA8', 20, 10, '1', 1),"
                  "         (9, 'BA9', 10, 0, '1', 1)");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw (exceptions::msg() << "could not create BAs: "
                 << q.lastError().text());
      }
      // {
      //   QString query(
      //             "INSERT INTO mod_bam_ba_tp_rel (ba_id, timeperiod_id,"
      //             "            is_default)"
      //             "  VALUES (1, 1, 1),"
      //             "         (2, 1, 1),"
      //             "         (3, 1, 1),"
      //             "         (4, 1, 1),"
      //             "         (5, 1, 1),"
      //             "         (6, 1, 1),"
      //             "         (7, 1, 1),"
      //             "         (8, 1, 1),"
      //             "         (9, 1, 1)");
      //   QSqlQuery q(*db.centreon_db());
      //   if (!q.exec(query))
      //     throw (exceptions::msg()
      //            << "could not link BAs to timeperiods: "
      //            << q.lastError().text());
      // }

      // Create associated services.
      {
        QString query(
                  "INSERT INTO host (host_id, host_name)"
                  "  VALUES (1001, 'virtual_ba_host')");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw (exceptions::msg()
                 << "could not create virtual BA host: "
                 << q.lastError().text());
      }
      for (int i(1); i <= 9; ++i) {
        {
          std::ostringstream oss;
          oss << "INSERT INTO service (service_id, service_description)"
              << "  VALUES (" << 1000 + i << ", 'ba_" << i << "')";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw (exceptions::msg()
                   << "could not create virtual service of BA "
                   << i << ": " << q.lastError().text());
        }
        {
          std::ostringstream oss;
          oss << "INSERT INTO host_service_relation (host_host_id, "
              << "            service_service_id)"
              << "  VALUES (1001, " << 1000 + i << ")";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw (exceptions::msg()
                   << "could not create link between virtual host "
                   << "and virtual service of BA " << i << ": "
                   << q.lastError().text());
        }
      }
    }

    // Create boolean expressions.
    {
      QString query(
                "INSERT INTO mod_bam_boolean (boolean_id, name,"
                "            expression, bool_state, activate)"
                "  VALUES (1, 'BoolExp1', '{1 1} {is} {OK}', 0, 1),"
                "         (2, 'BoolExp2', '{1 2} {not} {CRITICAL} {OR} {1 3} {not} {OK}', 1, 1),"
                "         (3, 'BoolExp3', '({1 5} {not} {WARNING} {AND} {1 6} {is} {WARNING}) {OR} {1 7} {is} {CRITICAL}', 1, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create boolexps: "
               << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query(
                "INSERT INTO mod_bam_kpi (kpi_id, kpi_type, host_id,"
                "            service_id, id_indicator_ba, id_ba,"
                "            meta_id, boolean_id, config_type,"
                "            drop_warning, drop_warning_impact_id,"
                "            drop_critical, drop_critical_impact_id,"
                "            drop_unknown, drop_unknown_impact_id,"
                "            ignore_downtime, ignore_acknowledged,"
                "            state_type, activate)"
                "  VALUES (1, '0', 1, 1, NULL, 2, NULL, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (2, '0', 1, 2, NULL, 3, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (3, '0', 1, 3, NULL, 3, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (4, '0', 1, 4, NULL, 4, NULL, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (5, '0', 1, 5, NULL, 4, NULL, NULL, '0', 26, NULL, 35, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (6, '0', 1, 6, NULL, 4, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (7, '0', 1, 7, NULL, 5, NULL, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (8, '0', 1, 8, NULL, 5, NULL, NULL, '0', 26, NULL, 35, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (9, '0', 1, 9, NULL, 5, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (10, '0', 1, 10, NULL, 5, NULL, NULL, '0', 45, NULL, 55, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (11, '1', NULL, NULL, 2, 6, NULL, NULL, '0', 65, NULL, 75, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (12, '1', NULL, NULL, 3, 7, NULL, NULL, '0', 25, NULL, 35, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (13, '1', NULL, NULL, 4, 7, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (14, '1', NULL, NULL, 5, 7, NULL, NULL, '0', 45, NULL, 55, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (15, '1', NULL, NULL, 6, 8, NULL, NULL, '0', 85, NULL, 95, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (16, '1', NULL, NULL, 7, 8, NULL, NULL, '0', 95, NULL, 105, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (17, '3', NULL, NULL, NULL, 9, NULL, 1, '0', 10, NULL, 75, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (18, '3', NULL, NULL, NULL, 9, NULL, 2, '0', 10, NULL, 25, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (19, '3', NULL, NULL, NULL, 9, NULL, 3, '0', 10, NULL, 6, NULL, 99, NULL, '0', '0', '1', '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create KPIs: "
               << q.lastError().text());
    }

    // Start monitoring engine.
    time_t t0(time(NULL));
    std::cout << "T0: " << t0 << "\n";
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    // Let the daemon initialize.
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #0
    time_t t1(time(NULL));
    std::cout << "T1: " << t1 << "\n";
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
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;output1 for (1, 2)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output1 for (1, 1)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;4;2;output1 for (1, 4)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;7;2;output1 for (1, 7)");

    // Sleep a while.
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #1
    time_t t2(time(NULL));
    std::cout << "T2: " << t2 << "\n";
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
        { 19.0, 0.0, 0.0 }   // BE1 F  = 75, BE3 T = 6 => O
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
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 75.0, 0.0, 0.0 },
        { 1, 0, 0.0, 0.0, 0.0 },
        { 1, 2, 6.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;3;1;output2 for (1, 3)");
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;5;1;output2 for (1, 5)");
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;8;1;output2 for (1, 8)");

    // Sleep a while.
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // #2
    time_t t3(time(NULL));
    std::cout << "T3: " << t3 << "\n";
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
        { 1, 2, 105.0, 0.0, 0.0 },
        { 1, 2, 75.0, 0.0, 0.0 },
        { 1, 2, 25.0, 0.0, 0.0 },
        { 1, 2, 6.0, 0.0, 0.0 }
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }
    {
      ba_event const baevents[] = {
        { 1, t0, t1, true, 0, 0, 0, false },
        { 2, t0, t1, false, t1, t2, 0, false },
        { 2, t1, t2, true, 0, 0, 1, false },
        { 3, t0, t1, false, t1, t2, 0, false },
        { 3, t1, t2, true, 0, 0, 2, false },
        { 4, t0, t1, false, t2, t3, 0, false },
        { 4, t2, t3, true, 0, 0, 2, false },
        { 5, t0, t1, false, t2, t3, 0, false },
        { 5, t2, t3, true, 0, 0, 1, false },
        { 6, t0, t1, false, t1, t2, 0, false },
        { 6, t1, t2, true, 0, 0, 1, false },
        { 7, t0, t1, false, t2, t3, 0, false },
        { 7, t2, t3, true, 0, 0, 2, false },
        { 8, t0, t1, false, t1, t2, 0, false },
        { 8, t1, t2, false, t2, t3, 1, false },
        { 8, t2, t3, true, 0, 0, 2, false },
        { 9, t0, t1, false, t2, t3, 0, false },
        { 9, t2, t3, true, 0, 0, 2, false }
      };
      check_ba_events(
        *db.bi_db(),
        baevents,
        sizeof(baevents) / sizeof(*baevents));
    }
    {
      kpi_event const kpievents[] = {
        { false, 1, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 1, t1, t2, true, 0, 0, 2, false, 25, "output1 for (1, 1)\n", "" },
        { false, 2, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 2, t1, t2, true, 0, 0, 2, false, 45, "output1 for (1, 2)\n", "" },
        { false, 3, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 3, t2, t3, true, 0, 0, 1, false, 35, "output2 for (1, 3)\n", "" },
        { false, 4, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 4, t1, t2, true, 0, 0, 2, false, 25, "output1 for (1, 4)\n", "" },
        { false, 5, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 5, t2, t3, true, 0, 0, 1, false, 26, "output2 for (1, 5)\n", "" },
        { false, 6, t0, t1, true, 0, 0, 0, false, 0, "", "" },
        { false, 7, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 7, t1, t2, true, 0, 0, 2, false, 25, "output1 for (1, 7)\n", "" },
        { false, 8, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 8, t2, t3, true, 0, 0, 1, false, 26, "output2 for (1, 8)\n", "" },
        { false, 9, t0, t1, true, 0, 0, 0, false, 0, "", "" },
        { false, 10, t0, t1, true, 0, 0, 0, false, 0, "", "" },
        { false, 11, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 11, t1, t2, true, 0, 0, 1, false, 65, "BA 2 has state 1 and level 75\n", "BA_Level=75%" },
        { false, 12, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 12, t1, t2, true, 0, 0, 2, false, 35, "BA 3 has state 2 and level 55\n", "BA_Level=55%" },
        { false, 13, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 13, t2, t3, true, 0, 0, 2, false, 45, "BA 4 has state 2 and level 49\n", "BA_Level=49%" },
        { false, 14, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 14, t2, t3, true, 0, 0, 1, false, 45, "BA 5 has state 1 and level 49\n", "BA_Level=49%" },
        { false, 15, t0, t1, false, t1, t2, 0, false, 0, "", "" },
        { false, 15, t1, t2, true, 0, 0, 1, false, 85, "BA 6 has state 1 and level 35\n", "BA_Level=35%" },
        { false, 16, t0, t1, false, t2, t3, 0, false, 0, "", "" },
        { false, 16, t2, t3, true, 0, 0, 2, false, 105, "BA 7 has state 2 and level 20\n", "BA_Level=20%" },
        { false, 17, t0, t1, false, t1, t2, 0, false, 0, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 17, t1, t2, true, 0, 0, 2, false, 75, "BAM boolean expression computed by Centreon Broker", "" },
        { true, 18, t0, t1, false, t0, t1, 0, false, 0, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 18, t0, t1, false, t1, t2, 2, false, 25, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 18, t1, t2, false, t2, t3, 0, false, 0, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 18, t2, t3, true, 0, 0, 2, false, 25, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 19, t0, t1, false, t1, t2, 0, false, 0, "BAM boolean expression computed by Centreon Broker", "" },
        { false, 19, t1, t2, true, 0, 0, 2, false, 6, "BAM boolean expression computed by Centreon Broker", "" }
      };
      check_kpi_events(
        *db.bi_db(),
        kpievents,
        sizeof(kpievents) / sizeof(*kpievents));
    }
    {
      ba_event_duration const badurations[] = {
        { 2, t0, t1, t1, t2, 0, t2 - t0, 0, t2 - t0, true },
        { 3, t0, t1, t1, t2, 0, t2 - t0, 0, t2 - t0, true },
        { 4, t0, t1, t2, t3, t2 - t1, t3 - t0, t2 - t1, t3 - t0, true },
        { 5, t0, t1, t2, t3, t2 - t1, t3 - t0, t2 - t1, t3 - t0, true },
        { 6, t0, t1, t1, t2, 0, t2 - t0, 0, t2 - t0, true },
        { 7, t0, t1, t2, t3, t2 - t1, t3 - t0, t2 - t1, t3 - t0, true },
        { 8, t0, t1, t1, t2, 0, t2 - t0, 0, t2 - t0, true },
        { 8, t1, t2, t2, t3, 0, t3 - t1, 0, t3 - t1, true },
        { 9, t0, t1, t2, t3, t2 - t1, t3 - t0, t2 - t1, t3 - t0, true }
      };
      check_ba_events_durations(
        *db.bi_db(),
        badurations,
        sizeof(badurations) / sizeof(*badurations));
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
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
