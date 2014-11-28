/*
** Copyright 2012-2014 Merethis
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
#include <ctime>
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

using namespace com::centreon::broker;

#define DB_NAME "broker_comments_to_sql"

/**
 *  Check that comments are properly inserted in SQL database.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::string engine_config_path(tmpnam(NULL));
  external_command commander;
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, 10);
    generate_services(services, hosts, 5);
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR << "/test/cfg/comments_to_sql.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();

    // Let the daemon initialize.
    sleep_for(14 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Add comments on two hosts.
    commander.execute(
                "ADD_HOST_COMMENT;9;1;Merethis;Centreon");
    commander.execute(
                "ADD_HOST_COMMENT;4;0;RandomAuthor;Some random comment.");

    // Add comments on two services.
    commander.execute(
                "ADD_SVC_COMMENT;3;13;1;Broker;Another comment !");
    commander.execute(
                "ADD_SVC_COMMENT;1;1;0;FooBar;Baz    Qux");

    // Let the monitoring engine run a while.
    sleep_for(12 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Base time.
    time_t now(time(NULL));

    // Check comments.
    {
      struct {
        QVariant internal_id;
        QVariant host_id;
        QVariant service_id;
        QVariant author;
        QVariant data;
        QVariant persistent;
      } entries[] = {
        { QVariant(1u), QVariant(9u), QVariant(QVariant::UInt),
          QVariant("Merethis"), QVariant("Centreon"), QVariant(1) },
        { QVariant(2u), QVariant(4u), QVariant(QVariant::UInt),
          QVariant("RandomAuthor"), QVariant("Some random comment."),
          QVariant(0) },
        { QVariant(3u), QVariant(3u), QVariant(13u), QVariant("Broker"),
          QVariant("Another comment !"), QVariant(1) },
        { QVariant(4u), QVariant(1u), QVariant(1u), QVariant("FooBar"),
          QVariant("Baz    Qux"), QVariant(0) }
      };
      std::ostringstream query;
      query << "SELECT internal_id, host_id, service_id, author, data, persistent, entry_time, deletion_time"
            << "  FROM comments"
            << "  ORDER BY internal_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg() << "cannot get comments from DB: "
               << q.lastError().text().toStdString().c_str());

      // Check entries.
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough comments in DB (check #1): got " << i
                 << ", expected "
                 << sizeof(entries) / sizeof(*entries));
        if ((q.value(0) != entries[i].internal_id)
            || (q.value(1) != entries[i].host_id)
            || (q.value(2) != entries[i].service_id)
            || (q.value(3) != entries[i].author)
            || (q.value(4) != entries[i].data)
            || (static_cast<bool>(q.value(5).toInt())
                != static_cast<bool>(entries[i].persistent.toInt()))
            || (static_cast<time_t>(q.value(6).toLongLong())
                < (now - 12 * MONITORING_ENGINE_INTERVAL_LENGTH - 5))
            || (static_cast<time_t>(q.value(6).toLongLong()) > now)
            || !q.value(7).isNull())
          throw (exceptions::msg() << "invalid comment " << i
                 << ": got (internal id "
                 << q.value(0).toUInt() << ", host_id "
                 << q.value(1).toUInt() << ", service_id "
                 << q.value(2).toUInt() << ", author "
                 << qPrintable(q.value(3).toString()) << ", data "
                 << qPrintable(q.value(4).toString()) << ", persistent "
                 << q.value(5).toInt() << ", entry time "
                 << q.value(6).toLongLong() << ", deletion time "
                 << q.value(7).isNull() << "), expected ("
                 << entries[i].internal_id.toUInt() << ", "
                 << entries[i].host_id.toUInt() << ", "
                 << entries[i].service_id.toUInt() << ", "
                 << qPrintable(entries[i].author.toString()) << ", "
                 << qPrintable(entries[i].data.toString()) << ", "
                 << (now - 12 * MONITORING_ENGINE_INTERVAL_LENGTH - 5)
                 << ":" << now << ", true)");
      }
      if (q.next())
        throw (exceptions::msg() << "too much comments in DB");
    }

    // Remove two comments.
    commander.execute("DEL_HOST_COMMENT;1");
    commander.execute("DEL_SVC_COMMENT;4");
    sleep_for(7 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Restart daemon.
    daemon.stop();
    daemon.start();
    sleep_for(7 * MONITORING_ENGINE_INTERVAL_LENGTH);
    now = time(NULL);

    // Check that comments where deleted.
    {
      struct {
        unsigned int internal_id;
        bool         deletion_time_is_null;
        time_t       deletion_time_low;
        time_t       deletion_time_high;
      } entries[] = {
        { 1u, false, now - 15 * MONITORING_ENGINE_INTERVAL_LENGTH,
          now - 7 * MONITORING_ENGINE_INTERVAL_LENGTH },
        { 2u, false, now - 8 * MONITORING_ENGINE_INTERVAL_LENGTH, now },
        { 3u, true, 0, 0 },
        { 4u, false, now - 15 * MONITORING_ENGINE_INTERVAL_LENGTH,
          now - 7 * MONITORING_ENGINE_INTERVAL_LENGTH }
      };
      std::ostringstream query;
      query << "SELECT internal_id, deletion_time"
            << "  FROM comments"
            << "  ORDER BY internal_id";
      QSqlQuery q(*db.storage_db());
      if (!q.exec(query.str().c_str()))
        throw (exceptions::msg()
               << "cannot get deletion time of comments from DB: "
               << q.lastError().text().toStdString().c_str());

      // Check comments.
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough comments in DB (check #2): got " << i
                 << ", expected "
                 << sizeof(entries) / sizeof(*entries));
        bool entry_deletion_time_is_null(q.value(1).isNull());
        if ((q.value(0).toUInt() != entries[i].internal_id)
            || (entry_deletion_time_is_null
                != entries[i].deletion_time_is_null)
            || (entry_deletion_time_is_null
                && ((static_cast<time_t>(q.value(1).toLongLong())
                     < entries[i].deletion_time_low)
                    || (static_cast<time_t>(q.value(1).toLongLong())
                        > entries[i].deletion_time_high))))
          throw (exceptions::msg() << "invalid comment " << i
                 << ": got (internal id " << q.value(0).toUInt()
                 << ", null deletion time "
                 << entry_deletion_time_is_null << ", deletion time "
                 << q.value(1).toLongLong() << "), expected ("
                 << entries[i].internal_id << ", "
                 << entries[i].deletion_time_is_null << ", "
                 << entries[i].deletion_time_low << ":"
                 << entries[i].deletion_time_high << ")");
      }
    }

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  daemon.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  return (retval);
}
