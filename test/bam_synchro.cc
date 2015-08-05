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
#include "test/broker_extcmd.hh"

using namespace com::centreon::broker;

#define CENTREON_DB_NAME "broker_bam_synchro_centreon"
#define CENTREON_DB_WRITER_NAME "broker_bam_synchro_writer_centreon"
#define BA_COUNT 11
#define HOST_COUNT 1
#define SERVICES_BY_HOST 14

/**
 *  Check that the two dbs BAs are synchronized.
 *
 *  @param[in] db1  The first db.
 *  @param[in] db2  The second db.
 */
static void check_bas(QSqlDatabase& db1, QSqlDatabase& db2) {
  QString query(
            "SELECT ba_id, current_level, downtime, acknowledged"
            "  FROM mod_bam"
            "  ORDER BY ba_id");
  QSqlQuery q1(db1);
  QSqlQuery q2(db2);
  if (!q1.exec(query) || !q1.exec(query))
    throw (exceptions::msg() << "could not fetch BAs");

  unsigned int row_index = 0;

  while (q1.next() && q2.next()) {
    ++row_index;
    for (unsigned int i = 0; ;++i) {
      QVariant val1 = q1.value(i);
      QVariant val2 = q2.value(i);
      if (val1 != val2)
        throw (exceptions::msg() << "unequal ba found at row " << row_index);
      if (!val1.isValid() || !val2.isValid())
        break ;
    }
  }
}

/**
 *  Check that the two dbs KPIs are synchronized.
 *
 *  @param[in] db1  The first db.
 *  @param[in] db2  The second db.
 */
static void check_kpis(QSqlDatabase& db1, QSqlDatabase& db2) {
  QString query(
            "SELECT kpi_id, state_type, current_status, last_level,"
            "       downtime, acknowledged, valid"
            "  FROM mod_bam_kpi"
            "  ORDER BY kpi_id");
  QSqlQuery q1(db1);
  QSqlQuery q2(db2);
  if (!q1.exec(query) || !q1.exec(query))
    throw (exceptions::msg() << "could not fetch KPIs");

  unsigned int row_index = 0;

  while (q1.next() && q2.next()) {
    ++row_index;
    for (unsigned int i = 0; ;++i) {
      QVariant val1 = q1.value(i);
      QVariant val2 = q2.value(i);
      if (val1 != val2)
        throw (exceptions::msg() << "unequal kpi found at row " << row_index);
      if (!val1.isValid() || !val2.isValid())
        break ;
    }
  }
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
  broker_extcmd broker_command_file;
  engine monitoring;
  test_db db;
  test_db db2;
  test_file cfg;

  try {
    // Prepare database.
    db.open(NULL, NULL, CENTREON_DB_NAME);
    db2.open(NULL, NULL, CENTREON_DB_WRITER_NAME);

    // Prepare monitoring engine configuration parameters.
    commander.set_file(tmpnam(NULL));
    broker_command_file.set_file(tmpnam(NULL));
    cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam_synchro.xml.in");
    cfg.set("COMMAND_FILE", commander.get_file());
    cfg.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    cfg.set("DB_WRITER_CENTREON", CENTREON_DB_WRITER_NAME);
    cfg.set("BROKER_COMMAND_FILE", broker_command_file.get_file());
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
      additional_config.c_str());

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
             j <= limit;
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
                  "         (9, 'BA9', 10, 0, '1', 1),"
                  "         (10, 'BA10', 80, 60, '1', 1),"
                  "         (11, 'BA11', 70, 50, '1', 1)");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw (exceptions::msg() << "could not create BAs: "
                 << q.lastError().text());
      }

      // Create BAs poller relation.
      {
        QString query(
                  "INSERT INTO mod_bam_poller_relations (ba_id, poller_id)"
                  "  VALUES (1, 42),"
                  "         (2, 42),"
                  "         (3, 42),"
                  "         (4, 42),"
                  "         (5, 42),"
                  "         (6, 42),"
                  "         (7, 42),"
                  "         (8, 42),"
                  "         (9, 42),"
                  "         (10, 42),"
                  "         (11, 42)");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw (exceptions::msg()
                 << "could not create BA/poller relations: "
                 << q.lastError().text());
      }

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
      for (int i(1); i <= BA_COUNT; ++i) {
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
                "         (3, 'BoolExp3', '({1 5} {not} {WARNING} {AND} {1 6} {is} {WARNING}) {OR} {1 7} {is} {CRITICAL}', 1, 1),"
                "         (4, 'BoolExp4', '{1 14} {is} {CRITICAL}', 1, 1),"
                "         (5, 'BoolExp5', '{Non Existent} {is} {OK}', 1, 1)");
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
                "         (19, '3', NULL, NULL, NULL, 9, NULL, 3, '0', 10, NULL, 6, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (20, '0', 1, 11, NULL, 10, NULL, NULL, '0', NULL, NULL, NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (21, '0', 1, 12, NULL, 10, NULL, NULL, '0', NULL, NULL, NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (22, '0', 1, 13, NULL, 10, NULL, NULL, '0', NULL, NULL, NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (23, '3', NULL, NULL, NULL, 10, NULL, 4, '0', NULL, NULL, NULL, NULL, 99, NULL, '0', '0', '1', '1'),"
                "         (24, '3', NULL, NULL, NULL, 11, NULL, 5, '0', NULL, NULL, NULL, NULL, 99, NULL, '0', '0', '1', '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create KPIs: "
               << q.lastError().text());
    }

    // Start monitoring engine.
    time_t t0(time(NULL));
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();

    // Let the daemon initialize.
    sleep_for(20 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Send the copy signal.
    broker_command_file.execute("EXECUTE;DBReader;SYNC_CFG_DB;42");

    // Sleep a while.
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    check_bas(*db.centreon_db(), *db2.centreon_db());
    check_kpis(*db.centreon_db(), *db2.centreon_db());
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
