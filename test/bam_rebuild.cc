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
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/cbd.hh"
#include "test/vars.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define CENTREON_DB_NAME "broker_bam_centreon_rebuild"
#define BI_DB_NAME "broker_bam_bi_rebuild"
#define COMMAND_FILE "command_file_rebuild"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 10

struct ba_event_duration {
  unsigned int ba_event_id;
  unsigned int timeperiod_id;
  time_t start_time;
  time_t end_time;
  unsigned int duration;
  unsigned int sla_duration;
  bool timeperiod_is_default;
};

static void check_ba_event_durations(
              QSqlDatabase& db,
              ba_event_duration const* baed,
              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT ba_event_id, timeperiod_id, start_time, end_time,"
             "      duration, sla_duration, timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_events_durations"
             "  ORDER BY ba_event_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg()
           << "could not fetch BA event durations at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA event durations at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != baed[i].ba_event_id
        || q.value(1).toUInt() != baed[i].timeperiod_id
        || static_cast<time_t>(q.value(2).toLongLong())
           != baed[i].start_time
        || static_cast<time_t>(q.value(3).toLongLong())
           != baed[i].end_time
        || q.value(4).toUInt() != baed[i].duration
        || q.value(5).toUInt() != baed[i].sla_duration
        || q.value(6).toBool() != baed[i].timeperiod_is_default)
      throw (exceptions::msg() << "invalid BA event durations " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (ba event id "
             << q.value(0).toUInt() << ", timeperiod id "
             << q.value(1).toUInt() << ", start time "
             << q.value(2).toInt() << ", end time "
             << q.value(3).toInt() << ", duration "
             << q.value(4).toInt() << ", sla duration "
             << q.value(5).toBool() << ", timeperiod is default "
             << q.value(6).toInt() << ") expected ("
             << baed[i].ba_event_id << ", " << baed[i].timeperiod_id << ", "
             << baed[i].start_time << ", " << baed[i].end_time << ", "
             << baed[i].duration << ", " << baed[i].sla_duration << ", "
             << baed[i].timeperiod_is_default << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA event durations at iteration "
           << iteration << ": expected " << count);
  return ;
}

struct ba_availability {
  unsigned int ba_id;
  time_t time_id;
  unsigned int timeperiod_id;
  unsigned int available;
  unsigned int unavailable;
  unsigned int degraded;
  unsigned int unknown;
  unsigned int downtime;
  unsigned int alert_unavailable_opened;
  unsigned int alert_degraded_opened;
  unsigned int alert_unknown_opened;
  unsigned int nb_downtime;
  bool timeperiod_is_default;
};

static void check_ba_availability(
              QSqlDatabase& db,
              ba_availability const* baav,
              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT ba_id, time_id, timeperiod_id, available,"
             "      unavailable, degraded, unknown, downtime,"
             "      alert_unavailable_opened, alert_degraded_opened,"
             "      alert_unknown_opened, nb_downtime,"
             "      timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_availabilities"
             "  ORDER BY ba_id, time_id ASC");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg()
           << "could not fetch BA event availabilities at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA availabilities at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != baav[i].ba_id
        || static_cast<time_t>(q.value(1).toLongLong())
           != baav[i].time_id
        || q.value(2).toUInt() != baav[i].timeperiod_id
        || q.value(3).toUInt() != baav[i].available
        || q.value(4).toUInt() != baav[i].unavailable
        || q.value(5).toUInt() != baav[i].degraded
        || q.value(6).toUInt() != baav[i].unknown
        || q.value(7).toUInt() != baav[i].downtime
        || q.value(8).toUInt() != baav[i].alert_unavailable_opened
        || q.value(9).toUInt() != baav[i].alert_degraded_opened
        || q.value(10).toUInt() != baav[i].alert_unknown_opened
        || q.value(11).toUInt() != baav[i].nb_downtime
        || q.value(12).toBool() != baav[i].timeperiod_is_default)
      throw (exceptions::msg() << "invalid BA availability "
             << " at iteration " << iteration << ": got (ba id "
             << q.value(0).toUInt() << ", time id "
             << q.value(1).toLongLong() << ", timeperiod id "
             << q.value(2).toUInt() << ", available "
             << q.value(3).toUInt() << ", unavailable "
             << q.value(4).toUInt() << ", degraded "
             << q.value(5).toUInt() << ", unknown "
             << q.value(6).toUInt() << ", downtime "
             << q.value(7).toUInt() << ", alert_unavailable_opened "
             << q.value(8).toUInt() << ", alert_degraded_opened "
             << q.value(9).toUInt() << ", alert_unknown_opened "
             << q.value(10).toUInt() << ", nb_downtime "
             << q.value(11).toUInt() << ", timeperiod_is_default"
             << q.value(12).toBool() << ") expected ("
             << baav[i].ba_id << ", " << baav[i].time_id << ", "
             << baav[i].timeperiod_id << ", " << baav[i].available
             << ", " << baav[i].unavailable << ", "
             << baav[i].degraded << ", " << baav[i].unknown << ", "
             << baav[i].downtime << ", " << baav[i].alert_unavailable_opened << ", "
             << baav[i].alert_degraded_opened << ", " << baav[i].alert_unknown_opened << ", "
             << baav[i].alert_unknown_opened << ", " << baav[i].nb_downtime << ", "
             << baav[i].timeperiod_is_default << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA availabilities at iteration "
           << iteration << ": expected " << count);
  return ;
}

/**
 *  Check that the BAM broker correctly rebuild the data.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  cbd broker;
  test_db db;

  try {
    // Prepare database.
    db.open(NULL, BI_DB_NAME, CENTREON_DB_NAME);

    // Create the config bam xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    file.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    file.set("DB_NAME_BI", BI_DB_NAME);
    file.set("COMMAND_FILE", COMMAND_FILE);
    std::string config_file = file.generate();

    // Create timeperiods
    {
      QString query(
                "INSERT INTO timeperiod (tp_id, tp_name, tp_sunday, tp_monday,"
                "                        tp_tuesday, tp_wednesday, tp_thursday,"
                "                        tp_friday, tp_saturday)"
                "  VALUES (1, '24x7', '00:00-24:00', '00:00-24:00', '00:00-24:00',"
                "          '00:00-24:00', '00:00-24:00', '00:00-24:00', '00:00-24:00')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create timeperiods: "
               << q.lastError().text());
    }

    // Create BAs.
    {
      QString query(
                "INSERT INTO mod_bam (ba_id, name, description,"
                "                     sla_month_percent_warn, sla_month_percent_crit,"
                "                     sla_month_duration_warn, sla_month_duration_crit,"
                "                     must_be_rebuild, id_reporting_period, activate)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60, '1', 1, '1'),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50, '1', NULL, '1'),"
                "         (3, 'BA3', 'DESC3', 70, 60, 50, 40, '1', 1, '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BAs: "
               << q.lastError().text());
    }
    {
      QString query(
                "INSERT INTO host (host_id, host_name)"
                "  VALUES (1001, 'Virtual BA host')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create virtual BA host: "
               << q.lastError().text());
    }
    {
      QString query(
                "INSERT INTO service (service_id, service_description)"
                "  VALUES (1001, 'ba_1'),"
                "         (1002, 'ba_2'),"
                "         (1003, 'ba_3')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not create virtual BA services: "
               << q.lastError().text());
    }
    {
      QString query(
                "INSERT INTO host_service_relation"
                "            (host_host_id, service_service_id)"
                "  VALUES (1001, 1001), (1001, 1002), (1001, 1003)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not create virtual BA host/service links: "
               << q.lastError().text());
    }

    // Create BAs dimension.
    {
      QString query(
                "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
                "                     sla_month_percent_crit, sla_month_percent_warn,"
                "                     sla_month_duration_crit, sla_month_duration_warn)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50),"
                "         (3, 'BA3', 'DESC3', 70, 60, 50, 40)");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BA dimensions: "
               << q.lastError().text());
    }

    time_t now = ::time(NULL);
    time_t midnight_time;
    // Create BA events.
    {
      struct tm midnight;
      ::localtime_r(&now, &midnight);
      midnight.tm_sec = midnight.tm_min = midnight.tm_hour = 0;
      midnight_time = ::mktime(&midnight) - (3600 * 24);
      std::stringstream ss;
      ss << "INSERT INTO mod_bam_reporting_ba_events (ba_event_id, ba_id, start_time, end_time, status, in_downtime)"
            "  VALUES (1, 1, 0, 30, 0, false),"
            "         (2, 2, 0, 50, 0, false),"
            "         (3, 1, 30, 120, 1, false),"
            "         (4, 2, 50, 160, 2, true),"
            "         (5, 1, " << midnight_time << ", NULL, 1, false),"
            "         (6, 2, " << midnight_time << ", NULL, 2, true),"
            "         (7, 3, 1418209039, 1418314059, 0, false),"
            "         (8, 3, 1418314059, 1418327489, 0, false),"
            "         (9, 3, 1418327489, 1418329589, 2, false),"
            "         (10, 3, 1418329589, 1418398892, 0, false)";
      QString query(ss.str().c_str());
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BA events: "
               << q.lastError().text());
    }

    // Create BA-Timeperiods relations
    {
      QString query(
                "INSERT INTO mod_bam_relations_ba_timeperiods (ba_id, tp_id)"
                "  VALUES (2, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not create BAs-timeperiods relations: "
               << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(config_file);
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Let the broker do its things.
    sleep_for(6 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // See if the ba events durations were created.
    {
      ba_event_duration baed[] = {
        { 1, 1, 0, 30, 30, 30, true },
        { 2, 1, 0, 50, 50, 50, false },
        { 3, 1, 30, 120, 90, 90, true },
        { 4, 1, 50, 160, 110, 110, false },
        { 7, 1, 1418209039, 1418314059, 105020, 105020, true },
        { 8, 1, 1418314059, 1418327489, 13430, 13430, true },
        { 9, 1, 1418327489, 1418329589, 2100, 2100, true },
        { 10, 1, 1418329589, 1418398892, 69303, 69303, true }
      };
      check_ba_event_durations(
        *db.bi_db(),
        baed,
        sizeof(baed) / sizeof(*baed));
    }

    // See if the ba availabilities were created.
    {
      ba_availability baav[] = {
        { 1, -3600, 1, 30, 0, 90, 0, 0, 0, 1, 0, 0, true },
        { 1, midnight_time, 1, 0, 0, 3600 * 24, 0, 0, 0, 1, 0, 0, true },
        { 2, -3600, 1, 50, 110, 0, 0, 110, 1, 0, 0, 1, false },
        { 2, midnight_time, 1, 0, 3600 * 24, 0, 0, 3600 * 24, 1, 0, 0, 1, false },
        { 3, 1418166000, 1, 43361, 0, 0, 0, 0, 0, 0, 0, 0, true },
        { 3, 1418252400, 1, 84300, 2100, 0, 0, 0, 1, 0, 0, 0, true },
        { 3, 1418338800, 1, 60092, 0, 0, 0, 0, 0, 0, 0, 0, true }
      };
      check_ba_availability(
        *db.bi_db(),
        baav,
        sizeof(baav) / sizeof(*baav));
    }

    // See if the ba were marked as rebuilt.
    {
      QString query(
                "SELECT * FROM mod_bam WHERE must_be_rebuild = '1'");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not get the number of BA: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg() << "the bas must_be_rebuild field was "
                                    "not updated.");
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    db.set_remove_db_on_close(false);
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
