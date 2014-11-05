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

#define CENTREON_DB_NAME "broker_bam_centreon"
#define BI_DB_NAME "broker_bam_bi"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 10

static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
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

    // Create BAs.
    {
      QString query(
                "INSERT INTO mod_bam (ba_id, name, description,"
                "                     sla_month_percent_warn, sla_month_percent_crit,"
                "                     sla_month_duration_warn, sla_month_duration_crit,"
                "                     must_be_rebuild)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60, 1),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BAs: "
               << q.lastError().text());
    }

    // Create BA events.
    {
      QString query(
                "INSERT INTO mod_bam_reporting_ba_events (ba_id, start_time, end_time)"
                "  VALUES (1, 0, 30),"
                "         (2, 0, 50),"
                "         (1, 30, 120),"
                "         (2, 50, 160)");
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
                "                        tp_friday, tp_saturday"
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
    broker.set_config_file(
      PROJECT_SOURCE_DIR "/test/cfg/bam.xml");
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Let the broker do its things.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // See if the ba events durations were created.
    {
      QString query(
                "SELECT * FROM mod_bam_reporting_ba_events_durations WHERE must_be_rebuild = 1");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not get the number of BA: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg() << "the BAs must_be_rebuild field were "
                                    "not updated.");
    }

    // See if the ba were marked as rebuilt.
    {
      QString query(
                "SELECT * FROM mod_bam WHERE must_be_rebuild = 1");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not get the number of BA: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg() << "the bas must_be_rebuild field were "
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
