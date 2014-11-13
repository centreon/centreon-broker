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

static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
}

struct ba_event_duration {
  unsigned int ba_event_id;
  unsigned int timeperiod_id;
  unsigned int start_time;
  unsigned int end_time;
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
    if (q.value(0).toInt() != baed[i].ba_event_id
        || q.value(1).toInt() != baed[i].timeperiod_id
        || q.value(2).toInt() != baed[i].start_time
        || q.value(3).toInt() != baed[i].end_time
        || q.value(4).toInt() != baed[i].duration
        || q.value(5).toInt() != baed[i].sla_duration
        || q.value(6).toInt() != baed[i].timeperiod_is_default)
      throw (exceptions::msg() << "invalid BA event durations " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (ba event id "
             << q.value(0).toInt() << ", timeperiod id "
             << q.value(1).toInt() << ", start time "
             << q.value(2).toInt() << ", end time "
             << q.value(3).toInt() << ", duration "
             << q.value(4).toInt() << ", sla duration "
             << q.value(5).toInt() << ", timeperiod is default "
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
  unsigned int time_id;
  unsigned int timeperiod_id;
  unsigned int available;
  unsigned int unavailable;
  unsigned int degraded;
  unsigned int unknown;
  unsigned int downtime;
  unsigned int alert_unavailable_opened;
  unsigned int alert_degraded_opened;
  unsigned int alert_unknown_opened;
  unsigned int alert_downtime_opened;
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
             "      alert_unknown_opened, alert_downtime_opened,"
             "      timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_availabilities"
             "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg()
           << "could not fetch BA event availabilities at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA availabilities at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toInt() != baav[i].ba_id
        || q.value(1).toInt() != baav[i].time_id
        || q.value(2).toInt() != baav[i].timeperiod_id
        || q.value(3).toInt() != baav[i].available
        || q.value(4).toInt() != baav[i].unavailable
        || q.value(5).toInt() != baav[i].degraded
        || q.value(6).toInt() != baav[i].unknown
        || q.value(7).toInt() != baav[i].downtime
        || q.value(8).toInt() != baav[i].alert_unavailable_opened
        || q.value(9).toInt() != baav[i].alert_degraded_opened
        || q.value(10).toInt() != baav[i].alert_unknown_opened
        || q.value(11).toInt() != baav[i].alert_downtime_opened
        || q.value(12).toInt() != baav[i].timeperiod_is_default)
      throw (exceptions::msg() << "invalid BA availability "
             << " at iteration " << iteration << ": got (ba id "
             << q.value(0).toInt() << ", time id "
             << q.value(1).toInt() << ", timeperiod id "
             << q.value(2).toInt() << ", available "
             << q.value(3).toInt() << ", unavailable "
             << q.value(4).toInt() << ", degraded "
             << q.value(5).toInt() << ", unknown "
             << q.value(6).toInt() << ", downtime "
             << q.value(7).toInt() << ", alert_unavailable_opened "
             << q.value(8).toInt() << ", alert_degraded_opened "
             << q.value(9).toInt() << ", alert_unknown_opened "
             << q.value(10).toInt() << ", alert_downtime_opened "
             << q.value(11).toInt() << ", timeperiod_is_default"
             << q.value(12).toBool() << ") expected ("
             << baav[i].ba_id << ", " << baav[i].time_id << ", "
             << baav[i].timeperiod_id << ", " << baav[i].unavailable << ", "
             << baav[i].degraded << ", " << baav[i].unknown << ", "
             << baav[i].downtime << ", " << baav[i].alert_unavailable_opened << ", "
             << baav[i].alert_degraded_opened << ", " << baav[i].alert_unknown_opened << ", "
             << baav[i].alert_unknown_opened << "," << baav[i].alert_downtime_opened << ", "
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

    // Create BAs.
    {
      QString query(
                "INSERT INTO mod_bam (ba_id, name, description,"
                "                     sla_month_percent_warn, sla_month_percent_crit,"
                "                     sla_month_duration_warn, sla_month_duration_crit,"
                "                     must_be_rebuild)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60, '1'),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50, '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BAs: "
               << q.lastError().text());
    }

    // Create BAs dimension.
    {
      QString query(
                "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
                "                     sla_month_percent_1, sla_month_percent_2,"
                "                     sla_month_duration_1, sla_month_duration_2)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50)");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BA dimensions: "
               << q.lastError().text());
    }

    // Create BA events.
    {
      QString query(
                "INSERT INTO mod_bam_reporting_ba_events (ba_event_id, ba_id, start_time, end_time, status, in_downtime)"
                "  VALUES (1, 1, 0, 30, 0, false),"
                "         (2, 2, 0, 50, 0, false),"
                "         (3, 1, 30, 120, 1, false),"
                "         (4, 2, 50, 160, 2, true)");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BA events: "
               << q.lastError().text());
    }

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

    // Create BA-Timeperiods relations
    {
      QString query(
                "INSERT INTO mod_bam_ba_tp_rel (ba_id, timeperiod_id, is_default)"
                "  VALUES (1, 1, true),"
                "         (2, 1, false)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not BAs timeperiods relations: "
               << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(config_file);
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Let the broker do its things.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // See if the ba events durations were created.
    {
      ba_event_duration baed[] =
      {{1, 1, 0, 30, 30, 30, true},
       {2, 1, 0, 50, 50, 50, false},
       {3, 1, 30, 120, 90, 90, true},
       {4, 1, 50, 160, 110, 110, false}};

      check_ba_event_durations(*db.bi_db(),
                               baed,
                               sizeof(baed) / sizeof(*baed));
    }

    // See if the ba availabilities were created.
    {
      ba_availability baav[] =
      {{1, 0, 1, 30, 90, 0, 0, 0, 1, 0, 0, 0, true},
       {2, 0, 1, 50, 0, 110, 0, 110, 0, 1, 0, 1, false}};
      check_ba_availability(*db.bi_db(),
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
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
