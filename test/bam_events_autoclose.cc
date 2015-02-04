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

#include <cstdlib>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_bam_events_autoclose"

/**
 *  Check that inconsistent events are properly closed.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Open database.
    test_db db;
    db.open(NULL, DB_NAME, NULL);

    // Populate database.
    char const* queries[] = {
      // BAs
      "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name)"
      "  VALUES (1, 'BA1'), (2, 'KPI2')",
      "INSERT INTO mod_bam_reporting_ba_events (ba_id, start_time,"
      "            end_time)"
      "  VALUES (1, 123456789, NULL),"
      "         (1, 123456790, 123456791),"
      "         (1, 123456791, 123456792),"
      "         (1, 123456792, NULL),"
      "         (1, 123456800, 123456900),"
      "         (2, 123456789, 123456790),"
      "         (2, 123456792, NULL),"
      "         (2, 123456000, NULL)",
      // KPIs
      "INSERT INTO mod_bam_reporting_kpi (kpi_id, kpi_name)"
      "  VALUES (1, 'KPI1'), (2, 'KPI2')",
      "INSERT INTO mod_bam_reporting_kpi_events (kpi_id, start_time,"
      "            end_time)"
      "  VALUES (1, 123456789, NULL),"
      "         (1, 123456790, 123456791),"
      "         (1, 123456791, 123456792),"
      "         (1, 123456792, NULL),"
      "         (1, 123456800, 123456900),"
      "         (2, 123456789, 123456790),"
      "         (2, 123456792, NULL),"
      "         (2, 123456000, NULL)"
    };
    for (size_t i(0); i < sizeof(queries) / sizeof(*queries); ++i) {
      QSqlQuery q(*db.bi_db());
      if (!q.exec(queries[i]))
        throw (exceptions::msg() << "could not run query: "
               << q.lastError().text() << " (" << queries[i] << ")");
    }

    // Run Broker and expect it to close the inconsistent events.
    test_file cfg;
    cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam_events_autoclose.xml.in");
    cfg.set("DB_NAME", DB_NAME);
    cbd broker;
    broker.set_config_file(cfg.generate());
    broker.start();
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.stop();

    // Check database.
    {
      struct {
        unsigned int ba_id;
        long long start_time;
        long long end_time;
      } entries[] = {
        { 1, 123456789, 123456790 },
        { 1, 123456790, 123456791 },
        { 1, 123456791, 123456792 },
        { 1, 123456792, 123456800 },
        { 1, 123456800, 123456900 },
        { 2, 123456000, 123456789 },
        { 2, 123456789, 123456790 },
        { 2, 123456792, 0 }
      };
      QSqlQuery q(*db.bi_db());
      if (!q.exec(
               "SELECT ba_id, start_time, end_time"
               "  FROM mod_bam_reporting_ba_events"
               "  ORDER BY ba_id, start_time"))
        throw (exceptions::msg() << "could not get BA events: "
               << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i >= sizeof(entries) / sizeof(*entries))
          throw (exceptions::msg() << "too much BA events");
        if ((q.value(0).toUInt() != entries[i].ba_id)
            || (q.value(1).toLongLong() != entries[i].start_time)
            || (q.value(2).toLongLong() != entries[i].end_time))
          throw (exceptions::msg()
                 << "invalid BA event entry: got (BA ID "
                 << q.value(0).toUInt() << ", start time "
                 << q.value(1).toLongLong() << ", end time "
                 << q.value(2).toLongLong() << "), expected ("
                 << entries[i].ba_id << ", " << entries[i].start_time
                 << ", " << entries[i].end_time << ")");
        ++i;
      }
    }
    {
      struct {
        unsigned int kpi_id;
        long long start_time;
        long long end_time;
      } entries[] = {
        { 1, 123456789, 123456790 },
        { 1, 123456790, 123456791 },
        { 1, 123456791, 123456792 },
        { 1, 123456792, 123456800 },
        { 1, 123456800, 123456900 },
        { 2, 123456000, 123456789 },
        { 2, 123456789, 123456790 },
        { 2, 123456792, 0 }
      };
      QSqlQuery q(*db.bi_db());
      if (!q.exec(
               "SELECT kpi_id, start_time, end_time"
               "  FROM mod_bam_reporting_kpi_events"
               "  ORDER BY kpi_id, start_time"))
        throw (exceptions::msg() << "could not get KPI events: "
               << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i > sizeof(entries) / sizeof(*entries))
          throw (exceptions::msg() << "too much KPI events");
        if ((q.value(0).toUInt() != entries[i].kpi_id)
            || (q.value(1).toLongLong() != entries[i].start_time)
            || (q.value(2).toLongLong() != entries[i].end_time))
          throw (exceptions::msg()
                 << "invalid KPI event entry: got (KPI ID "
                 << q.value(0).toUInt() << ", start time "
                 << q.value(1).toLongLong() << ", end time "
                 << q.value(2).toLongLong() << "), expected ("
                 << entries[i].kpi_id << ", " << entries[i].start_time
                 << ", " << entries[i].end_time << ")");
        ++i;
      }
    }

    // Sucess !
    error = false;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }

  // Return value.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
