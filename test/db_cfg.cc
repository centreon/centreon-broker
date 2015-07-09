/*
** Copyright 2015 Merethis
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/external_command.hh"
#include "test/misc.hh"
#include "test/vars.hh"

#define READ_DB_NAME "broker_db_cfg1"
#define WRITE_DB_NAME "broker_db_cfg2"

using namespace com::centreon::broker;

struct ba_entry {
  unsigned int ba_id;
  std::string name;
  int warning;
  int critical;
};
struct kpi_entry {
  unsigned int kpi_id;
  int kpi_type;
  unsigned int host_id;
  unsigned int service_id;
  unsigned int id_indicator_ba;
  unsigned int id_ba;
  unsigned int meta_id;
  unsigned int boolean_id;
  int drop_warning;
  int drop_critical;
  int drop_unknown;
};

/**
 *  Check BA entries in database.
 *
 *  @param[in] db       Database.
 *  @param[in] entries  Expected entries.
 */
static void check_bas(test_db& db, ba_entry const* entries) {
  QSqlQuery q(*db.centreon_db());
  if (!q.exec(
           "SELECT ba_id, name, level_w, level_c, activate"
           "  FROM cfg_bam"
           "  ORDER BY ba_id ASC"))
    throw (exceptions::msg() << "could not retrieve BAs from DB: "
           << q.lastError().text());
  for (int i(0); entries[i].ba_id; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough BA entries in DB");
    if ((q.value(0).toUInt() != entries[i].ba_id)
        || (q.value(1).toString().toStdString() != entries[i].name)
        || (q.value(2).toInt() != entries[i].warning)
        || (q.value(3).toInt() != entries[i].critical)
        || !q.value(4).toBool())
      throw (exceptions::msg() << "invalid BA entry: got (BA ID "
             << q.value(0).toUInt() << ", name '"
             << q.value(1).toString() << "', warning "
             << q.value(2).toInt() << ", critical "
             << q.value(3).toInt() << "), expected ("
             << entries[i].ba_id << ", '" << entries[i].name << "', "
             << entries[i].warning << ", " << entries[i].critical
             << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much BA entries in DB");
  return ;
}

/**
 *  Check KPI entries in database.
 *
 *  @param[in] db       Database.
 *  @param[in] entries  Expected entries.
 */
static void check_kpis(test_db& db, kpi_entry const* entries) {
  QSqlQuery q(*db.centreon_db());
  if (!q.exec(
           "SELECT kpi_id, kpi_type, host_id, service_id,"
           "       id_indicator_ba, id_ba, meta_id, boolean_id,"
           "       drop_warning, drop_critical, drop_unknown,"
           "       activate"
           "  FROM cfg_bam_kpi"
           "  ORDER BY kpi_id ASC"))
    throw (exceptions::msg() << "could not retrieve KPIs from DB: "
           << q.lastError().text());
  for (int i(0); entries[i].kpi_id; ++i) {
    if (!q.next())
      throw (exceptions::msg() << "not enough KPI entries in DB");
    if ((q.value(0).toUInt() != entries[i].kpi_id)
        || (q.value(1).toInt() != entries[i].kpi_type)
        || (q.value(2).toUInt() != entries[i].host_id)
        || (q.value(3).toUInt() != entries[i].service_id)
        || (q.value(4).toUInt() != entries[i].id_indicator_ba)
        || (q.value(5).toUInt() != entries[i].id_ba)
        || (q.value(6).toUInt() != entries[i].meta_id)
        || (q.value(7).toUInt() != entries[i].boolean_id)
        || (q.value(8).toInt() != entries[i].drop_warning)
        || (q.value(9).toInt() != entries[i].drop_critical)
        || (q.value(10).toInt() != entries[i].drop_unknown)
        || !q.value(11).toBool())
      throw (exceptions::msg() << "invalid KPI entry: got (KPI ID "
             << q.value(0).toUInt() << ", type " << q.value(1).toInt()
             << ", host ID " << q.value(2).toUInt() << ", service ID "
             << q.value(3).toUInt() << ", ID indicator BA "
             << q.value(4).toUInt() << ", ID BA " << q.value(5).toUInt()
             << ", meta ID " << q.value(6).toUInt() << ", boolean ID "
             << q.value(7).toUInt() << ", drop warning "
             << q.value(8).toInt() << ", drop critical "
             << q.value(9).toInt() << ", drop unknown "
             << q.value(10).toInt() << "), expected ("
             << entries[i].kpi_id << ", " << entries[i].kpi_type
             << ", " << entries[i].host_id << ", "
             << entries[i].service_id << ", "
             << entries[i].id_indicator_ba << ", " << entries[i].id_ba
             << ", " << entries[i].meta_id << ", "
             << entries[i].boolean_id << ", " << entries[i].drop_warning
             << ", " << entries[i].drop_critical << ", "
             << entries[i].drop_unknown << ")");
  }
  if (q.next())
    throw (exceptions::msg() << "too much KPI entries in DB");
  return ;
}

/**
 *  Check that DB configuration synchronisation work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  cbd db_reader;
  cbd db_writer;
  test_db db_to_read;
  test_db db_to_write;
  external_command commander;

  try {
    // Prepare databases.
    db_to_read.open(READ_DB_NAME, NULL, true);
    db_to_write.open(WRITE_DB_NAME, NULL, true);

    // Populate database.
    {
      char const* queries[] = {
        "INSERT INTO cfg_bam (ba_id, name, level_w, level_c, activate)"
        "  VALUES (1, 'BA1', 90, 80, '1'),"
        "         (2, 'BA2', 80, 70, '1'),"
        "         (3, 'BA3', 70, 60, '1'),"
        "         (4, 'BA4', 60, 50, '1'),"
        "         (5, 'BA5', 50, 40, '1'),"
        "         (6, 'BA6', 40, 30, '1'),"
        "         (7, 'BA7', 30, 20, '1'),"
        "         (8, 'BA8', 20, 10, '1'),"
        "         (9, 'BA9', 10, 0, '1')",
        "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
        "            service_id, id_indicator_ba, id_ba, config_type,"
        "            drop_warning, drop_critical, drop_unknown,"
        "            activate)"
        "  VALUES (1, '0', 3, 2, NULL, 1, '0', 10, 20, 30, '1'),"
        "         (2, '0', 4, 3, NULL, 2, '0', 20, 30, 40, '1'),"
        "         (3, '0', 5, 4, NULL, 3, '0', 30, 40, 50, '1'),"
        "         (4, '0', 6, 5, NULL, 4, '0', 40, 50, 60, '1'),"
        "         (5, '1', NULL, NULL, 6, 5, '0', 50, 60, 70, '1')",
        NULL
      };
      QSqlQuery q(*db_to_read.centreon_db());
      for (int i(0); queries[i]; ++i) {
        if (!q.exec(queries[i]))
          throw (exceptions::msg() << "could not run population query: "
                 << q.lastError().text());
      }
    }

    // Prepare config files.
    commander.set_file(tmpnam(NULL));
    test_file db_reader_cfg;
    db_reader_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/db_cfg_1.xml.in");
    db_reader_cfg.set("DB_NAME", READ_DB_NAME);
    db_reader_cfg.set("COMMAND_FILE", commander.get_file());
    test_file db_writer_cfg;
    db_writer_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/db_cfg_2.xml.in");
    db_writer_cfg.set("DB_NAME", WRITE_DB_NAME);

    // Run Brokers.
    db_writer.set_config_file(db_writer_cfg.generate());
    db_writer.start();
    sleep_for(2);
    db_reader.set_config_file(db_reader_cfg.generate());
    db_reader.start();
    sleep_for(2);

    // Full synchronization.
    commander.execute("SYNC_CFG_DB;DBCfg1-DBReader;43");
    sleep_for(3);

    // Check database.
    {
      ba_entry const bas[] = {
        { 1, "BA1", 90, 80 },
        { 2, "BA2", 80, 70 },
        { 3, "BA3", 70, 60 },
        { 4, "BA4", 60, 50 },
        { 5, "BA5", 50, 40 },
        { 6, "BA6", 40, 30 },
        { 7, "BA7", 30, 20 },
        { 8, "BA8", 20, 10 },
        { 9, "BA9", 10, 0 },
        { 0, "", 0, 0 }
      };
      check_bas(db_to_write, bas);
    }
    {
      kpi_entry const kpis[] = {
        { 1, 0, 3, 2, 0, 1, 0, 0, 10, 20, 30 },
        { 2, 0, 4, 3, 0, 2, 0, 0, 20, 30, 40 },
        { 3, 0, 5, 4, 0, 3, 0, 0, 30, 40, 50 },
        { 4, 0, 6, 5, 0, 4, 0, 0, 40, 50, 60 },
        { 5, 1, 0, 0, 6, 5, 0, 0, 50, 60, 70 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
      };
      check_kpis(db_to_write, kpis);
    }

    // Change database entries.
    {
      char const* queries [] = {
        "INSERT INTO cfg_bam (ba_id, name, level_w, level_c, activate)"
        "  VALUES (10, 'BA10', 75, 50, '1')",
        "INSERT INTO cfg_bam_kpi (kpi_id, kpi_type, host_id,"
        "            service_id, id_indicator_ba, id_ba, config_type,"
        "            drop_warning, drop_critical, drop_unknown,"
        "            activate)"
        "  VALUES (6, '0', 7, 6, NULL, 8, '0', 15, 25, 35, '1'),"
        "         (7, '1', NULL, NULL, 10, 9, '0', 45, 55, 65, '1')",
        "UPDATE cfg_bam SET name='SuperBA', level_w=42 WHERE ba_id=3",
        "UPDATE cfg_bam_kpi SET kpi_type='1', host_id=NULL,"
        "       service_id=NULL, id_indicator_ba=4"
        "  WHERE kpi_id=3",
        "DELETE FROM cfg_bam WHERE ba_id=1 OR ba_id=7",
        "DELETE FROM cfg_bam_kpi WHERE kpi_id=2",
        NULL
      };
      QSqlQuery q(*db_to_read.centreon_db());
      for (int i(0); queries[i]; ++i) {
        if (!q.exec(queries[i]))
          throw (exceptions::msg()
                 << "could not change database entries: "
                 << q.lastError().text());
      }
    }

    // Diff synchronization.
    commander.execute("UPDATE_CFG_DB;DBCfg1-DBReader;43");
    sleep_for(3);

    // Check database.
    {
      ba_entry const bas[] = {
        { 2, "BA2", 80, 70 },
        { 3, "SuperBA", 42, 60 },
        { 4, "BA4", 60, 50 },
        { 5, "BA5", 50, 40 },
        { 6, "BA6", 40, 30 },
        { 8, "BA8", 20, 10 },
        { 9, "BA9", 10, 0 },
        { 10, "BA10", 75, 50 },
        { 0, "", 0, 0 }
      };
      check_bas(db_to_write, bas);
    }
    {
      kpi_entry const kpis[] = {
        { 3, 1, 0, 0, 4, 3, 0, 0, 30, 40, 50 },
        { 4, 0, 6, 5, 0, 4, 0, 0, 40, 50, 60 },
        { 5, 1, 0, 0, 6, 5, 0, 0, 50, 60, 70 },
        { 6, 0, 7, 6, 0, 8, 0, 0, 15, 25, 35 },
        { 7, 1, 0, 0, 10, 9, 0, 0, 45, 55, 65 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
      };
      check_kpis(db_to_write, kpis);
    }

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    db_to_read.set_remove_db_on_close(false);
    db_to_write.set_remove_db_on_close(false);
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    db_to_read.set_remove_db_on_close(false);
    db_to_write.set_remove_db_on_close(false);
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  db_reader.stop();
  db_writer.stop();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
