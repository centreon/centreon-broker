/*
** Copyright 2014-2015 Centreon
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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
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

static void check_ba_event_durations(QSqlDatabase& db,
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
    throw(exceptions::msg()
          << "could not fetch BA event durations at iteration " << iteration
          << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg()
            << "not enough BA event durations at iteration " << iteration
            << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != baed[i].ba_event_id ||
        q.value(1).toUInt() != baed[i].timeperiod_id ||
        static_cast<time_t>(q.value(2).toLongLong()) != baed[i].start_time ||
        static_cast<time_t>(q.value(3).toLongLong()) != baed[i].end_time ||
        q.value(4).toUInt() != baed[i].duration ||
        q.value(5).toUInt() != baed[i].sla_duration ||
        q.value(6).toBool() != baed[i].timeperiod_is_default)
      throw(exceptions::msg()
            << "invalid BA event durations " << q.value(0).toUInt()
            << " at iteration " << iteration << ": got (ba event id "
            << q.value(0).toUInt() << ", timeperiod id " << q.value(1).toUInt()
            << ", start time " << q.value(2).toInt() << ", end time "
            << q.value(3).toInt() << ", duration " << q.value(4).toInt()
            << ", sla duration " << q.value(5).toInt()
            << ", timeperiod is default " << q.value(6).toBool()
            << ") expected (" << baed[i].ba_event_id << ", "
            << baed[i].timeperiod_id << ", " << baed[i].start_time << ", "
            << baed[i].end_time << ", " << baed[i].duration << ", "
            << baed[i].sla_duration << ", " << baed[i].timeperiod_is_default
            << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BA event durations at iteration "
                            << iteration << ": expected " << count);
  return;
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

static void check_ba_availability(QSqlDatabase& db,
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
      "  ORDER BY ba_id, time_id ASC, timeperiod_id ASC");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg()
          << "could not fetch BA event availabilities at iteration "
          << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg()
            << "not enough BA availabilities at iteration " << iteration
            << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != baav[i].ba_id ||
        static_cast<time_t>(q.value(1).toLongLong()) != baav[i].time_id ||
        q.value(2).toUInt() != baav[i].timeperiod_id ||
        q.value(3).toUInt() != baav[i].available ||
        q.value(4).toUInt() != baav[i].unavailable ||
        q.value(5).toUInt() != baav[i].degraded ||
        q.value(6).toUInt() != baav[i].unknown ||
        q.value(7).toUInt() != baav[i].downtime ||
        q.value(8).toUInt() != baav[i].alert_unavailable_opened ||
        q.value(9).toUInt() != baav[i].alert_degraded_opened ||
        q.value(10).toUInt() != baav[i].alert_unknown_opened ||
        q.value(11).toUInt() != baav[i].nb_downtime ||
        q.value(12).toBool() != baav[i].timeperiod_is_default)
      throw(exceptions::msg()
            << "invalid BA availability "
            << " at iteration " << iteration << " (entry " << i
            << "): got (ba id " << q.value(0).toUInt() << ", time id "
            << q.value(1).toLongLong() << ", timeperiod id "
            << q.value(2).toUInt() << ", available " << q.value(3).toUInt()
            << ", unavailable " << q.value(4).toUInt() << ", degraded "
            << q.value(5).toUInt() << ", unknown " << q.value(6).toUInt()
            << ", downtime " << q.value(7).toUInt()
            << ", alert_unavailable_opened " << q.value(8).toUInt()
            << ", alert_degraded_opened " << q.value(9).toUInt()
            << ", alert_unknown_opened " << q.value(10).toUInt()
            << ", nb_downtime " << q.value(11).toUInt()
            << ", timeperiod_is_default " << q.value(12).toBool()
            << ") expected (" << baav[i].ba_id << ", " << baav[i].time_id
            << ", " << baav[i].timeperiod_id << ", " << baav[i].available
            << ", " << baav[i].unavailable << ", " << baav[i].degraded << ", "
            << baav[i].unknown << ", " << baav[i].downtime << ", "
            << baav[i].alert_unavailable_opened << ", "
            << baav[i].alert_degraded_opened << ", "
            << baav[i].alert_unknown_opened << ", "
            << baav[i].alert_unknown_opened << ", " << baav[i].nb_downtime
            << ", " << baav[i].timeperiod_is_default << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BA availabilities at iteration "
                            << iteration << ": expected " << count);
  return;
}

/**
 *  Check that the BAM broker correctly rebuild the data.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Set timezone.
  setenv("TZ", ":Europe/Paris", 1);

  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  cbd broker;
  test_db db;

  try {
    // Prepare database.
    db.open(CENTREON_DB_NAME, BI_DB_NAME, true);

    // Create the config bam xml file.
    test_file file;
    file.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    file.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    file.set("DB_NAME_BI", BI_DB_NAME);
    file.set("COMMAND_FILE", COMMAND_FILE);
    std::string config_file = file.generate();

    // Create organization.
    {
      QString query;
      query =
          "INSERT INTO cfg_organizations (organization_id, name, shortname)"
          "  VALUES (42, '42', '42')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create organization: " << q.lastError().text());
    }

    // Create timeperiods
    {
      QString query(
          "INSERT INTO cfg_timeperiods (tp_id, tp_name, tp_sunday, tp_monday,"
          "                        tp_tuesday, tp_wednesday, tp_thursday,"
          "                        tp_friday, tp_saturday, organization_id)"
          "  VALUES (1, '24x7', '00:00-24:00', '00:00-24:00', '00:00-24:00',"
          "          '00:00-24:00', '00:00-24:00', '00:00-24:00', "
          "'00:00-24:00', 42),"
          "         (2, 'workhours', '', '09:00-17:00', '09:00-17:00',"
          "          '09:00-17:00', '09:00-17:00', '09:00-17:00', '', 42),"
          "         (3, 'non-worhours', '', "
          "          '00:00-09:00,17:00-24:00', '00:00-09:00,17:00-24:00',"
          "          '00:00-09:00,17:00-24:00', '00:00-09:00,17:00-24:00',"
          "           '00:00-09:00,17:00-24:00', '', 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create timeperiods: " << q.lastError().text());
    }

    // Create timeperiod excludes
    /*{
      QString query("INSERT INTO timeperiod_exclude_relations"
                    "            (exclude_id, timeperiod_id,"
                    "             timeperiod_exclude_id)"
                    "  VALUES (1, 3, 2)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create timeperiod excludes: "
               << q.lastError().text());
    }*/

    // Create BAs.
    {
      QString query(
          "INSERT INTO cfg_bam_ba_types (ba_type_id, name, slug,"
          "            description)"
          "  VALUES (1, 'Default', 'default', 'Default type')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BA types: " << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_bam (ba_id, name, description,"
          "                     sla_month_percent_warn, sla_month_percent_crit,"
          "                     sla_month_duration_warn, "
          "sla_month_duration_crit,"
          "                     must_be_rebuild, id_reporting_period, activate,"
          "                     ba_type_id, organization_id)"
          "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60, '1', 1, '1', 1, 42),"
          "         (2, 'BA2', 'DESC2', 80, 70, 60, 50, '1', NULL, '1', 1, 42),"
          "         (3, 'BA3', 'DESC3', 70, 60, 50, 40, '1', 1, '1', 1, 42),"
          "         (4, 'BA4', 'DESC4', 60, 50, 40, 30, '1', 1, '1', 1, 42),"
          "         (5, 'BA5', 'DESC5', 50, 40, 30, 20, '1', 2, '1', 1, 42),"
          "         (6, 'BA6', 'DESC6', 40, 30, 20, 10, '1', 1, '1', 1, 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BAs: " << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_bam_poller_relations (ba_id, poller_id)"
          "  VALUES (1, 42), (2, 42), (3, 42), (4, 42),"
          "         (5, 42), (6, 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create BA/poller relations: "
                                << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
          "  VALUES (1001, 'Virtual BA host', 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create virtual BA host: " << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_services (service_id, service_description, "
          "organization_id)"
          "  VALUES (1001, 'ba_1', 42),"
          "         (1002, 'ba_2', 42),"
          "         (1003, 'ba_3', 42),"
          "         (1004, 'ba_4', 42),"
          "         (1005, 'ba_5', 42),"
          "         (1006, 'ba_6', 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create virtual BA services: "
                                << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_hosts_services_relations"
          "            (host_host_id, service_service_id)"
          "  VALUES (1001, 1001), (1001, 1002), (1001, 1003),"
          "         (1001, 1004), (1001, 1005), (1001, 1006)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create virtual BA host/service links: "
              << q.lastError().text());
    }

    // Create BAs dimension.
    {
      QString query(
          "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
          "                     sla_month_percent_crit, sla_month_percent_warn,"
          "                     sla_month_duration_crit, "
          "sla_month_duration_warn)"
          "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60),"
          "         (2, 'BA2', 'DESC2', 80, 70, 60, 50),"
          "         (3, 'BA3', 'DESC3', 70, 60, 50, 40),"
          "         (4, 'BA4', 'DESC4', 60, 50, 40, 30),"
          "         (5, 'BA5', 'DESC5', 50, 40, 30, 20),"
          "         (6, 'BA6', 'DESC6', 40, 30, 20, 10)");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BA dimensions: " << q.lastError().text());
    }

    time_t now = ::time(NULL);
    time_t midnight_time;
    struct tm midnight;
    ::localtime_r(&now, &midnight);
    midnight.tm_sec = midnight.tm_min = midnight.tm_hour = 0;
    midnight_time = ::mktime(&midnight) - (3600 * 24);
    // Create BA events.
    {
      std::stringstream ss;
      ss << "INSERT INTO mod_bam_reporting_ba_events (ba_event_id, ba_id, "
            "start_time, end_time, status, in_downtime)"
            "  VALUES (1, 1, 0, 30, 0, false),"
            "         (2, 2, 0, 50, 0, false),"
            "         (3, 1, 30, 120, 1, false),"
            "         (4, 2, 50, 160, 2, true),"
            "         (5, 1, "
         << midnight_time << ", " << now
         << ", 1, false),"
            "         (6, 2, "
         << midnight_time << ", " << now
         << ", 2, true),"
            "         (7, 3, 1418209039, 1418314059, 0, false),"
            "         (8, 3, 1418314059, 1418327489, 0, false),"
            "         (9, 3, 1418327489, 1418329589, 2, false),"
            "         (10, 3, 1418329589, 1418398892, 0, false),"
            "         (11, 4, 1396303200, 1396389600, 0, false),"
            "         (12, 5, 1418598000, 1418684400, 0, false),"
            "         (13, 6, 1418327489, 1418586689, 2, false)";
      QString query(ss.str().c_str());
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BA events: " << q.lastError().text());
    }

    // Create BA-Timeperiods relations
    {
      QString query(
          "INSERT INTO cfg_bam_relations_ba_timeperiods (ba_id, tp_id)"
          "  VALUES (2, 1), (5, 3)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BAs-timeperiods relations: "
              << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(config_file);
    broker.start();
    sleep_for(2);

    // Let the broker do its things.
    sleep_for(6);

    // See if the ba events durations were created.
    {
      ba_event_duration baed[] = {
          {1, 1, 0, 30, 30, 30, true},
          {2, 1, 0, 50, 50, 50, false},
          {3, 1, 30, 120, 90, 90, true},
          {4, 1, 50, 160, 110, 110, false},
          {5, 1, 0, 0, 86400, 86400, true},
          {6, 1, 0, 0, 86400, 86400, false},
          {7, 1, 1418209039, 1418314059, 105020, 105020, true},
          {8, 1, 1418314059, 1418327489, 13430, 13430, true},
          {9, 1, 1418327489, 1418329589, 2100, 2100, true},
          {10, 1, 1418329589, 1418398892, 69303, 69303, true},
          {11, 1, 1396303200, 1396389600, 86400, 86400, true},
          {12, 2, 1418630400, 1418684400, 54000, 28800, true},
          {12, 3, 1418598000, 1418684400, 86400, 57600, false},
          {13, 1, 1418327489, 1418586689, 259200, 259200, true}};
      baed[4].start_time = midnight_time;
      baed[4].end_time = now;
      baed[4].duration = now - midnight_time;
      baed[4].sla_duration = now - midnight_time;
      baed[5].start_time = midnight_time;
      baed[5].end_time = now;
      baed[5].duration = now - midnight_time;
      baed[5].sla_duration = now - midnight_time;
      check_ba_event_durations(*db.bi_db(), baed, sizeof(baed) / sizeof(*baed));
    }

    // See if the ba availabilities were created.
    {
      ba_availability baav[] = {
          {1, -3600, 1, 30, 0, 90, 0, 0, 0, 1, 0, 0, true},
          {1, midnight_time, 1, 0, 0, 3600 * 24, 0, 0, 0, 1, 0, 0, true},
          {2, -3600, 1, 50, 0, 0, 0, 110, 0, 0, 0, 1, false},
          {2, midnight_time, 1, 0, 0, 0, 0, 3600 * 24, 0, 0, 0, 1, false},
          {3, 1418166000, 1, 43361, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {3, 1418252400, 1, 84300, 2100, 0, 0, 0, 1, 0, 0, 0, true},
          {3, 1418338800, 1, 60092, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {4, 1396303200, 1, 86400, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {4, 1396389600, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {5, 1418598000, 2, 28800, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {5, 1418598000, 3, 57600, 0, 0, 0, 0, 0, 0, 0, 0, false},
          {5, 1418684400, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, true},
          {5, 1418684400, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, false},
          {6, 1418252400, 1, 0, 11311, 0, 0, 0, 1, 0, 0, 0, true},
          {6, 1418338800, 1, 0, 86400, 0, 0, 0, 0, 0, 0, 0, true},
          {6, 1418425200, 1, 0, 86400, 0, 0, 0, 0, 0, 0, 0, true},
          {6, 1418511600, 1, 0, 75089, 0, 0, 0, 0, 0, 0, 0, true}};
      check_ba_availability(*db.bi_db(), baav, sizeof(baav) / sizeof(*baav));
    }

    // See if the ba were marked as rebuilt.
    {
      QString query("SELECT * FROM cfg_bam WHERE must_be_rebuild = '1'");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not get the number of BA: " << q.lastError().text());
      if (q.size() != 0)
        throw(exceptions::msg() << "the bas must_be_rebuild field was "
                                   "not updated.");
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    db.set_remove_db_on_close(false);
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
