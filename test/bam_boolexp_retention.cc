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
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define CENTREON_DB_NAME "broker_bam_boolexp_retention_centreon"
#define BI_DB_NAME "broker_bam_boolexp_retention_bi"
#define BA_COUNT 1
#define HOST_COUNT 1
#define SERVICES_BY_HOST 1

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

/**
 *  Compare two double values.
 */
static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
}

/**
 *  Check content of the cfg_bam table.
 */
static void check_bas(QSqlDatabase& db, ba_state const* bas, size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT ba_id, current_level, downtime, acknowledged"
      "  FROM cfg_bam"
      "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch BAs at iteration " << iteration
                            << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg() << "not enough BAs at iteration " << iteration
                              << ": got " << i << ", expected " << count);
    if (!double_equals(q.value(1).toDouble(), bas[i].current_level) ||
        !double_equals(q.value(2).toDouble(), bas[i].downtime) ||
        !double_equals(q.value(3).toDouble(), bas[i].acknowledged))
      throw(exceptions::msg()
            << "invalid BA " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (level " << q.value(1).toDouble()
            << ", downtime " << q.value(2).toDouble() << ", acknowledged "
            << q.value(3).toDouble() << "), expected (" << bas[i].current_level
            << ", " << bas[i].downtime << ", " << bas[i].acknowledged << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BAs at iteration " << iteration
                            << ": expected " << count);
  return;
}

/**
 *  Check content of the cfg_bam_kpi table.
 */
static void check_kpis(QSqlDatabase& db, kpi_state const* kpis, size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT kpi_id, state_type, current_status, last_level,"
      "       downtime, acknowledged"
      "  FROM cfg_bam_kpi"
      "  ORDER BY kpi_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch KPIs at iteration " << iteration
                            << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg() << "not enough KPIs at iteration " << iteration
                              << ": got " << i << ", expected " << count);
    if ((q.value(1).toInt() != kpis[i].state_type) ||
        (q.value(2).toInt() != kpis[i].state) ||
        !double_equals(q.value(3).toDouble(), kpis[i].last_level) ||
        !double_equals(q.value(4).toDouble(), kpis[i].downtime) ||
        !double_equals(q.value(5).toDouble(), kpis[i].acknowledged))
      throw(exceptions::msg()
            << "invalid KPI " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (state type " << q.value(1).toInt()
            << ", state " << q.value(2).toInt() << ", level "
            << q.value(3).toDouble() << ", downtime " << q.value(4).toDouble()
            << ", acknowledged " << q.value(5).toDouble() << "), expected ("
            << kpis[i].state_type << ", " << kpis[i].state << ", "
            << kpis[i].last_level << ", " << kpis[i].downtime << ", "
            << kpis[i].acknowledged << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much KPIs at iteration " << iteration
                            << ": expected " << count);
  return;
}

/**
 *  Check functionnally that the boolexp state is retened.
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
  engine_extcmd commander;
  engine monitoring;
  cbd broker;
  test_db db;
  test_file monitoring_cfg;
  test_file broker_cfg;

  try {
    // Prepare database.
    db.open(CENTREON_DB_NAME, BI_DB_NAME, true);

    // Generate standard hosts and services.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it) {
      it->accept_passive_service_checks = 1;
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }

    // Generate virtual BA host and services.
    {
      host h;
      memset(&h, 0, sizeof(h));
      char const* str("virtual_ba_host");
      h.name = new char[strlen(str) + 1];
      strcpy(h.name, str);
      set_custom_variable(h, "HOST_ID", "1001");
      h.checks_enabled = 0;
      hosts.push_back(h);
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
        set_custom_variable(s, "SERVICE_ID", oss.str().c_str());
      }
      {
        std::ostringstream oss;
        oss << "1!" << i;
        std::string str(oss.str());
        s.service_check_command = new char[str.size() + 1];
        strcpy(s.service_check_command, str.c_str());
      }
      services.push_back(s);
    }

    // Update properties of all services.
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it) {
      it->accept_passive_service_checks = 1;
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }

    // Generate commands.
    generate_commands(commands, 1);
    {
      char const* cmdline;
      cmdline = MY_PLUGIN_BAM_PATH " " CENTREON_DB_NAME " $ARG1$";
      command& cmd(commands.front());
      cmd.command_line = new char[strlen(cmdline) + 1];
      strcpy(cmd.command_line, cmdline);
    }

    commander.set_file(tmpnam(NULL));
    monitoring_cfg.set_template(PROJECT_SOURCE_DIR
                                "/test/cfg/bam_boolexp_retention1.xml.in");
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << monitoring_cfg.generate() << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services, &commands);

    // Generate broker config files.
    broker_cfg.set_template(PROJECT_SOURCE_DIR
                            "/test/cfg/bam_boolexp_retention2.xml.in");
    broker_cfg.set("COMMAND_FILE", commander.get_file());
    broker_cfg.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    broker_cfg.set("DB_NAME_BI", BI_DB_NAME);
    broker.set_config_file(broker_cfg.generate());

    // Create organization.
    {
      QString query;
      query =
          "INSERT INTO cfg_organizations (organization_id, name, shortname)"
          "  VALUES (1, '42', '42')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create organization: " << q.lastError().text());
    }

    // Create timeperiods.
    {
      QString query;
      query =
          "INSERT INTO cfg_timeperiods (tp_id, tp_name, tp_alias, "
          "            tp_sunday, tp_monday, tp_tuesday,"
          "            tp_wednesday, tp_thursday, tp_friday, "
          "            tp_saturday, organization_id)"
          "  VALUES (1, '24x7', '24x7', '00:00-24:00',"
          "          '00:00-24:00', '00:00-24:00', '00:00-24:00',"
          "          '00:00-24:00', '00:00-24:00', '00:00-24:00', 1)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create timeperiods: " << q.lastError().text());
    }

    // Create host/service entries.
    {
      for (int i(1); i <= HOST_COUNT; ++i) {
        {
          std::ostringstream oss;
          oss << "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
              << "  VALUES (" << i << ", '" << i << "', 1)";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw(exceptions::msg() << "could not create host " << i << ": "
                                    << q.lastError().text());
        }
        for (int j((i - 1) * SERVICES_BY_HOST + 1), limit(i * SERVICES_BY_HOST);
             j <= limit; ++j) {
          {
            std::ostringstream oss;
            oss << "INSERT INTO cfg_services (service_id, service_description, "
                   "organization_id)"
                << "  VALUES (" << j << ", '" << j << "', 1)";
            QSqlQuery q(*db.centreon_db());
            if (!q.exec(oss.str().c_str()))
              throw(exceptions::msg()
                    << "could not create service (" << i << ", " << j
                    << "): " << q.lastError().text());
          }
          {
            std::ostringstream oss;
            oss << "INSERT INTO cfg_hosts_services_relations (host_host_id, "
                   "service_service_id)"
                << "  VALUES (" << i << ", " << j << ")";
            QSqlQuery q(*db.centreon_db());
            if (!q.exec(oss.str().c_str()))
              throw(exceptions::msg()
                    << "could not link service " << j << " to host " << i
                    << ": " << q.lastError().text());
          }
        }
      }
    }

    // Create BAs.
    {
      {
        QString query(
            "INSERT INTO cfg_bam_ba_types (ba_type_id, name,"
            "            slug, description)"
            "  VALUES (1, 'Default', 'default', 'Default type')");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw(exceptions::msg()
                << "could not create BA types: " << q.lastError().text());
      }
      {
        QString query(
            "INSERT INTO cfg_bam (ba_id, name, level_w, level_c,"
            "            activate, id_reporting_period,"
            "            ba_type_id, organization_id)"
            "  VALUES (1, 'BA1', 90, 80, 1, 1, 1, 1)");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw(exceptions::msg()
                << "could not create BAs: " << q.lastError().text());
      }
      {
        QString query;
        query =
            "INSERT INTO cfg_bam_poller_relations (ba_id, poller_id)"
            "  VALUES (1, 42)";
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw(exceptions::msg() << "could not create BAs / poller relations: "
                                  << q.lastError().text());
      }

      // Create associated services.
      {
        QString query(
            "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
            "  VALUES (1001, 'virtual_ba_host', 1)");
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(query))
          throw(exceptions::msg() << "could not create virtual BA host: "
                                  << q.lastError().text());
      }
      for (int i(1); i <= BA_COUNT; ++i) {
        {
          std::ostringstream oss;
          oss << "INSERT INTO cfg_services (service_id, service_description, "
                 "organization_id)"
              << "  VALUES (" << 1000 + i << ", 'ba_" << i << "', 1)";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw(exceptions::msg() << "could not create virtual service of BA "
                                    << i << ": " << q.lastError().text());
        }
        {
          std::ostringstream oss;
          oss << "INSERT INTO cfg_hosts_services_relations (host_host_id, "
              << "            service_service_id)"
              << "  VALUES (1001, " << 1000 + i << ")";
          QSqlQuery q(*db.centreon_db());
          if (!q.exec(oss.str().c_str()))
            throw(exceptions::msg()
                  << "could not create link between virtual host "
                  << "and virtual service of BA " << i << ": "
                  << q.lastError().text());
        }
      }
    }

    // Create boolean expressions.
    {
      QString query(
          "INSERT INTO cfg_bam_boolean (boolean_id, name,"
          "            expression, bool_state, activate)"
          "  VALUES (1, 'BoolExp1', '{1 1} {is} {OK}', 0, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create boolexps: " << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query(
          "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
          "            service_id, id_indicator_ba, id_ba,"
          "            meta_id, boolean_id, config_type,"
          "            drop_warning, drop_warning_impact_id,"
          "            drop_critical, drop_critical_impact_id,"
          "            drop_unknown, drop_unknown_impact_id,"
          "            ignore_downtime, ignore_acknowledged,"
          "            state_type, activate)"
          "  VALUES (1, '3', NULL, NULL, NULL, 1, NULL, 1, '0', 10, NULL, 75, "
          "NULL, 99, NULL, '0', '0', '1', '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create KPIs: " << q.lastError().text());
    }

    // Start monitoring engine and broker.
    time_t t0(time(NULL));
    std::cout << "T0: " << t0 << "\n";
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    broker.start();

    // Let the daemon initialize.
    sleep_for(5);

    // #0
    time_t t1(time(NULL));
    std::cout << "T1: " << t1 << "\n";
    {
      ba_state const bas[] = {{100.0, 0.0, 0.0}};
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
          {1, 0, 0.0, 0.0, 0.0},
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output1 for (1, 1)");

    // Sleep a while.
    sleep_for(8);

    // Check that the kpi was correctly impacted.
    time_t t2(time(NULL));
    std::cout << "T2: " << t2 << "\n";
    {
      ba_state const bas[] = {{25.0, 0.0, 0.0}};
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
          {1, 2, 75.0, 0.0, 0.0},
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    broker.stop();
    // Let the daemon stop.
    sleep_for(5);
    broker.start();
    // Let the daemon start.
    sleep_for(5);
    time_t t3(time(NULL));

    // Check that the kpi didn't change.
    std::cout << "T3: " << t3 << "\n";
    {
      ba_state const bas[] = {// Impacted by services.
                              {25.0, 0.0, 0.0}};
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
          {1, 2, 75.0, 0.0, 0.0},
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    // Modify service states.
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;0;output2 for (1, 1)");

    // Sleep a while.
    sleep_for(40);

    // Check that the kpi was correctly impacted.
    time_t t4(time(NULL));
    std::cout << "T4: " << t4 << "\n";
    {
      ba_state const bas[] = {{100.0, 0.0, 0.0}};
      check_bas(*db.centreon_db(), bas, sizeof(bas) / sizeof(*bas));
    }
    {
      kpi_state const kpis[] = {
          {1, 0, 0.0, 0.0, 0.0},
      };
      check_kpis(*db.centreon_db(), kpis, sizeof(kpis) / sizeof(*kpis));
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    db.set_remove_db_on_close(false);
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    db.set_remove_db_on_close(false);
  }

  // Cleanup.
  monitoring.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
