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
#include <QString>
#include <QVariant>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_bam_correlation"
#define HOST_COUNT 1
#define SERVICES_BY_HOST 8

/**
 *  Check that correlation work with BAs.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  engine monitoring;
  engine_extcmd commander;
  test_db db;
  test_file cbmod_cfg;
  cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/bam_correlation.xml.in");
  cbmod_cfg.set("DB_NAME", DB_NAME);

  try {
    // Prepare database.
    db.open(DB_NAME, NULL, true);

    // Create organization.
    {
      QString query;
      query =
          "INSERT INTO cfg_organizations "
          "            (organization_id, name, shortname)"
          "  VALUES (42, '42', '42')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create organization: " << q.lastError().text());
    }

    // Create hosts.
    {
      QString query;
      query =
          "INSERT INTO cfg_hosts"
          "            (host_id, host_name, organization_id)"
          "  VALUES (1, '1', 42), (1001, '_Module_BAM', 42)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create hosts: " << q.lastError().text());
    }

    // Create services. Virtual services are needed to get the same ID
    // between DB and correlation file.
    {
      QString query;
      query =
          "INSERT INTO cfg_services"
          "            (service_id, service_description, organization_id)"
          "  VALUES (1, '1', 42),"
          "         (2, '2', 42),"
          "         (3, '3', 42),"
          "         (4, '4', 42),"
          "         (5, '5', 42),"
          "         (6, '6', 42),"
          "         (7, '7', 42),"
          "         (8, '8', 42),"
          "         (1001, 'ba_1', 42),"
          "         (1002, 'ba_2', 42),"
          "         (1003, 'ba_3', 42),"
          "         (1004, 'ba_4', 42)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create services: " << q.lastError().text());
    }
    {
      QString query;
      query =
          "INSERT INTO cfg_hosts_services_relations"
          "            (host_host_id, service_service_id)"
          "  VALUES (1, 1),"
          "         (1, 2),"
          "         (1, 3),"
          "         (1, 4),"
          "         (1, 5),"
          "         (1, 6),"
          "         (1, 7),"
          "         (1, 8),"
          "         (1001, 1001),"
          "         (1001, 1002),"
          "         (1001, 1003),"
          "         (1001, 1004)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not link services to hosts: " << q.lastError().text());
    }

    // Create BAs.
    {
      QString query;
      query =
          "INSERT INTO cfg_bam_ba_types (ba_type_id, name, slug,"
          "            description)"
          "  VALUES (1, 'Default', 'default', 'Default type')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BA types: " << q.lastError().text());
    }
    {
      QString query;
      query =
          "INSERT INTO cfg_bam (ba_id, name, level_w, level_c,"
          "            activate, ba_type_id, organization_id)"
          "  VALUES (1, 'BA1', 25, 75, 1, 1, 42),"
          "         (2, 'BA2', 25, 75, 1, 1, 42),"
          "         (3, 'BA3', 25, 75, 1, 1, 42),"
          "         (4, 'BA4', 25, 75, 1, 1, 42)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create BAs: " << q.lastError().text());
    }
    {
      QString query;
      query =
          "INSERT INTO cfg_bam_poller_relations (ba_id, poller_id)"
          "  VALUES (1, 42), (2, 42), (3, 42), (4, 42)";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg() << "could not create BA/poller relations: "
                                << q.lastError().text());
    }
    {
      QString query;
      query =
          "INSERT INTO rt_instances (instance_id, name)"
          "  VALUES (42, 'MyBroker')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create RT instance: " << q.lastError().text());
    }

    // Create KPIs.
    {
      QString query;
      query =
          "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
          "            service_id, id_ba, config_type,"
          "            drop_warning, drop_critical, drop_unknown,"
          "            state_type, activate)"
          "  VALUES (1, '0', 1, 1, 1, '0', 50, 50, 50, '1', '1'),"
          "         (2, '0', 1, 2, 1, '0', 50, 50, 50, '1', '1'),"
          "         (3, '0', 1, 3, 2, '0', 50, 50, 50, '1', '1'),"
          "         (4, '0', 1, 4, 2, '0', 50, 50, 50, '1', '1'),"
          "         (5, '0', 1, 5, 3, '0', 50, 50, 50, '1', '1'),"
          "         (6, '0', 1, 6, 3, '0', 50, 50, 50, '1', '1'),"
          "         (7, '0', 1, 7, 4, '0', 50, 50, 50, '1', '1'),"
          "         (8, '0', 1, 8, 4, '0', 50, 50, 50, '1', '1')";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "could not create KPIs: " << q.lastError().text());
    }

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
         it != end; ++it) {
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it) {
      it->accept_passive_service_checks = 1;
      it->checks_enabled = 0;
      it->max_attempts = 1;
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << cbmod_cfg.generate() << "\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(3);

    // Set all services to OK.
    for (int i(0); i < SERVICES_BY_HOST; ++i) {
      std::ostringstream oss;
      oss << "PROCESS_SERVICE_CHECK_RESULT;1;" << i + 1 << ";0;OK";
      commander.execute(oss.str());
    }
    sleep_for(5);

    //
    // RULES
    //
    // - BAs are impacted by two services for 50 each.
    // - One service not OK => BA is WARNING.
    // - Two services not OK => BA is CRITICAL.
    // - BA1 depends on BA2.
    // - BA2 has for parents BA3 and BA4.
    //
    // ACTIONS
    //
    // 1) S1 CRITICAL => BA1 WARNING  => new issue
    // 2) S2 CRITICAL => BA1 CRITICAL => nothing
    // 3) S3 CRITICAL => BA2 WARNING  => new issue linked to BA1 issue
    // 4) S5 CRITICAL => BA3 WARNING  => new issue
    // 5) S6 CRITICAL => BA3 CRITICAL => nothing
    // 6) S7 CRITICAL => BA4 WARNING  => new issue
    //                                   BA2 issue gets linked to BA3 and BA4
    //                                   issues
    //

    // Step #1.
    time_t t1(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;CRITICAL S1");
    sleep_for(3);

    // Step #2.
    time_t t2(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;2;2;CRITICAL S2");
    sleep_for(3);

    // Step #3.
    time_t t3(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;3;2;CRITICAL S3");
    sleep_for(3);

    // Step #4.
    time_t t4(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;5;2;CRITICAL S5");
    sleep_for(3);

    // Step #5.
    time_t t5(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;6;2;CRITICAL S6");
    sleep_for(3);

    // Step #6.
    time_t t6(time(NULL));
    commander.execute("PROCESS_SERVICE_CHECK_RESULT;1;7;2;CRITICAL S7");
    sleep_for(3);
    time_t t7(time(NULL));

    // Check issues.
    {
      struct {
        unsigned int service_id;
        time_t start_time_low;
        time_t start_time_high;
      } expected[] = {
          {1001, t1, t2}, {1002, t3, t4}, {1003, t4, t5}, {1004, t6, t7}};

      QString query;
      query =
          "SELECT host_id, service_id, start_time, end_time, ack_time"
          "  FROM rt_issues";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "cannot get issues from DB: " << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i >= sizeof(expected) / sizeof(*expected))
          throw(exceptions::msg()
                << "too much issues, got " << i << ", expected "
                << sizeof(expected) / sizeof(*expected));
        if ((q.value(0).toUInt() != 1001) ||
            (q.value(1).toUInt() != expected[i].service_id) ||
            (q.value(2).toLongLong() < expected[i].start_time_low) ||
            (q.value(2).toLongLong() > expected[i].start_time_high) ||
            !q.value(3).isNull() || !q.value(4).isNull())
          throw(exceptions::msg()
                << "invalid issue: got (host " << q.value(0).toUInt()
                << ", service " << q.value(1).toUInt() << ", start time "
                << q.value(2).toLongLong() << ", end time "
                << (q.value(3).isNull() ? "NULL" : "non-NULL") << ", ack time "
                << (q.value(4).isNull() ? "NULL" : "non-NULL")
                << "), expected (1001, " << expected[i].service_id << ", "
                << expected[i].start_time_low << ":"
                << expected[i].start_time_high << ", NULL, NULL)");
        ++i;
      }
      if (i != sizeof(expected) / sizeof(*expected))
        throw(exceptions::msg() << "got " << i << " issues, expected "
                                << sizeof(expected) / sizeof(*expected));
    }

    // Check issues parents.
    {
      struct {
        time_t start_time_low;
        time_t start_time_high;
        unsigned int child_service_id;
        unsigned int parent_service_id;
      } expected[] = {
          {t3, t4, 1001, 1002}, {t6, t7, 1002, 1003}, {t6, t7, 1002, 1004}};
      QString query;
      query =
          "SELECT ip.start_time, ip.end_time, i1.host_id,"
          "       i1.service_id, i2.host_id, i2.service_id"
          "  FROM rt_issues_issues_parents AS ip"
          "  LEFT JOIN rt_issues AS i1"
          "    ON ip.child_id = i1.issue_id"
          "  LEFT JOIN rt_issues AS i2"
          "    ON ip.parent_id = i2.issue_id"
          "  ORDER BY i1.service_id ASC, i2.service_id ASC";
      QSqlQuery q(*db.centreon_db());
      if (!q.exec(query))
        throw(exceptions::msg()
              << "cannot get issues parents from DB: " << q.lastError().text());
      size_t i(0);
      while (q.next()) {
        if (i >= sizeof(expected) / sizeof(*expected))
          throw(exceptions::msg()
                << "too much issues parents, got " << i << ", expected "
                << sizeof(expected) / sizeof(*expected));
        if ((q.value(0).toLongLong() < expected[i].start_time_low) ||
            (q.value(0).toLongLong() > expected[i].start_time_high) ||
            !q.value(1).isNull() || (q.value(2).toUInt() != 1001) ||
            (q.value(3).toUInt() != expected[i].child_service_id) ||
            (q.value(4).toUInt() != 1001) ||
            (q.value(5).toUInt() != expected[i].parent_service_id))
          throw(exceptions::msg()
                << "invalid issue parent: got (start time "
                << q.value(0).toLongLong() << ", end time "
                << (q.value(1).isNull() ? "NULL" : "non-NULL")
                << ", child host " << q.value(2).toUInt() << ", child service "
                << q.value(3).toUInt() << ", parent host "
                << q.value(4).toUInt() << ", parent service "
                << q.value(5).toUInt() << "), expected ("
                << expected[i].start_time_low << ":"
                << expected[i].start_time_high << ", NULL, 1001, "
                << expected[i].child_service_id << ", 1001, "
                << expected[i].parent_service_id << ")");
        ++i;
      }
      if (i != sizeof(expected) / sizeof(*expected))
        throw(exceptions::msg() << "got " << i << " issues parents, expected "
                                << sizeof(expected) / sizeof(*expected));
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

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
