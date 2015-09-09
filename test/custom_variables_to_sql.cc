/*
** Copyright 2013-2015 Centreon
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
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_custom_variables_to_sql"
#define HOST_COUNT 3
#define SERVICES_BY_HOST 2

/**
 *  Entry struct.
 */
struct entry {
  unsigned int host_id;
  unsigned int service_id;
  char const*  name;
  char const*  value;
  char const*  default_value;
};

/**
 *  Check host custom variables.
 *
 *  @param[in] db           DB object.
 *  @param[in] entries      Expected entries.
 *  @param[in] entries_size Size of entries array.
 *  @param[in] update_low   Minimum update time.
 *  @param[in] update_high  Maximum update time.
 *  @param[in] modified     Modification flag.
 *
 *  @return Return on success, throw on failure.
 */
static void check_host_custom_variables(
              QSqlDatabase& db,
              entry const* entries,
              unsigned int entries_size,
              time_t update_low,
              time_t update_high,
              bool modified) {
  // Execute query.
  char const* query("SELECT host_id, name, default_value, modified,"
                    "       type, update_time, value"
                    "  FROM rt_customvariables"
                    "  WHERE service_id IS NULL"
                    "  ORDER BY host_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg()
           << "cannot get hosts custom variables from DB: "
           << qPrintable(q.lastError().text()));
  for (unsigned int i(0); i < entries_size; ++i) {
    if (!q.next())
      throw (exceptions::msg()
             << "not enough hosts custom variables in DB: got "
             << i << ", expected " << entries_size);
    if ((q.value(0).toUInt() != entries[i].host_id)
        || (q.value(1).toString() != entries[i].name)
        || (q.value(2).toString() != (entries[i].default_value
                                      ? entries[i].default_value
                                      : entries[i].value))
        || (modified && !q.value(3).toUInt())
        || (!modified && q.value(3).toUInt())
        || (q.value(4).toUInt() != 0)
        || (static_cast<time_t>(q.value(5).toUInt()) < update_low)
        || (static_cast<time_t>(q.value(5).toUInt()) > update_high)
        || (q.value(6).toString() != entries[i].value))
      throw (exceptions::msg() << "invalid host custom variable: "
             << "got (host id " << q.value(0).toUInt() << ", name "
             << q.value(1).toString() << ", default value "
             << q.value(2).toString() << ", modified "
             << q.value(3).toUInt() << ", type "
             << q.value(4).toUInt() << ", update time "
             << q.value(5).toUInt() << ", value "
             << q.value(6).toString() << "), expected ("
             << entries[i].host_id << ", " << entries[i].name << ", "
             << (entries[i].default_value
                 ? entries[i].default_value
                 : entries[i].value) << ", " << modified << ", 0, "
             << update_low << ":" << update_high << ", "
             << entries[i].value << ")");
  }
  if (q.next())
    throw (exceptions::msg()
           << "too much host custom variables in DB");
  return ;
}

/**
 *  Check service custom variables.
 *
 *  @param[in] db           DB object.
 *  @param[in] entries      Expected entries.
 *  @param[in] entries_size Size of entries array.
 *  @param[in] update_low   Minimum update time.
 *  @param[in] update_high  Maximum update time.
 *  @param[in] modified     Modification flag.
 *
 *  @return Return on success, throw on failure.
 */
static void check_service_custom_variables(
              QSqlDatabase& db,
              entry const* entries,
              unsigned int entries_size,
              time_t update_low,
              time_t update_high,
              bool modified) {
  // Execute query.
  char const* query("SELECT host_id, service_id, name,"
                    "       default_value, modified, type, "
                    "       update_time, value"
                    "  FROM rt_customvariables"
                    "  WHERE service_id IS NOT NULL"
                    "  ORDER BY host_id, service_id");
  QSqlQuery q(db);
  if (!q.exec(query))
    throw (exceptions::msg()
           << "cannot get services custom variables from DB: "
           << qPrintable(q.lastError().text()));
  for (unsigned int i(0); i < entries_size; ++i) {
    if (!q.next())
      throw (exceptions::msg()
             << "not enough services custom variables in DB: got "
             << i << ", expected "
             << entries_size);
    if ((q.value(0).toUInt() != entries[i].host_id)
        || (q.value(1).toUInt() != entries[i].service_id)
        || (q.value(2).toString() != entries[i].name)
        || (q.value(3).toString() != (entries[i].default_value
                                      ? entries[i].default_value
                                      : entries[i].value))
        || (modified && !q.value(4).toUInt())
        || (!modified && q.value(4).toUInt())
        || (q.value(5).toUInt() != 1)
        || (static_cast<time_t>(q.value(6).toUInt()) < update_low)
        || (static_cast<time_t>(q.value(6).toUInt()) > update_high)
        || (q.value(7).toString() != entries[i].value))
      throw (exceptions::msg() << "invalid service custom variable:"
             << " got (host id " << q.value(0).toUInt()
             << ", service id " << q.value(1).toUInt()
             << ", name " << q.value(2).toString()
             << ", default value " << q.value(3).toString()
             << ", modifed " << q.value(4).toUInt()
             << ", type " << q.value(5).toUInt()
             << ", update time " << q.value(6).toUInt()
             << ", value " << q.value(7).toString()
             << "), expected (" << entries[i].host_id << ", "
             << entries[i].service_id << ", " << entries[i].name
             << ", " << (entries[i].default_value
                         ? entries[i].default_value
                         : entries[i].value) << ", " << modified
             << ", 1, " << update_low << ":" << update_high << ", "
             << entries[i].value << ")");
  }
  if (q.next())
    throw (exceptions::msg()
           << "too much service custom variables in DB");
  return ;
}

/**
 *  Check that custom variables are properly inserted and updated in
 *  SQL database.
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
  engine_extcmd commander;
  engine daemon;
  test_db db;

  try {
    // Prepare database.
    db.open(DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_hosts(hosts, HOST_COUNT);
    {
      unsigned int i(0);
      for (std::list<host>::iterator
             it(hosts.begin()),
             end(hosts.end());
           it != end;
           ++it) {
        std::ostringstream name;
        name << "MYHOSTVARIABLE" << ++i;
        std::ostringstream value;
        value << "This is the value of MYHOSTVARIABLE" << i;
        set_custom_variable(
          *it,
          name.str().c_str(),
          value.str().c_str());
      }
    }
    generate_services(services, hosts, SERVICES_BY_HOST);
    {
      unsigned int i(0);
      for (std::list<service>::iterator
             it(services.begin()),
             end(services.end());
           it != end;
           ++it) {
        std::ostringstream name;
        name << "MYSERVICEVARIABLE" << ++i;
        std::ostringstream value;
        value << "Value (as string) of MYSERVICEVARIABLE" << i
              << " custom variable";
        set_custom_variable(
          *it,
          name.str().c_str(),
          value.str().c_str());
      }
    }
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << PROJECT_SOURCE_DIR
          << "/test/cfg/custom_variables_to_sql.xml";
      additional_config = oss.str();
    }

    // Generate monitoring engine configuration files.
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services);

    // T1.
    time_t t1(time(NULL));

    // Start monitoring engine.
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    daemon.set_config_file(engine_config_file);
    daemon.start();
    sleep_for(10);

    // T2.
    time_t t2(time(NULL));

    // Check hosts custom variables.
    {
      // Expected entries.
      entry const entries[] = {
        {
          1,
          0,
          "MYHOSTVARIABLE1",
          "This is the value of MYHOSTVARIABLE1",
          NULL
        },
        {
          2,
          0,
          "MYHOSTVARIABLE2",
          "This is the value of MYHOSTVARIABLE2",
          NULL
        },
        {
          3,
          0,
          "MYHOSTVARIABLE3",
          "This is the value of MYHOSTVARIABLE3",
          NULL
        }
      };

      // Check.
      check_host_custom_variables(
        *db.centreon_db(),
        entries,
        sizeof(entries) / sizeof(*entries),
        t1,
        t2,
        false);
    }

    // Check services custom variables.
    {
      // Expected entries.
      entry const entries[] = {
        {
          1,
          1,
          "MYSERVICEVARIABLE1",
          "Value (as string) of MYSERVICEVARIABLE1 custom variable",
          NULL
        },
        {
          1,
          2,
          "MYSERVICEVARIABLE2",
          "Value (as string) of MYSERVICEVARIABLE2 custom variable",
          NULL
        },
        {
          2,
          3,
          "MYSERVICEVARIABLE3",
          "Value (as string) of MYSERVICEVARIABLE3 custom variable",
          NULL
        },
        {
          2,
          4,
          "MYSERVICEVARIABLE4",
          "Value (as string) of MYSERVICEVARIABLE4 custom variable",
          NULL
        },
        {
          3,
          5,
          "MYSERVICEVARIABLE5",
          "Value (as string) of MYSERVICEVARIABLE5 custom variable",
          NULL
        },
        {
          3,
          6,
          "MYSERVICEVARIABLE6",
          "Value (as string) of MYSERVICEVARIABLE6 custom variable",
          NULL
        }
      };

      // Check.
      check_service_custom_variables(
        *db.centreon_db(),
        entries,
        sizeof(entries) / sizeof(*entries),
        t1,
        t2,
        false);
    }

    // Delay a little to allow invalid update time to be triggered.
    sleep_for(2);

    // T1.
    t1 = time(NULL);

    // Modify host custom variables.
    {
      commander.execute("CHANGE_CUSTOM_HOST_VAR;2;MYHOSTVARIABLE2;New value of variable #2");
      commander.execute("CHANGE_CUSTOM_HOST_VAR;1;MYHOSTVARIABLE1;Some random value string");
      commander.execute("CHANGE_CUSTOM_HOST_VAR;3;MYHOSTVARIABLE3;Mens sana in corpore sano");
    }

    // Modify service custom variables.
    {
      commander.execute("CHANGE_CUSTOM_SVC_VAR;2;3;MYSERVICEVARIABLE3;Foofoofoofoofoo");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;3;6;MYSERVICEVARIABLE6;Will get erased below");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;1;2;MYSERVICEVARIABLE2;");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;2;4;MYSERVICEVARIABLE4;Bar Bar");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;3;6;MYSERVICEVARIABLE6;Yes ! Erasing the old value ...");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;1;1;MYSERVICEVARIABLE1;");
      commander.execute("CHANGE_CUSTOM_SVC_VAR;3;5;MYSERVICEVARIABLE5;Value (as string) of MYSERVICEVARIABLE5 custom variable");
    }

    // Apply.
    sleep_for(6);

    // T2.
    t2 = time(NULL);

    // Check hosts custom variables.
    {
      // Expected entries.
      entry const entries[] = {
        {
          1,
          0,
          "MYHOSTVARIABLE1",
          "Some random value string",
          "This is the value of MYHOSTVARIABLE1"
        },
        {
          2,
          0,
          "MYHOSTVARIABLE2",
          "New value of variable #2",
          "This is the value of MYHOSTVARIABLE2"
        },
        {
          3,
          0,
          "MYHOSTVARIABLE3",
          "Mens sana in corpore sano",
          "This is the value of MYHOSTVARIABLE3",
        }
      };

      // Check.
      check_host_custom_variables(
        *db.centreon_db(),
        entries,
        sizeof(entries) / sizeof(*entries),
        t1,
        t2,
        true);
    }

    // Check services custom variables.
    {
      // Expected entries.
      entry const entries[] = {
        {
          1,
          1,
          "MYSERVICEVARIABLE1",
          "",
          "Value (as string) of MYSERVICEVARIABLE1 custom variable"
        },
        {
          1,
          2,
          "MYSERVICEVARIABLE2",
          "",
          "Value (as string) of MYSERVICEVARIABLE2 custom variable"
        },
        {
          2,
          3,
          "MYSERVICEVARIABLE3",
          "Foofoofoofoofoo",
          "Value (as string) of MYSERVICEVARIABLE3 custom variable"
        },
        {
          2,
          4,
          "MYSERVICEVARIABLE4",
          "Bar Bar",
          "Value (as string) of MYSERVICEVARIABLE4 custom variable"
        },
        {
          3,
          5,
          "MYSERVICEVARIABLE5",
          "Value (as string) of MYSERVICEVARIABLE5 custom variable",
          "Value (as string) of MYSERVICEVARIABLE5 custom variable"
        },
        {
          3,
          6,
          "MYSERVICEVARIABLE6",
          "Yes ! Erasing the old value ...",
          "Value (as string) of MYSERVICEVARIABLE6 custom variable"
        }
      };

      // Check.
      check_service_custom_variables(
        *db.centreon_db(),
        entries,
        sizeof(entries) / sizeof(*entries),
        t1,
        t2,
        true);
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
  daemon.stop();
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
