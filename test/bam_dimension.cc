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

#define CENTREON_DB_NAME "broker_bam_centreon_dimension"
#define BI_DB_NAME "broker_bam_bi_dimension"
#define COMMAND_FILE "command_file_dimension"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 10

static bool double_equals(double d1, double d2) {
  return (fabs(d1 - d2) < 0.0001);
}

struct ba_dimension {
  unsigned int id;
  const char* name;
  const char* description;
  double month_p1;
  double month_p2;
  double month_d1;
  double month_d2;
};

static void check_bas(
            QSqlDatabase& db,
            ba_dimension const* bas,
            size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT ba_id, ba_name, ba_description,"
            "       sla_month_percent_1, sla_month_percent_2,"
            "       sla_month_duration_1, sla_month_duration_2"
            "  FROM mod_bam_reporting_ba"
            "  ORDER BY ba_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch BAs at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BAs at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toInt() != bas[i].id
        || q.value(1).toString() != bas[i].name
        || q.value(2).toString() != bas[i].description
        || !double_equals(q.value(3).toDouble(), bas[i].month_p1)
        || !double_equals(q.value(4).toDouble(), bas[i].month_p2)
        || !double_equals(q.value(5).toDouble(), bas[i].month_d1)
        || !double_equals(q.value(6).toDouble(), bas[i].month_d2))
      throw (exceptions::msg() << "invalid BA " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (id "
             << q.value(0).toInt() << ", name "
             << q.value(1).toString() << ", description "
             << q.value(2).toString() << ", sla_month_percent1 "
             << q.value(3).toDouble() << ", sla_month_percent2 "
             << q.value(4).toDouble() << ", sla_month_duration1 "
             << q.value(5).toDouble() << ", sla_month_duration2 "
             << q.value(6).toDouble() << ") expected ("
             << bas[i].id << ", " << bas[i].name << ", "
             << bas[i].description << ", " << bas[i].month_p1
             << ", " << bas[i].month_p2 << ", " << bas[i].month_d1
             << ", " << bas[i].month_d2 <<  ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BAs at iteration "
           << iteration << ": expected " << count);
  return ;
}

struct bv_dimension {
  unsigned int id;
  const char* name;
  const char* description;
};

static void check_bvs(QSqlDatabase& db,
                      bv_dimension const* bvs,
                      size_t count) {
  static int iteration(-1);
  ++iteration;
  QString query(
            "SELECT bv_id, bv_name, bv_description"
            "  FROM mod_bam_reporting_bv"
            "  ORDER BY bv_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg() << "could not fetch BVs at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BVs at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toInt() != bvs[i].id
        || q.value(1).toString() != bvs[i].name
        || q.value(2).toString() != bvs[i].description)
      throw (exceptions::msg() << "invalid BVs " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (id "
             << q.value(0).toInt() << ", name "
             << q.value(1).toString() << ", description "
             << q.value(2).toString() << ") expected ("
             << bvs[i].id << ", " << bvs[i].name << ", "
             << bvs[i].description << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BVs at iteration "
           << iteration << ": expected " << count);
  return ;
}

struct ba_bv_dimension {
  unsigned int ba_id;
  unsigned int bv_id;
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
    throw (exceptions::msg() << "could not fetch BA-BV links at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA-BV links at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toInt() != babvs[i].ba_id
        || q.value(1).toInt() != babvs[i].bv_id)
      throw (exceptions::msg() << "invalid BA-BV links " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (ba id "
             << q.value(0).toInt() << ", bv id "
             << q.value(1).toInt() << ") expected ("
             << babvs[i].ba_id << ", " << babvs[i].bv_id << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA-BV links at iteration "
           << iteration << ": expected " << count);
  return ;
}

struct kpi_dimension {
  unsigned int kpi_id;
  const char* kpi_name;
  unsigned int ba_id;
  const char* ba_name;
  unsigned int host_id;
  const char* host_name;
  unsigned int service_id;
  const char* service_description;
  unsigned int kpi_ba_id;
  const char* kpi_ba_name;
  unsigned int meta_service_id;
  const char* meta_service_name;
  double impact_warning;
  double impact_critical;
  double impact_unknown;
  unsigned int boolean_id;
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
    throw (exceptions::msg() << "could not fetch KPIs at iteration "
           << iteration << ": " << q.lastError().text());
  for (size_t i(0); i < count; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough KPIs at iteration "
             << iteration << ": got " << i << ", expected " << count);
    if (q.value(0).toInt() != kpis[i].kpi_id
        || q.value(1).toString() != kpis[i].kpi_name
        || q.value(2).toInt() != kpis[i].ba_id
        || q.value(3).toString() != kpis[i].ba_name
        || q.value(4).toInt() != kpis[i].host_id
        || q.value(5).toString() != kpis[i].host_name
        || q.value(6).toInt() != kpis[i].service_id
        || q.value(7).toString() != kpis[i].service_description
        || q.value(8).toInt() != kpis[i].kpi_ba_id
        || q.value(9).toString() != kpis[i].kpi_ba_name
        || q.value(10).toInt() != kpis[i].meta_service_id
        || q.value(11).toString() != kpis[i].meta_service_name
        || !double_equals(q.value(12).toDouble(), kpis[i].impact_warning)
        || !double_equals(q.value(13).toDouble(), kpis[i].impact_critical)
        || !double_equals(q.value(14).toDouble(), kpis[i].impact_unknown)
        || q.value(15).toInt() != kpis[i].boolean_id
        || q.value(16).toString() != kpis[i].boolean_name)
      throw (exceptions::msg() << "invalid KPIs " << q.value(0).toUInt()
             << " at iteration " << iteration << ": got (kpi name "
             << q.value(1).toString() << ", ba id "
             << q.value(2).toInt() << ", ba_name "
             << q.value(3).toString() << ", host_id "
             << q.value(4).toInt() << ", host_name "
             << q.value(5).toString() << ", service_id "
             << q.value(6).toInt() << ", service_description "
             << q.value(7).toString() << ", kpi_ba_id "
             << q.value(8).toInt() << ", kpi_ba_name "
             << q.value(9).toString() << ", meta_service_id "
             << q.value(10).toInt() << ", meta_service_name "
             << q.value(11).toString() << ", impact_warning "
             << q.value(12).toDouble() << ", impact_critical "
             << q.value(13).toDouble() << ", impact_unknown "
             << q.value(14).toDouble() << ", boolean_id "
             << q.value(15).toInt() << ", boolean_name "
             << q.value(16).toString() << ") expected ("
             << kpis[i].kpi_name << ", " << kpis[i].ba_id << ", "
             << kpis[i].ba_name << ", " << kpis[i].host_id << ", "
             << kpis[i].host_name << ", " << kpis[i].service_id << ", "
             << kpis[i].service_description << ", " << kpis[i].kpi_ba_id << ", "
             << kpis[i].kpi_ba_name << ", " << kpis[i].meta_service_id << ", "
             << kpis[i].meta_service_name << ", " << kpis[i].impact_warning << ", "
             << kpis[i].impact_critical << ", " << kpis[i].impact_unknown << ", "
             << kpis[i].boolean_id << ", " << kpis[i].boolean_name << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much KPIs at iteration "
           << iteration << ": expected " << count);
  return ;
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
    db.open(NULL, BI_DB_NAME, CENTREON_DB_NAME);

    // Create the config bam xml file.
    test_file file;
    file.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/bam.xml.in");
    file.set("DB_NAME_CENTREON", CENTREON_DB_NAME);
    file.set("DB_NAME_BI", BI_DB_NAME);
    file.set("COMMAND_FILE", COMMAND_FILE);
    std::string config_file = file.generate();

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    generate_services(services, hosts, SERVICES_BY_HOST);

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
             j < limit;
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
      QString query(
                "INSERT INTO mod_bam (ba_id, name, description,"
                "                     sla_month_percent_warn, sla_month_percent_crit,"
                "                     sla_month_duration_warn, sla_month_duration_crit)"
                "  VALUES (1, 'BA1', 'DESC1', 90, 80, 70, 60),"
                "         (2, 'BA2', 'DESC2', 80, 70, 60, 50)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create BAs: "
               << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query(
                "INSERT INTO mod_bam_kpi (kpi_id, kpi_type, host_id,"
                "            service_id, id_indicator_ba, id_ba,"
                "            meta_id, boolean_id, config_type, drop_warning,"
                "            drop_warning_impact_id, drop_critical,"
                "            drop_critical_impact_id, drop_unknown,"
                "            drop_unknown_impact_id, ignore_downtime,"
                "            ignore_acknowledged)"
                "  VALUES (1, '0', 1, 1, NULL, 1, NULL, NULL, '0', 15, NULL, 25, NULL, 99, NULL, '0', '0'),"
                "         (2, '0', 1, 2, NULL, 2, NULL, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (3, '1', NULL, NULL, 1, 2, NULL, NULL, '0', 65, NULL, 75, NULL, 99, NULL, '0', '0'),"
                "         (4, '1', NULL, NULL, 2, 1, NULL, NULL, '0', 25, NULL, 35, NULL, 99, NULL, '0', '0'),"
                "         (5, '2', NULL, NULL, NULL, 1, 1, NULL, '0', 35, NULL, 45, NULL, 99, NULL, '0', '0'),"
                "         (6, '2', NULL, NULL, NULL, 2, 2, NULL, '0', 45, NULL, 55, NULL, 99, NULL, '0', '0'),"
                "         (7, '3', NULL, NULL, NULL, 1, NULL, 1, '0', 85, NULL, 95, NULL, 99, NULL, '0', '0'),"
                "         (8, '3', NULL, NULL, NULL, 2, NULL, 2, '0', 95, NULL, 105, NULL, 99, NULL, '0', '0')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create KPIs: "
               << q.lastError().text());
    }

    // Create boolean expressions.
    {
      QString query(
                "INSERT INTO mod_bam_boolean (boolean_id, name,"
                "            config_type, impact, impact_id,"
                "            expression, bool_state)"
                "  VALUES (1, 'BoolExp1', 0, 75, NULL, '{1 1} {is} {OK}', 0),"
                "         (2, 'BoolExp2', 0, 25, NULL, '{1 2} {not} {CRITICAL} {OR} {1 3} {not} {OK}', 1)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create boolexps: "
               << q.lastError().text());
    }
    // Create meta_services
    {
      QString query(
                "INSERT INTO meta_service (meta_id, meta_name)"
                "  VALUES (1, 'Meta1'),"
                "         (2, 'Meta2')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create the meta services: "
                                 << q.lastError().text());
    }
    // Create bvs
    {
      QString query(
                "INSERT INTO mod_bam_ba_groups (id_ba_group, ba_group_name,"
                "                               ba_group_description)"
                "  VALUES (1, 'BaGroup1', 'BaGroupDescription1'),"
                "         (2, 'BaGroup2', 'BaGroupDescription2')");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create the bvs: "
                                 << q.lastError().text());
    }
    // Create the ba bv relations
    {
      QString query(
                "INSERT INTO mod_bam_bagroup_ba_relation (id_bgr, id_ba, "
                "                                         id_ba_group)"
                "  VALUES (1, 2, 1),"
                "         (2, 1, 2)");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not create the ba bv relations: "
                                 << q.lastError().text());
    }

    // Start Broker daemon.
    broker.set_config_file(config_file);
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Let the broker do its thing.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that the dimensions were correctly copied.

    // Check bas
    ba_dimension bas[] =
    {{1, "BA1", "DESC1", 90, 80, 70, 60},
     {2, "BA2", "DESC2", 80, 70, 60, 50}};
    check_bas(*db.bi_db(), bas, sizeof(bas) / sizeof(*bas));

    // Check bvs
    bv_dimension bvs[] =
    {{1, "BaGroup1", "BaGroupDescription1"},
     {2, "BaGroup2", "BaGroupDescription2"}};
    check_bvs(*db.bi_db(), bvs, sizeof(bvs) / sizeof(*bvs));

    // Check ba-bv links.
    ba_bv_dimension babvs[] =
    {{1, 2},
     {2, 1}};
    check_ba_bv_links(*db.bi_db(), babvs, sizeof(babvs) / sizeof(*babvs));

    // Check kpis.
    kpi_dimension kpis[] =
    {
      // Host/Service kpis.
      {1, "1", 1, "BA1", 1, "1", 1, "1", 0, NULL, 0, NULL, 15, 25, 99, 0, NULL},
      {2, "2", 2, "BA2", 1, "1", 2, "2", 0, NULL, 0, NULL, 35, 45, 99, 0, NULL},
      // Ba kpis.
      {3, "BA1", 2, "BA2", 0, NULL, 0, NULL, 1, "BA1", 0, NULL, 65, 75, 99, 0, NULL},
      {4, "BA2", 1, "BA1", 0, NULL, 0, NULL, 2, "BA2", 0, NULL, 25, 35, 99, 0, NULL},
      // Meta service kpis.
      {5, "Meta1", 1, "BA1", 0, NULL, 0, NULL, 0, NULL, 1, "Meta1", 35, 45, 99, 0, NULL},
      {6, "Meta2", 2, "BA2", 0, NULL, 0, NULL, 0, NULL, 2, "Meta2", 45, 55, 99, 0, NULL},
      // Boolean kpis.
      {7, "BoolExp1", 1, "BA1", 0, NULL, 0, NULL, 0, NULL, 0, NULL, 85, 95, 99, 1, "BoolExp1"},
      {8, "BoolExp2", 2, "BA2", 0, NULL, 0, NULL, 0, NULL, 0, NULL, 95, 105, 99, 2, "BoolExp2"},
    };
    check_kpis(*db.bi_db(), kpis, sizeof(kpis) / sizeof(*kpis));

    // Erase everything.
    {
      QString query("DELETE FROM mod_bam");
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw (exceptions::msg() << "could not truncate the table mod_bam: "
                                 << q.lastError().text());
      query = "DELETE FROM mod_bam_kpi";
      if (!q.exec(query))
        throw (exceptions::msg() << "could not truncate the table mod_bam_kpi: "
                                 << q.lastError().text());
      query = "DELETE FROM mod_bam_ba_groups";
      if (!q.exec(query))
        throw (exceptions::msg() << "could not truncate the table mod_bam_ba_groups: "
                                 << q.lastError().text());
      query = "DELETE FROM mod_bam_bagroup_ba_relation";
      if (!q.exec(query))
        throw (exceptions::msg() << "could not truncate the table mod_bam_bagroup_ba_relation: "
                                 << q.lastError().text());
    }

    // Update the broker.
    broker.update();
    // Let the broker do its thing.
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check that everything was deleted.
    {
      QString query("SELECT * from mod_bam_reporting_ba");
      QSqlQuery q(*db.bi_db());
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not select the table mod_bam_reporting_ba: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg()
               << "the table mod_bam_reporting_ba wasn't updated: "
               << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_bv";
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not select the table mod_bam_reporting_bv: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg()
               << "the table mod_bam_reporting_bv wasn't updated: "
               << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_relations_ba_bv";
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not select the table mod_bam_reporting_relations_ba_bv: "
               << q.lastError().text());
      if (q.size() != 0)
        throw  (exceptions::msg()
                << "the table mod_bam_reporting_relations_ba_bv wasn't updated: "
                << q.lastError().text());

      query = "SELECT * from mod_bam_reporting_kpi";
      if (!q.exec(query))
        throw (exceptions::msg()
               << "could not select the table mod_bam_reporting_kpi: "
               << q.lastError().text());
      if (q.size() != 0)
        throw (exceptions::msg()
               << "the table mod_bam_reporting_kpi wasn't updated: "
               << q.lastError().text());
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
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
