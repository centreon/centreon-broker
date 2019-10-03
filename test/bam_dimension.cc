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
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define CENTREON_DB_NAME "broker_bam_centreon_dimension"
#define BI_DB_NAME "broker_bam_bi_dimension"
#define COMMAND_FILE "command_file_dimension"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 10

static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
}

struct ba_dimension {
  uint32_t id;
  const char* name;
  const char* description;
  double month_percent_warn;
  double month_percent_crit;
  double month_duration_warn;
  double month_duration_crit;
};

static void check_bas(QSqlDatabase& db, ba_dimension const* bas, size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT ba_id, ba_name, ba_description,"
      "       sla_month_percent_crit, sla_month_percent_warn,"
      "       sla_month_duration_crit, sla_month_duration_warn"
      "  FROM mod_bam_reporting_ba"
      "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch BAs at iteration " << iteration
                            << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg() << "not enough BAs at iteration " << iteration
                              << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != bas[i].id ||
        q.value(1).toString() != bas[i].name ||
        q.value(2).toString() != bas[i].description ||
        !double_equals(q.value(3).toDouble(), bas[i].month_percent_crit) ||
        !double_equals(q.value(4).toDouble(), bas[i].month_percent_warn) ||
        !double_equals(q.value(5).toDouble(), bas[i].month_duration_crit) ||
        !double_equals(q.value(6).toDouble(), bas[i].month_duration_warn))
      throw(exceptions::msg()
            << "invalid BA " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (id " << q.value(0).toUInt() << ", name "
            << q.value(1).toString() << ", description "
            << q.value(2).toString() << ", sla_month_percent_crit "
            << q.value(3).toDouble() << ", sla_month_percent_warn "
            << q.value(4).toDouble() << ", sla_month_duration_crit "
            << q.value(5).toDouble() << ", sla_month_duration_warn "
            << q.value(6).toDouble() << ") expected (" << bas[i].id << ", "
            << bas[i].name << ", " << bas[i].description << ", "
            << bas[i].month_percent_crit << ", " << bas[i].month_percent_warn
            << ", " << bas[i].month_duration_crit << ", "
            << bas[i].month_duration_warn << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BAs at iteration " << iteration
                            << ": expected " << count);
  return;
}

struct bv_dimension {
  uint32_t id;
  const char* name;
  const char* description;
};

static void check_bvs(QSqlDatabase& db, bv_dimension const* bvs, size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT bv_id, bv_name, bv_description"
      "  FROM mod_bam_reporting_bv"
      "  ORDER BY bv_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch BVs at iteration " << iteration
                            << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg() << "not enough BVs at iteration " << iteration
                              << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != bvs[i].id ||
        q.value(1).toString() != bvs[i].name ||
        q.value(2).toString() != bvs[i].description)
      throw(exceptions::msg()
            << "invalid BVs " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (id " << q.value(0).toUInt() << ", name "
            << q.value(1).toString() << ", description "
            << q.value(2).toString() << ") expected (" << bvs[i].id << ", "
            << bvs[i].name << ", " << bvs[i].description << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BVs at iteration " << iteration
                            << ": expected " << count);
  return;
}

struct ba_bv_dimension {
  uint32_t ba_id;
  uint32_t bv_id;
};

static void check_ba_bv_links(QSqlDatabase& db,
                              ba_bv_dimension const* babvs,
                              size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT ba_id, bv_id"
      "  FROM mod_bam_reporting_relations_ba_bv"
      "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch BA-BV links at iteration "
                            << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg()
            << "not enough BA-BV links at iteration " << iteration << ": got "
            << i << ", expected " << count);
    if (q.value(0).toUInt() != babvs[i].ba_id ||
        q.value(1).toUInt() != babvs[i].bv_id)
      throw(exceptions::msg()
            << "invalid BA-BV links " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (ba id " << q.value(0).toUInt() << ", bv id "
            << q.value(1).toUInt() << ") expected (" << babvs[i].ba_id << ", "
            << babvs[i].bv_id << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much BA-BV links at iteration " << iteration
                            << ": expected " << count);
  return;
}

struct kpi_dimension {
  uint32_t kpi_id;
  const char* kpi_name;
  uint32_t ba_id;
  const char* ba_name;
  uint32_t host_id;
  const char* host_name;
  uint32_t service_id;
  const char* service_description;
  uint32_t kpi_ba_id;
  const char* kpi_ba_name;
  uint32_t meta_service_id;
  const char* meta_service_name;
  double impact_warning;
  double impact_critical;
  double impact_unknown;
  uint32_t boolean_id;
  const char* boolean_name;
};

static void check_kpis(QSqlDatabase& db,
                       kpi_dimension const* kpis,
                       size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
      "SELECT kpi_id, kpi_name,"
      "       ba_id, ba_name, host_id, host_name,"
      "       service_id, service_description, kpi_ba_id,"
      "       kpi_ba_name, meta_service_id, meta_service_name,"
      "       impact_warning, impact_critical, impact_unknown,"
      "       boolean_id, boolean_name"
      "  FROM mod_bam_reporting_kpi"
      "  ORDER BY kpi_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw(exceptions::msg() << "could not fetch KPIs at iteration " << iteration
                            << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw(exceptions::msg() << "not enough KPIs at iteration " << iteration
                              << ": got " << i << ", expected " << count);
    if (q.value(0).toUInt() != kpis[i].kpi_id ||
        q.value(1).toString() != kpis[i].kpi_name ||
        q.value(2).toUInt() != kpis[i].ba_id ||
        q.value(3).toString() != kpis[i].ba_name ||
        q.value(4).toUInt() != kpis[i].host_id ||
        q.value(5).toString() != kpis[i].host_name ||
        q.value(6).toUInt() != kpis[i].service_id ||
        q.value(7).toString() != kpis[i].service_description ||
        q.value(8).toUInt() != kpis[i].kpi_ba_id ||
        q.value(9).toString() != kpis[i].kpi_ba_name ||
        q.value(10).toUInt() != kpis[i].meta_service_id ||
        q.value(11).toString() != kpis[i].meta_service_name ||
        !double_equals(q.value(12).toDouble(), kpis[i].impact_warning) ||
        !double_equals(q.value(13).toDouble(), kpis[i].impact_critical) ||
        !double_equals(q.value(14).toDouble(), kpis[i].impact_unknown) ||
        q.value(15).toUInt() != kpis[i].boolean_id ||
        q.value(16).toString() != kpis[i].boolean_name)
      throw(exceptions::msg()
            << "invalid KPIs " << q.value(0).toUInt() << " at iteration "
            << iteration << ": got (kpi name " << q.value(1).toString()
            << ", ba id " << q.value(2).toUInt() << ", ba_name "
            << q.value(3).toString() << ", host_id " << q.value(4).toUInt()
            << ", host_name " << q.value(5).toString() << ", service_id "
            << q.value(6).toUInt() << ", service_description "
            << q.value(7).toString() << ", kpi_ba_id " << q.value(8).toUInt()
            << ", kpi_ba_name " << q.value(9).toString() << ", meta_service_id "
            << q.value(10).toUInt() << ", meta_service_name "
            << q.value(11).toString() << ", impact_warning "
            << q.value(12).toDouble() << ", impact_critical "
            << q.value(13).toDouble() << ", impact_unknown "
            << q.value(14).toDouble() << ", boolean_id " << q.value(15).toUInt()
            << ", boolean_name " << q.value(16).toString() << ") expected ("
            << kpis[i].kpi_name << ", " << kpis[i].ba_id << ", "
            << kpis[i].ba_name << ", " << kpis[i].host_id << ", "
            << kpis[i].host_name << ", " << kpis[i].service_id << ", "
            << kpis[i].service_description << ", " << kpis[i].kpi_ba_id << ", "
            << kpis[i].kpi_ba_name << ", " << kpis[i].meta_service_id << ", "
            << kpis[i].meta_service_name << ", " << kpis[i].impact_warning
            << ", " << kpis[i].impact_critical << ", " << kpis[i].impact_unknown
            << ", " << kpis[i].boolean_id << ", " << kpis[i].boolean_name
            << ")");
  }
  if (q.next())
    throw(exceptions::msg() << "too much KPIs at iteration " << iteration
                            << ": expected " << count);
  return;
}

/**
 *  Check that the BAM broker correctly copy the dimension tables to the BI DB.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
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

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);

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
             j < limit; ++j) {
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
          "                     activate, ba_type_id, organization_id)"
          "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60, '1', 1, 1),"
          "         (2, 'BA2', 'DESC2', 80, 70, 60, 50, '1', 1, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BAs: " << q.lastError().text());
    }
    {
      QString query(
          "INSERT INTO cfg_bam_poller_relations (ba_id, poller_id)"
          "  VALUES (1, 42), (2, 42)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create BA/poller relations: "
                                << q.lastError().text());
    }
    {
      QString queries[] = {
          "INSERT INTO cfg_hosts (host_id, host_name, organization_id)"
          "  VALUES (1001, 'Virtual BA host', 1)",
          "INSERT INTO cfg_services (service_id, service_description, "
          "organization_id)"
          "  VALUES (1001, 'ba_1', 1), (1002, 'ba_2', 1), (1003, 'meta_1', 1), "
          "(1004, 'meta_2', 1)",
          "INSERT INTO cfg_hosts_services_relations (host_host_id, "
          "service_service_id)"
          "  VALUES (1001, 1001), (1001, 1002), (1001, 1003), (1001, 1004)"};
      for (size_t i(0); i < sizeof(queries) / sizeof(*queries); ++i) {
        QSqlQuery q(*db.centreon_db());
        if (!q.exec(queries[i]))
          throw(exceptions::msg()
                << "could not create virtual BA services: "
                << "query " << i << " failed: " << q.lastError().text());
      }
    }

    // Create boolean expressions.
    {
      QString query(
          "INSERT INTO cfg_bam_boolean (boolean_id, name,"
          "            expression, bool_state, activate)"
          "  VALUES (1, 'BoolExp1', '{1 1} {is} {OK}', 0, 1),"
          "         (2, 'BoolExp2', '{1 2} {not} {CRITICAL} {OR} {1 3} {not} "
          "{OK}', 1, 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create boolexps: " << q.lastError().text());
    }

    // Create meta-services.
    {
      QString query(
          "INSERT INTO cfg_meta_services (meta_id, meta_name, meta_activate, "
          "organization_id)"
          "  VALUES (1, 'Meta1', '1', 1),"
          "         (2, 'Meta2', '1', 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create the meta services: "
                                << q.lastError().text());
    }

    // Create BVs.
    {
      QString query(
          "INSERT INTO cfg_bam_ba_groups (id_ba_group, ba_group_name,"
          "                               ba_group_description)"
          "  VALUES (1, 'BaGroup1', 'BaGroupDescription1'),"
          "         (2, 'BaGroup2', 'BaGroupDescription2')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create the bvs: " << q.lastError().text());
    }

    // Create the BA/BV relations.
    {
      QString query(
          "INSERT INTO cfg_bam_bagroup_ba_relation (id_ba, id_ba_group)"
          "  VALUES (2, 1),"
          "         (1, 2)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create the ba bv relations: "
                                << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query(
          "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
          "            service_id, id_indicator_ba, id_ba,"
          "            meta_id, boolean_id, config_type, drop_warning,"
          "            drop_warning_impact_id, drop_critical,"
          "            drop_critical_impact_id, drop_unknown,"
          "            drop_unknown_impact_id, ignore_downtime,"
          "            ignore_acknowledged, activate)"
          "  VALUES (1, '0', 1, 1, NULL, 1, NULL, NULL, '0', 15, NULL, 25, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (2, '0', 1, 2, NULL, 2, NULL, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (4, '1', NULL, NULL, 2, 1, NULL, NULL, '0', 25, NULL, 35, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (5, '2', NULL, NULL, NULL, 1, 1, NULL, '0', 35, NULL, 45, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (6, '2', NULL, NULL, NULL, 2, 2, NULL, '0', 45, NULL, 55, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (7, '3', NULL, NULL, NULL, 1, NULL, 1, '0', 85, NULL, 95, "
          "NULL, 99, NULL, '0', '0', '1'),"
          "         (8, '3', NULL, NULL, NULL, 2, NULL, 2, '0', 95, NULL, 105, "
          "NULL, 99, NULL, '0', '0', '1')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create KPIs: " << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(config_file);
    broker.start();
    sleep_for(2);
    broker.update();

    // Let the broker do its thing.
    sleep_for(3);

    // Check that the dimensions were correctly copied.

    // Check bas
    ba_dimension bas[] = {{1, "BA1", "DESC1", 90, 80, 70, 60},
                          {2, "BA2", "DESC2", 80, 70, 60, 50}};
    check_bas(*db.bi_db(), bas, sizeof(bas) / sizeof(*bas));

    // Check bvs
    bv_dimension bvs[] = {{1, "BaGroup1", "BaGroupDescription1"},
                          {2, "BaGroup2", "BaGroupDescription2"}};
    check_bvs(*db.bi_db(), bvs, sizeof(bvs) / sizeof(*bvs));

    // Check ba-bv links.
    ba_bv_dimension babvs[] = {{1, 2}, {2, 1}};
    check_ba_bv_links(*db.bi_db(), babvs, sizeof(babvs) / sizeof(*babvs));

    // Check kpis.
    kpi_dimension kpis[] = {
        // Host/Service kpis.
        {1, "1 1", 1, "BA1", 1, "1", 1, "1", 0, NULL, 0, NULL, 15, 25, 99, 0,
         NULL},
        {2, "1 2", 2, "BA2", 1, "1", 2, "2", 0, NULL, 0, NULL, 35, 45, 99, 0,
         NULL},
        // Ba kpis.
        {4, "BA2", 1, "BA1", 0, NULL, 0, NULL, 2, "BA2", 0, NULL, 25, 35, 99, 0,
         NULL},
        // Meta service kpis.
        {5, "Meta1", 1, "BA1", 0, NULL, 0, NULL, 0, NULL, 1, "Meta1", 35, 45,
         99, 0, NULL},
        {6, "Meta2", 2, "BA2", 0, NULL, 0, NULL, 0, NULL, 2, "Meta2", 45, 55,
         99, 0, NULL},
        // Boolean kpis.
        {7, "BoolExp1", 1, "BA1", 0, NULL, 0, NULL, 0, NULL, 0, NULL, 85, 95,
         99, 1, "BoolExp1"},
        {8, "BoolExp2", 2, "BA2", 0, NULL, 0, NULL, 0, NULL, 0, NULL, 95, 105,
         99, 2, "BoolExp2"},
    };
    check_kpis(*db.bi_db(), kpis, sizeof(kpis) / sizeof(*kpis));

    // Erase everything.
    {
      QString query("DELETE FROM cfg_bam");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not truncate the table cfg_bam: "
                                << q.lastError().text());
      query = "DELETE FROM cfg_bam_kpi";
      if (!q.exec(query))
        throw(exceptions::msg() << "could not truncate the table cfg_bam_kpi: "
                                << q.lastError().text());
      query = "DELETE FROM cfg_bam_ba_groups";
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not truncate the table cfg_bam_ba_groups: "
              << q.lastError().text());
      query = "DELETE FROM cfg_bam_bagroup_ba_relation";
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not truncate the table cfg_bam_bagroup_ba_relation: "
              << q.lastError().text());
    }

    // Update the broker.
    broker.update();
    // Let the broker do its thing.
    sleep_for(3);

    // Check that everything was deleted.
    {
      QString query("SELECT * FROM mod_bam_reporting_ba");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not select the table mod_bam_reporting_ba: "
              << q.lastError().text());
      if (q.size() != 0)
        throw(exceptions::msg()
              << "the table mod_bam_reporting_ba wasn't updated: "
              << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_bv";
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not select the table mod_bam_reporting_bv: "
              << q.lastError().text());
      if (q.size() != 0)
        throw(exceptions::msg()
              << "the table mod_bam_reporting_bv wasn't updated: "
              << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_relations_ba_bv";
      if (!q.exec(query))
        throw(
            exceptions::msg()
            << "could not select the table mod_bam_reporting_relations_ba_bv: "
            << q.lastError().text());
      if (q.size() != 0)
        throw(exceptions::msg()
              << "the table mod_bam_reporting_relations_ba_bv wasn't updated: "
              << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_kpi";
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not select the table mod_bam_reporting_kpi: "
              << q.lastError().text());
      if (q.size() != 0)
        throw(exceptions::msg()
              << "the table mod_bam_reporting_kpi wasn't updated: "
              << q.lastError().text());
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
