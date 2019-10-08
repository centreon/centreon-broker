/*
** Copyright 2015 Centreon
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
#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/db.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/predicate.hh"
#include "test/table_content.hh"
#include "test/time_points.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "bam_v2_main"
#define DB_NAME "broker_" TEST_NAME

// Check count.
static int check_number(0);

/**
 *  Compare two BA or KPI events rows.
 *
 *  @param[in] r1  First row.
 *  @param[in] r2  Second row.
 *
 *  @return True if r1 is strictly less than r2.
 */
static bool events_compare(std::vector<test::predicate> const& r1,
                           std::vector<test::predicate> const& r2) {
  int id_1(r1[0].get_value().uival);
  int id_2(r2[0].get_value().uival);
  time_t start_time_1(r1[1].get_value().tval);
  time_t start_time_2(r2[1].get_value().tval);
  if (id_1 != id_2)
    return (id_1 < id_2);
  return (start_time_1 < start_time_2);
}

/**
 *  Precheck routine.
 */
static void precheck(test::time_points& tpoints) {
  ++check_number;
  std::cout << "check #" << check_number << "\n";
  tpoints.store();
  return;
}

/**
 *  Postcheck routine.
 */
static void postcheck(test::db& db,
                      test::table_content const& expected_bas,
                      test::table_content const& expected_kpis,
                      test::table_content const& expected_ba_events,
                      test::table_content const& expected_kpi_events) {
  std::cout << "  BAs\n";
  static std::string ba_check(
      "SELECT ba_id, current_level, downtime, acknowledged"
      "  FROM mod_bam"
      "  WHERE activate='1'"
      "  ORDER BY ba_id ASC");
  db.check_content(ba_check, expected_bas);
  std::cout << "    passed\n";
  std::cout << "  KPIs\n";
  static std::string kpi_check(
      "SELECT kpi_id, current_status, state_type, last_level, downtime,"
      "       acknowledged, valid"
      "  FROM mod_bam_kpi"
      "  WHERE activate='1'"
      "  ORDER BY kpi_id ASC");
  db.check_content(kpi_check, expected_kpis);
  std::cout << "    passed\n";
  std::cout << "  BA events\n";
  static std::string ba_event_check(
      "SELECT ba_id, start_time, end_time, first_level, status,"
      "       in_downtime"
      "  FROM mod_bam_reporting_ba_events"
      "  ORDER BY ba_id ASC, start_time ASC");
  db.check_content(ba_event_check, expected_ba_events);
  std::cout << "    passed\n";
  std::cout << "  KPI events\n";
  static std::string kpi_event_check(
      "SELECT kpi_id, start_time, end_time, status, in_downtime,"
      " impact_level FROM mod_bam_reporting_kpi_events"
      " ORDER BY kpi_id ASC, start_time ASC");
  db.check_content(kpi_event_check, expected_kpi_events);
  std::cout << "    passed\n";
}

/**
 *  Check that the BAM module work mostly properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", BAM_MONITORING_TABLES_V2,
                            BAM_REPORTING_TABLES_V2, NULL};
    test::db db(DB_NAME, tables);

    // Populate database.
    {
      char const* const queries[] = {
          "INSERT INTO timeperiod (tp_id, tp_name, tp_alias,"
          "            tp_sunday, tp_monday, tp_tuesday,"
          "            tp_wednesday, tp_thursday, tp_friday, "
          "            tp_saturday)"
          "  VALUES (1, '24x7', '24x7', '00:00-24:00',"
          "          '00:00-24:00', '00:00-24:00', '00:00-24:00',"
          "          '00:00-24:00', '00:00-24:00', '00:00-24:00')",
          "INSERT INTO host (host_id, host_name)"
          "  VALUES (1, '1')",
          "INSERT INTO service (service_id, service_description)"
          "  VALUES (1, '1'), (2, '2'), (3, '3'), (4, '4'), (5, '5'),"
          "         (6, '6'), (7, '7'), (8, '8'), (9, '9'), (10, '10'),"
          "         (11, '11'), (12, '12'), (13, '13'), (14, '14')",
          "INSERT INTO host_service_relation (host_host_id,"
          "            service_service_id)"
          "  VALUES (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (1, 6),"
          "         (1, 7), (1, 8), (1, 9), (1, 10), (1, 11), (1, 12),"
          "         (1, 13), (1, 14)",
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
          "         (9, 'BA9', 10, 0, '1', 1),"
          "         (10, 'BA10', 80, 60, '1', 1),"
          "         (11, 'BA11', 70, 50, '1', 1)",
          "INSERT INTO mod_bam_poller_relations (ba_id, poller_id)"
          "  VALUES (1, 42), (2, 42), (3, 42), (4, 42), (5, 42),"
          "         (6, 42), (7, 42), (8, 42), (9, 42), (10, 42),"
          "         (11, 42)",
          "INSERT INTO host (host_id, host_name)"
          "  VALUES (1001, '_Module_BAM')",
          "INSERT INTO service (service_id, service_description)"
          "  VALUES (1001, 'ba_1'), (1002, 'ba_2'), (1003, 'ba_3'),"
          "         (1004, 'ba_4'), (1005, 'ba_5'), (1006, 'ba_6'),"
          "         (1007, 'ba_7'), (1008, 'ba_8'), (1009, 'ba_9'),"
          "         (1010, 'ba_10'), (1011, 'ba_11')",
          "INSERT INTO host_service_relation (host_host_id,"
          "            service_service_id)"
          "  VALUES (1001, 1001), (1001, 1002), (1001, 1003),"
          "         (1001, 1004), (1001, 1005), (1001, 1006),"
          "         (1001, 1007), (1001, 1008), (1001, 1009),"
          "         (1001, 1010), (1001, 1011)",
          "INSERT INTO mod_bam_boolean (boolean_id, name,"
          "            expression, bool_state, activate)"
          "  VALUES (1, 'BoolExp1', '{1 1} {is} {OK}', 0, 1),"
          "         (2, 'BoolExp2', '{1 2} {not} {CRITICAL} {AND} {1 3} {not} "
          "{OK}', 0, 1),"
          "         (3, 'BoolExp3', '({1 5} {not} {WARNING} {AND} {1 6} {is} "
          "{WARNING}) {OR} {1 7} {is} {CRITICAL}', 1, 1),"
          "         (4, 'BoolExp4', '{1 14} {is} {CRITICAL}', 1, 1),"
          "         (5, 'BoolExp5', '{Non Existent} {is} {OK}', 1, 1)",
          "INSERT INTO mod_bam_kpi (kpi_id, kpi_type, host_id,"
          "            service_id, id_indicator_ba, id_ba,"
          "            meta_id, boolean_id, config_type,"
          "            drop_warning, drop_warning_impact_id,"
          "            drop_critical, drop_critical_impact_id,"
          "            drop_unknown, drop_unknown_impact_id,"
          "            ignore_downtime, ignore_acknowledged,"
          "            state_type, activate)"
          "  VALUES (1, '0', 1, 1, NULL, 2, NULL, NULL, '0', 15, NULL, 25, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (2, '0', 1, 2, NULL, 3, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (3, '0', 1, 3, NULL, 3, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (4, '0', 1, 4, NULL, 4, NULL, NULL, '0', 15, NULL, 25, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (5, '0', 1, 5, NULL, 4, NULL, NULL, '0', 26, NULL, 35, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (6, '0', 1, 6, NULL, 4, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (7, '0', 1, 7, NULL, 5, NULL, NULL, '0', 15, NULL, 25, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (8, '0', 1, 8, NULL, 5, NULL, NULL, '0', 26, NULL, 35, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (9, '0', 1, 9, NULL, 5, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (10, '0', 1, 10, NULL, 5, NULL, NULL, '0', 45, NULL, 55, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (11, '1', NULL, NULL, 2, 6, NULL, NULL, '0', 65, NULL, 75, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (12, '1', NULL, NULL, 3, 7, NULL, NULL, '0', 25, NULL, 35, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (13, '1', NULL, NULL, 4, 7, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (14, '1', NULL, NULL, 5, 7, NULL, NULL, '0', 45, NULL, 55, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (15, '1', NULL, NULL, 6, 8, NULL, NULL, '0', 85, NULL, 95, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (16, '1', NULL, NULL, 7, 8, NULL, NULL, '0', 95, NULL, "
          "105, NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (17, '3', NULL, NULL, NULL, 9, NULL, 1, '0', 10, NULL, 75, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (18, '3', NULL, NULL, NULL, 9, NULL, 2, '0', 10, NULL, 25, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (19, '3', NULL, NULL, NULL, 9, NULL, 3, '0', 10, NULL, 6, "
          "NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (20, '0', 1, 11, NULL, 10, NULL, NULL, '0', NULL, NULL, "
          "NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (21, '0', 1, 12, NULL, 10, NULL, NULL, '0', NULL, NULL, "
          "NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (22, '0', 1, 13, NULL, 10, NULL, NULL, '0', NULL, NULL, "
          "NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (23, '3', NULL, NULL, NULL, 10, NULL, 4, '0', NULL, NULL, "
          "NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
          "         (24, '3', NULL, NULL, NULL, 11, NULL, 5, '0', NULL, NULL, "
          "NULL, NULL, 99, NULL, '0', '0', '1', '1')",
          NULL};
      for (int i(0); queries[i]; ++i) {
        QSqlQuery q(*db.get_db());
        if (!q.exec(queries[i]))
          throw(exceptions::msg()
                << "could not populate database: " << q.lastError().text()
                << " (query was " << queries[i] << ")");
      }
    }

    // Time points.
    test::time_points tpoints;
    tpoints.store();

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5586");
    cbd_cfg.set("DB_NAME", DB_NAME);
    test::cbd broker;
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    test::sleep_for(3);
    tpoints.store();

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/tcp.xml.in");
    cbmod_cfg.set("BROKER_ID", "83");
    cbmod_cfg.set("BROKER_NAME", TEST_NAME "-cbmod");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("TCP_HOST", "localhost");
    cbmod_cfg.set("TCP_PORT", "5586");
    test::centengine_config engine_config;
    engine_config.generate_hosts(1);
    engine_config.generate_services(14);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_services().begin()),
         end(engine_config.get_services().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("passive_checks_enabled", "1");
      it->set("max_check_attempts", "1");
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("accept_passive_host_checks", "1");
    engine_config.set_directive("accept_passive_service_checks", "1");
    engine_config.set_directive("execute_host_checks", "0");
    engine_config.set_directive("execute_service_checks", "0");
    test::centengine engine(&engine_config);

    // Check default entry.
    precheck(tpoints);
    engine.start();
    test::sleep_for(2);
    tpoints.store();
    test::predicate null_predicate(test::predicate::type_null);
    test::table_content expected_bas;
    {
      test::predicate array[][4] = {
          {1, 100.0, 0.0, 0.0},  {2, 100.0, 0.0, 0.0},  {3, 100.0, 0.0, 0.0},
          {4, 100.0, 0.0, 0.0},  {5, 100.0, 0.0, 0.0},  {6, 100.0, 0.0, 0.0},
          {7, 100.0, 0.0, 0.0},  {8, 100.0, 0.0, 0.0},  {9, 75.0, 0.0, 0.0},
          {10, 100.0, 0.0, 0.0}, {11, 100.0, 0.0, 0.0}, {test::predicate()}};
      expected_bas = array;
    }
    test::table_content expected_kpis;
    {
      test::predicate array[][7] = {{1, 0, 1, 0.0, 0.0, 0.0, true},
                                    {2, 0, 1, 0.0, 0.0, 0.0, true},
                                    {3, 0, 1, 0.0, 0.0, 0.0, true},
                                    {4, 0, 1, 0.0, 0.0, 0.0, true},
                                    {5, 0, 1, 0.0, 0.0, 0.0, true},
                                    {6, 0, 1, 0.0, 0.0, 0.0, true},
                                    {7, 0, 1, 0.0, 0.0, 0.0, true},
                                    {8, 0, 1, 0.0, 0.0, 0.0, true},
                                    {9, 0, 1, 0.0, 0.0, 0.0, true},
                                    {10, 0, 1, 0.0, 0.0, 0.0, true},
                                    {11, 0, 1, 0.0, 0.0, 0.0, true},
                                    {12, 0, 1, 0.0, 0.0, 0.0, true},
                                    {13, 0, 1, 0.0, 0.0, 0.0, true},
                                    {14, 0, 1, 0.0, 0.0, 0.0, true},
                                    {15, 0, 1, 0.0, 0.0, 0.0, true},
                                    {16, 0, 1, 0.0, 0.0, 0.0, true},
                                    {17, 0, 1, 0.0, 0.0, 0.0, true},
                                    {18, 2, 1, 25.0, 0.0, 0.0, true},
                                    {19, 0, 1, 0.0, 0.0, 0.0, true},
                                    {20, 0, 1, 0.0, 0.0, 0.0, true},
                                    {21, 0, 1, 0.0, 0.0, 0.0, true},
                                    {22, 0, 1, 0.0, 0.0, 0.0, true},
                                    {23, 0, 1, 0.0, 0.0, 0.0, true},
                                    {24, 3, 1, 0.0, 0.0, 0.0, false},
                                    {test::predicate()}};
      expected_kpis = array;
    }
    test::table_content expected_ba_events;
    expected_ba_events.set_order_by(events_compare);
    {
      test::predicate array[12][6] = {
          {1, test::predicate(tpoints[0], tpoints[1] + 1), null_predicate,
           100.0, 0, false},
          {test::predicate()}};
      for (int i(1); i < 11; ++i) {
        array[i][0] = i + 1;
        for (int j(1); j < 7; ++j)
          array[i][j] = array[0][j];
      }
      // Boolean expressions have a default state that can immediately
      // impacts a BA.
      array[8][3] = 75.0;
      array[10][4] = 3;
      array[11][0] = test::predicate();
      expected_ba_events = array;
    }
    test::table_content expected_kpi_events;
    expected_kpi_events.set_order_by(events_compare);
    {
      test::predicate array[25][8] = {
          {1, test::predicate(tpoints[0], tpoints[1] + 1), null_predicate, 0,
           false, 0.0, "", ""},
          {test::predicate()}};
      for (int i(1); i < 24; ++i) {
        array[i][0] = i + 1;
        for (int j(1); j < 8; ++j)
          array[i][j] = array[0][j];
      }
      array[10][6] = "BA : BA2 - current_level = 100%";
      array[10][7] = "BA_Level=100%;80;70;0;100 BA_Downtime=0";
      array[11][6] = "BA : BA3 - current_level = 100%";
      array[11][7] = "BA_Level=100%;70;60;0;100 BA_Downtime=0";
      array[12][6] = "BA : BA4 - current_level = 100%";
      array[12][7] = "BA_Level=100%;60;50;0;100 BA_Downtime=0";
      array[13][6] = "BA : BA5 - current_level = 100%";
      array[13][7] = "BA_Level=100%;50;40;0;100 BA_Downtime=0";
      array[14][6] = "BA : BA6 - current_level = 100%";
      array[14][7] = "BA_Level=100%;40;30;0;100 BA_Downtime=0";
      array[15][6] = "BA : BA7 - current_level = 100%";
      array[15][7] = "BA_Level=100%;30;21;0;100 BA_Downtime=0";
      array[16][6] = "BAM boolean expression computed by Centreon Broker";
      array[17][3] = 2;
      array[17][5] = 25.0;
      array[17][6] = "BAM boolean expression computed by Centreon Broker";
      array[18][6] = "BAM boolean expression computed by Centreon Broker";
      array[22][6] = "BAM boolean expression computed by Centreon Broker";
      array[23][0] = test::predicate();
      expected_kpi_events = array;
    }
    postcheck(db, expected_bas, expected_kpis, expected_ba_events,
              expected_kpi_events);

    // Change service states.
    precheck(tpoints);
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;2;2;output1 for (1, 2)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;1;2;output1 for (1, 1)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;4;2;output1 for (1, 4)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;7;2;output1 for (1, 7)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;11;2;output1 for (1, 11)");
    test::sleep_for(3);
    //
    // BAs.
    //
    // Impacted by services.
    expected_bas[1][1] = 75.0;  // SVC1 C = 25 => W
    expected_bas[2][1] = 55.0;  // SVC2 C = 45 => C
    expected_bas[3][1] = 75.0;  // SVC4 C = 24 => O
    expected_bas[4][1] = 75.0;  // SVC7 C = 25 => O
    // Impacted by BAs.
    expected_bas[5][1] = 35.0;  // BA2 W  = 65 => W
    expected_bas[6][1] = 65.0;  // BA3 C  = 35 => O
    expected_bas[7][1] = 15.0;  // BA6 W  = 85 -> W
    expected_bas[8][1] = 0.0;   // BE1 F  = 75, BE2 F = 25, BE3 T = 6 => C
    expected_bas[9][1] = 75.0;  // SVC11 C = 25 => O
    //
    // KPIs.
    //
    expected_kpis[0][1] = 2;
    expected_kpis[0][3] = 25.0;
    expected_kpis[1][1] = 2;
    expected_kpis[1][3] = 45.0;
    expected_kpis[3][1] = 2;
    expected_kpis[3][3] = 25.0;
    expected_kpis[6][1] = 2;
    expected_kpis[6][3] = 25.0;
    expected_kpis[10][1] = 1;
    expected_kpis[10][3] = 65.0;
    expected_kpis[11][1] = 2;
    expected_kpis[11][3] = 35.0;
    expected_kpis[14][1] = 1;
    expected_kpis[14][3] = 85.0;
    expected_kpis[16][1] = 2;
    expected_kpis[16][3] = 75.0;
    expected_kpis[18][1] = 2;
    expected_kpis[18][3] = 6.0;
    expected_kpis[19][1] = 2;
    expected_kpis[19][3] = 25.0;
    //
    // BA events.
    //
    test::predicate last_interval(tpoints.prelast(), tpoints.last() + 1);
    // Close terminated BA events.
    {
      int terminated[] = {1, 2, 5, 7, 8, 9, -1};
      for (int i(0); terminated[i] >= 0; ++i)
        expected_ba_events[terminated[i]][2] = last_interval;
    }
    // Insert new events.
    {
      test::predicate new_rows[][6] = {
          {2, last_interval, null_predicate, 75.0, 1, false},
          {3, last_interval, null_predicate, 55.0, 2, false},
          {6, last_interval, null_predicate, 35.0, 1, false},
          {8, last_interval, null_predicate, 15.0, 1, false},
          {9, last_interval, null_predicate, 0.0, 2, false},
          {10, last_interval, null_predicate, 75.0, 1, false},
          {test::predicate()}};
      for (int i(0); new_rows[i][0].is_valid(); ++i)
        expected_ba_events.insert_row(new_rows[i]);
    }
    //
    // KPI events.
    //
    // Close terminated KPI events.
    {
      int terminated[] = {0, 1, 3, 6, 10, 11, 14, 16, 18, 19, -1};
      for (int i(0); terminated[i] >= 0; ++i)
        expected_kpi_events[terminated[i]][2] = last_interval;
    }
    // Insert new events.
    {
      test::predicate new_rows[][8] = {
          {1, last_interval, null_predicate, 2, false, 25.0,
           "output1 for (1, 1)\n", ""},
          {2, last_interval, null_predicate, 2, false, 45.0,
           "output1 for (1, 2)\n", ""},
          {4, last_interval, null_predicate, 2, false, 25.0,
           "output1 for (1, 4)\n", ""},
          {7, last_interval, null_predicate, 2, false, 25.0,
           "output1 for (1, 7)\n", ""},
          {11, last_interval, null_predicate, 1, false, 65.0,
           "BA : BA2 - current_level = 75%",
           "BA_Level=75%;80;70;0;100 BA_Downtime=0"},
          {12, last_interval, null_predicate, 2, false, 35.0,
           "BA : BA3 - current_level = 55%",
           "BA_Level=55%;70;60;0;100 BA_Downtime=0"},
          {15, last_interval, null_predicate, 1, false, 85.0,
           "BA : BA6 - current_level = 35%",
           "BA_Level=35%;40;30;0;100 BA_Downtime=0"},
          {17, last_interval, null_predicate, 2, false, 75.0,
           "BAM boolean expression computed by Centreon Broker", ""},
          {19, last_interval, null_predicate, 2, false, 6.0,
           "BAM boolean expression computed by Centreon Broker", ""},
          {20, last_interval, null_predicate, 2, false, 25.0,
           "output1 for (1, 11)\n", ""},
          {test::predicate()}};
      for (int i(0); new_rows[i][0].is_valid(); ++i)
        expected_kpi_events.insert_row(new_rows[i]);
    }
    postcheck(db, expected_bas, expected_kpis, expected_ba_events,
              expected_kpi_events);

    // Change service states.
    precheck(tpoints);
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;3;1;output2 for (1, 3)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;5;1;output2 for (1, 5)");
    test::sleep_for(2);
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;8;1;output2 for (1, 8)");
    engine.extcmd().execute(
        "PROCESS_SERVICE_CHECK_RESULT;1;12;2;output2 for (1, 12)");
    test::sleep_for(3);
    tpoints.store();
    //
    // BAs.
    //
    expected_bas[2][1] = 20.0;  // SVC2 C = 45, SVC3 W = 35 => C
    expected_bas[3][1] = 49.0;  // SVC4 C = 25, SVC5 W = 26 => C
    expected_bas[4][1] = 49.0;  // SVC7 C = 25, SVC8 W = 26 => W
    expected_bas[6][1] = 0.0;   // BA3 C  = 35, BA4 C  = 45, BA5 W = 45 => C
    expected_bas[7][1] = 0.0;   // BA6 W  = 85 => W, BA7 C = 105 => C
    expected_bas[9][1] = 50.0;  // SVC11 C = 25, SVC12 C = 25 => C
    //
    // KPIs.
    //
    expected_kpis[2][1] = 1;
    expected_kpis[2][3] = 35.0;
    expected_kpis[4][1] = 1;
    expected_kpis[4][3] = 26.0;
    expected_kpis[7][1] = 1;
    expected_kpis[7][3] = 26.0;
    expected_kpis[12][1] = 2;
    expected_kpis[12][3] = 45.0;
    expected_kpis[13][1] = 1;
    expected_kpis[13][3] = 45.0;
    expected_kpis[15][1] = 2;
    expected_kpis[15][3] = 105.0;
    expected_kpis[17][1] = 2;
    expected_kpis[17][3] = 25.0;
    expected_kpis[20][1] = 2;
    expected_kpis[20][3] = 25.0;
    //
    // BA events.
    //
    last_interval = test::predicate(tpoints.prelast(), tpoints.last() + 1);
    {
      test::predicate new_rows[][6] = {
          {4, last_interval, null_predicate, 49.0, 2, false},
          {5, last_interval, null_predicate, 49.0, 1, false},
          {7, last_interval, null_predicate, 20.0, 2, false},
          {8, last_interval, null_predicate, 0.0, 2, false},
          {10, last_interval, null_predicate, 50.0, 2, false},
          {test::predicate()}};
      for (int i(0); new_rows[i][0].is_valid(); ++i)
        expected_ba_events.insert_row(new_rows[i]);
      int terminated[] = {5, 7, 11, 14, 19, -1};
      for (int i(0); terminated[i] >= 0; ++i)
        expected_ba_events[terminated[i]][2] = last_interval;
    }
    //
    // KPI events.
    //
    {
      test::predicate new_rows[][8] = {
          {3, last_interval, null_predicate, 1, false, 35.0,
           "output2 for (1, 3)\n", ""},
          {5, last_interval, null_predicate, 1, false, 26.0,
           "output2 for (1, 5)\n", ""},
          {8, last_interval, null_predicate, 1, false, 26.0,
           "output2 for (1, 8)\n", ""},
          {13, last_interval, null_predicate, 2, false, 45.0,
           "BA : BA4 - current_level = 49%",
           "BA_Level=49%;60;50;0;100 BA_Downtime=0"},
          {14, last_interval, null_predicate, 1, false, 45.0,
           "BA : BA5 - current_level = 49%",
           "BA_Level=49%;50;40;0;100 BA_Downtime=0"},
          {16, last_interval, null_predicate, 2, false, 105.0,
           "BA : BA7 - current_level = 20%",
           "BA_Level=20%;30;21;0;100 BA_Downtime=0"},
          {21, last_interval, null_predicate, 2, false, 25.0,
           "output2 for (1, 12)\n", ""},
          {test::predicate()}};
      for (int i(0); new_rows[i][0].is_valid(); ++i)
        expected_kpi_events.insert_row(new_rows[i]);
      int terminated[] = {4, 8, 13, 21, 23, 27, 36, -1};
      for (int i(0); terminated[i] >= 0; ++i)
        expected_kpi_events[terminated[i]][2] = last_interval;
    }
    postcheck(db, expected_bas, expected_kpis, expected_ba_events,
              expected_kpi_events);

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
    broker.stop();
  } catch (std::exception const& e) {
    std::cout << "    " << e.what() << "\n";
  } catch (...) {
    std::cout << "    unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
