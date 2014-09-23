/*
** Copyright 2013 Merethis
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
#include <cstring>
#include <ctime>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_event_handlers_to_sql"
#define HOST_COUNT 2
#define SERVICES_BY_HOST 2

/**
 *  Check that event handlers are properly inserted in the database.
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
  engine monitoring;
  cbd broker;

  try {
    // Prepare database.
    QSqlDatabase db(config_db_open(DB_NAME));

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 4);
    {
      // The two first command will return UNREACHABLE/CRITICAL for
      // host (#1) and service (#2).
      std::list<command>::iterator it(commands.begin());
      char const* cmdline(MY_PLUGIN_PATH " 1 ThisIsTheHostPluginOutput");
      it->command_line = new char[strlen(cmdline) + 1];
      strcpy(it->command_line, cmdline);
      ++it;
      cmdline = MY_PLUGIN_PATH " 2 ThisIsTheServicePluginOutput";
      it->command_line = new char[strlen(cmdline) + 1];
      strcpy(it->command_line, cmdline);
      // Create two commands that will be the host (command #3) and
      // service (command #4) event handlers.
      ++it;
      cmdline = MY_PLUGIN_PATH " 0 ThisIsTheHostEventHandlerOutput";
      it->command_line = new char[strlen(cmdline) + 1];
      strcpy(it->command_line, cmdline);
      ++it;
      cmdline = MY_PLUGIN_PATH " 0 ThisIsTheServiceEventHandlerOutput";
      it->command_line = new char[strlen(cmdline) + 1];
      strcpy(it->command_line, cmdline);
    }
    generate_hosts(hosts, HOST_COUNT);
    {
      host& h(*(++hosts.begin()));
      h.checks_enabled = 0;
      h.accept_passive_host_checks = 0;
      h.max_attempts = 3;
      h.host_check_command = new char[2];
      strcpy(h.host_check_command, "1");
      h.event_handler = new char[2];
      strcpy(h.event_handler, "3");
      h.event_handler_enabled = 1;
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      service& s(services.front());
      s.checks_enabled = 0;
      s.accept_passive_service_checks = 0;
      s.max_attempts = 3;
      s.service_check_command = new char[2];
      strcpy(s.service_check_command, "2");
      s.event_handler = new char[2];
      strcpy(s.event_handler, "4");
      s.event_handler_enabled = 1;
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << "enable_event_handlers=1\n"
          << "event_handler_timeout=42\n"
          << "use_aggressive_host_checking=1\n"
          << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " " << PROJECT_SOURCE_DIR
          << "/test/cfg/event_handlers_to_sql_1.xml\n";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      &commands);

    // Start Broker daemon.
    broker.set_config_file(
      PROJECT_SOURCE_DIR "/test/cfg/event_handlers_to_sql_2.xml");
    broker.start();
    sleep_for(2 * MONITORING_ENGINE_INTERVAL_LENGTH);
    broker.update();

    // Start engine.
    {
      std::string engine_config_file(engine_config_path);
      engine_config_file.append("/nagios.cfg");
      monitoring.set_config_file(engine_config_file);
      monitoring.start();
    }

    // T1.
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);
    time_t t1(time(NULL));

    // Host SOFT state #1.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_HOST_CHECK;2;" << time(NULL);
      commander.execute(oss.str());
    }

    // Service SOFT state #1.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_SVC_CHECK;1;1;" << time(NULL);
      commander.execute(oss.str());
    }

    // T2.
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);
    time_t t2(time(NULL));

    // Host SOFT states #2.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_HOST_CHECK;2;" << time(NULL);
      commander.execute(oss.str());
    }

    // Service SOFT states #2.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_SVC_CHECK;1;1;" << time(NULL);
      commander.execute(oss.str());
    }

    // T3.
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);
    time_t t3(time(NULL));

    // Host HARD state.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_HOST_CHECK;2;" << time(NULL);
      commander.execute(oss.str());
    }

    // Service HARD state.
    {
      std::ostringstream oss;
      oss << "SCHEDULE_FORCED_SVC_CHECK;1;1;" << time(NULL);
      commander.execute(oss.str());
    }

    // T4.
    sleep_for(4 * MONITORING_ENGINE_INTERVAL_LENGTH);
    time_t t4(time(NULL));

    // Check host eventhandlers entries.
    {
      struct {
        unsigned long host_id;
        unsigned long service_id;
        time_t start_time_low;
        time_t start_time_high;
        std::string command_args;
        std::string command_line;
        bool early_timeout;
        time_t end_time_low;
        time_t end_time_high;
        double execution_time_low;
        double execution_time_high;
        std::string output;
        int return_code;
        short state;
        short state_type;
        time_t timeout;
        short type;
      } const entries[] = {
        { 2, 0, t1, t2, "",
          MY_PLUGIN_PATH " 0 ThisIsTheHostEventHandlerOutput", false,
          t1, t2, 0.0, static_cast<double>(t2 - t1),
          ""/*"ThisIsTheHostEventHandlerOutput"*/, 0, 1, 0, 42, 0 },
        { 2, 0, t2, t3, "",
          MY_PLUGIN_PATH " 0 ThisIsTheHostEventHandlerOutput", false,
          t2, t3, 0.0, static_cast<double>(t3 - t2),
          ""/*"ThisIsTheHostEventHandlerOutput"*/, 0, 1, 0, 42, 0 },
        { 2, 0, t3, t4, "",
          MY_PLUGIN_PATH " 0 ThisIsTheHostEventHandlerOutput", false,
          t3, t4, 0.0, static_cast<double>(t4 - t3),
          ""/*"ThisIsTheHostEventHandlerOutput"*/, 0, 1, 1, 42, 0 },
        { 1, 1, t1, t2, "",
          MY_PLUGIN_PATH " 0 ThisIsTheServiceEventHandlerOutput", false,
          t1, t2, 0.0, static_cast<double>(t2 - t1),
          ""/*"ThisIsTheServiceEventHandlerOutput"*/, 0, 2, 0, 42, 1 },
        { 1, 1, t2, t3, "",
          MY_PLUGIN_PATH " 0 ThisIsTheServiceEventHandlerOutput", false,
          t2, t3, 0.0, static_cast<double>(t3 - t2),
          ""/*"ThisIsTheServiceEventHandlerOutput"*/, 0, 2, 0, 42, 1 },
        { 1, 1, t3, t4, "",
          MY_PLUGIN_PATH " 0 ThisIsTheServiceEventHandlerOutput", false,
          t3, t4, 0.0, static_cast<double>(t4 - t3),
          ""/*"ThisIsTheServiceEventHandlerOutput"*/, 0, 2, 1, 42, 1 }
      };

      std::string query(
        "SELECT host_id, service_id, start_time, command_args,"
        "       command_line, early_timeout, end_time, execution_time,"
        "       output, return_code, state, state_type, timeout, type"
        "  FROM rt_eventhandlers"
        "  ORDER BY host_id DESC, COALESCE(service_id, -1) ASC, start_time ASC");
      QSqlQuery q(db);
      if (!q.exec(query.c_str()))
        throw (exceptions::msg()
               << "cannot get host eventhandlers entries: "
               << qPrintable(q.lastError().text()));
      for (unsigned int i(0);
           i < sizeof(entries) / sizeof(*entries);
           ++i) {
        if (!q.next())
          throw (exceptions::msg()
                 << "not enough host eventhandlers entries: got " << i
                 << ", expected " << sizeof(entries) / sizeof(*entries));
        if ((q.value(0).toULongLong() != entries[i].host_id)
            || (q.value(1).toULongLong() != entries[i].service_id)
            || (q.value(2).toLongLong() < entries[i].start_time_low)
            || (q.value(2).toLongLong() > entries[i].start_time_high)
            || (q.value(3).toString() != entries[i].command_args.c_str())
            || (q.value(4).toString() != entries[i].command_line.c_str())
            || (static_cast<bool>(q.value(5).toInt())
                != entries[i].early_timeout)
            || (q.value(6).toLongLong() < entries[i].end_time_low)
            || (q.value(6).toLongLong() > entries[i].end_time_high)
            || (q.value(7).toDouble() < entries[i].execution_time_low)
            || (q.value(7).toDouble() > entries[i].execution_time_high)
            || (q.value(8).toString() != entries[i].output.c_str())
            || (q.value(9).toInt() != entries[i].return_code)
            || (q.value(10).toInt() != entries[i].state)
            || (q.value(11).toInt() != entries[i].state_type)
            || (q.value(12).toInt() != entries[i].timeout)
            || (q.value(13).toInt() != entries[i].type))
          throw (exceptions::msg() << "invalid eventhandlers entry #"
                 << i << ": got (host id "
                 << q.value(0).toULongLong() << ", service id "
                 << q.value(1).toULongLong() << ", start time "
                 << q.value(2).toLongLong() << ", command args "
                 << q.value(3).toString() << ", command line "
                 << q.value(4).toString() << ", early timeout "
                 << q.value(5).toInt() << ", end time "
                 << q.value(6).toLongLong() << ", execution time "
                 << q.value(7).toDouble() << ", output "
                 << q.value(8).toString() << ", return code "
                 << q.value(9).toInt() << ", state "
                 << q.value(10).toInt() << ", state type "
                 << q.value(11).toInt() << ", timeout "
                 << q.value(12).toInt() << ", type "
                 << q.value(13).toInt() << "), expected ("
                 << entries[i].host_id << ", "
                 << entries[i].service_id << ", "
                 << entries[i].start_time_low << ":"
                 << entries[i].start_time_high << ", "
                 << entries[i].command_args.c_str() << ", "
                 << entries[i].command_line.c_str() << ", "
                 << entries[i].early_timeout << ", "
                 << entries[i].end_time_low << ":"
                 << entries[i].end_time_high << ", "
                 << entries[i].execution_time_low << ":"
                 << entries[i].execution_time_high << ", "
                 << entries[i].output.c_str() << ", "
                 << entries[i].return_code << ", " << entries[i].state
                 << ", " << entries[i].state_type << ", "
                 << entries[i].timeout << ", " << entries[i].type
                 << ")");
      }
      if (q.next())
        throw (exceptions::msg()
               << "too much host eventhandlers entries in table");
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
  monitoring.stop();
  broker.stop();
  config_remove(engine_config_path.c_str());
  config_db_close(DB_NAME);
  free_hosts(hosts);
  free_services(services);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
