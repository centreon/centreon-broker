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
#include <cstdlib>
#include <ctime>
#include <iostream>
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
    db.open(NULL, DB_NAME);

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
        "         (2, 123456000, NULL)"};
    for (size_t i(0); i < sizeof(queries) / sizeof(*queries); ++i) {
      QSqlQuery q(*db.bi_db());
      if (!q.exec(queries[i]))
        throw(exceptions::msg()
              << "could not run query: " << q.lastError().text() << " ("
              << queries[i] << ")");
    }

    // Run Broker and expect it to close the inconsistent events.
    test_file cfg;
    cfg.set_template(PROJECT_SOURCE_DIR
                     "/test/cfg/bam_events_autoclose.xml.in");
    cfg.set("DB_NAME", DB_NAME);
    cbd broker;
    broker.set_config_file(cfg.generate());
    time_t now(::time(NULL));
    broker.start();
    sleep_for(5);
    broker.stop();

    // Check database.
    {
      struct {
        unsigned int ba_id;
        long long start_time;
        long long end_time;
        long long variance_max;
      } entries[] = {{1, 123456789, 123456790, 0}, {1, 123456790, 123456791, 0},
                     {1, 123456791, 123456792, 0}, {1, 123456792, 123456800, 0},
                     {1, 123456800, 123456900, 0}, {2, 123456000, 123456789, 0},
                     {2, 123456789, 123456790, 0}, {2, 123456792, 0, 3}};
      entries[7].end_time = static_cast<long long>(now);
      QSqlQuery q(*db.bi_db());
      if (!q.exec("SELECT ba_id, start_time, end_time"
                  "  FROM mod_bam_reporting_ba_events"
                  "  ORDER BY ba_id, start_time"))
        throw(exceptions::msg()
              << "could not get BA events: " << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i >= sizeof(entries) / sizeof(*entries))
          throw(exceptions::msg() << "too much BA events");
        if ((q.value(0).toUInt() != entries[i].ba_id) ||
            (q.value(1).toLongLong() != entries[i].start_time) ||
            ((q.value(2).toLongLong() < entries[i].end_time) ||
             (q.value(2).toLongLong() >
              (entries[i].end_time + entries[i].variance_max))))
          throw(exceptions::msg()
                << "invalid BA event entry: got (BA ID " << q.value(0).toUInt()
                << ", start time " << q.value(1).toLongLong() << ", end time "
                << q.value(2).toLongLong() << "), expected ("
                << entries[i].ba_id << ", " << entries[i].start_time << ", "
                << entries[i].end_time << ")");
        ++i;
      }
    }
    {
      struct {
        unsigned int kpi_id;
        long long start_time;
        long long end_time;
        long long variance_max;
      } entries[] = {{1, 123456789, 123456790, 0}, {1, 123456790, 123456791, 0},
                     {1, 123456791, 123456792, 0}, {1, 123456792, 123456800, 0},
                     {1, 123456800, 123456900, 0}, {2, 123456000, 123456789, 0},
                     {2, 123456789, 123456790, 0}, {2, 123456792, 0, 3}};
      entries[7].end_time = static_cast<long long>(now);
      QSqlQuery q(*db.bi_db());
      if (!q.exec("SELECT kpi_id, start_time, end_time"
                  "  FROM mod_bam_reporting_kpi_events"
                  "  ORDER BY kpi_id, start_time"))
        throw(exceptions::msg()
              << "could not get KPI events: " << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i > sizeof(entries) / sizeof(*entries))
          throw(exceptions::msg() << "too much KPI events");
        if ((q.value(0).toUInt() != entries[i].kpi_id) ||
            (q.value(1).toLongLong() != entries[i].start_time) ||
            ((q.value(2).toLongLong() < entries[i].end_time) ||
             (q.value(2).toLongLong() >
              (entries[i].end_time + entries[i].variance_max))))
          throw(exceptions::msg()
                << "invalid KPI event entry: got (KPI ID "
                << q.value(0).toUInt() << ", start time "
                << q.value(1).toLongLong() << ", end time "
                << q.value(2).toLongLong() << "), expected ("
                << entries[i].kpi_id << ", " << entries[i].start_time << ", "
                << entries[i].end_time << ")");
        ++i;
      }
    }

    // Sucess !
    error = false;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }

  // Return value.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
