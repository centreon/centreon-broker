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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/external_command.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_notification"
#define UTIL_FILE_WRITER PROJECT_SOURCE_DIR"/build/util_write_into_file"
#define TIME_MACROS "$LONGDATETIME$\n$SHORTDATETIME$\n$DATE$\n$TIME$\n$TIMET$\n"
#define HOST_MACROS \
  "$HOSTNAME$\n$HOSTDISPLAYNAME$\n$HOSTALIAS$\n$HOSTADDRESS$\n$HOSTSTATE$\n"\
  "$HOSTSTATEID$\n$HOSTSTATETYPE$\n$HOSTATTEMPT$\n$MAXHOSTATTEMPTS$\n$HOSTLATENCY$\n"\
  "$HOSTEXECUTIONTIME$\n$HOSTDURATION$\n$HOSTDURATIONSEC$\n$HOSTDOWNTIME$\n"\
  "$HOSTPERCENTCHANGE$\n$HOSTGROUPNAME$\n$HOSTGROUPNAMES$\n$LASTHOSTCHECK$\n"\
  "$LASTHOSTSTATECHANGE$\n$LASTHOSTUP$\n$LASTHOSTDOWN$\n"\
  "$LASTHOSTUNREACHABLE$\n$HOSTOUTPUT$\n$LONGHOSTOUTPUT$\n$HOSTPERFDATA$\n"\
  "$HOSTCHECKCOMMAND$\n$TOTALHOSTSERVICES$\n$TOTALHOSTSERVICESOK$\n"\
  "$TOTALHOSTSERVICESWARNING$\n$TOTALHOSTSERVICESUNKNOWN$\n"\
  "$TOTALHOSTSERVICESCRITICAL$\n"
#define SERVICE_MACROS \
  "$SERVICEDESC$\n$SERVICEDISPLAYNAME$\n$SERVICESTATE$\n$SERVICESTATEID$\n"\
  "$LASTSERVICESTATE$\n$LASTSERVICESTATEID$\n$SERVICESTATETYPE$\n"\
  "$SERVICEATTEMPT$\n$MAXSERVICEATTEMPTS$\n$SERVICEISVOLATILE$\n"\
  "$SERVICELATENCY$\n$SERVICEEXECUTIONTIME$\n$SERVICEDURATION$\n"\
  "$SERVICEDURATIONSEC$\n$SERVICEDOWNTIME$\n$SERVICEPERCENTCHANGE$\n"\
  "$SERVICEGROUPNAME$\n$SERVICEGROUPNAMES$\n$LASTSERVICECHECK$\n"\
  "$LASTSERVICESTATECHANGE$\n$LASTSERVICEOK$\n$LASTSERVICEWARNING$\n"\
  "$LASTSERVICEUNKNOWN$\n$LASTSERVICECRITICAL$\n$SERVICEOUTPUT$\n"\
  "$LONGSERVICEOUTPUT$\n$SERVICEPERFDATA$\n$SERVICECHECKCOMMAND$\n"

#define COUNTING_MACROS \
  "$TOTALHOSTSUP$\n$TOTALHOSTSDOWN$\n$TOTALHOSTSUNREACHABLE$\n"\
  "$TOTALHOSTSDOWNUNHANDLED$\n$TOTALHOSTSUNREACHABLEUNHANDLED$\n"\
  "$TOTALHOSTPROBLEMS$\n$TOTALHOSTPROBLEMSUNHANDLED$\n$TOTALSERVICESOK$\n"\
  "$TOTALSERVICESWARNING$\n$TOTALSERVICESCRITICAL$\n$TOTALSERVICESUNKNOWN$\n"\
  "$TOTALSERVICESWARNINGUNHANDLED$\n$TOTALSERVICESCRITICALUNHANDLED$\n"\
  "$TOTALSERVICESUNKNOWNUNHANDLED$\n$TOTALSERVICEPROBLEMS$\n"\
  "$TOTALSERVICEPROBLEMSUNHANDLED$\n"

#define GROUP_MACROS \
  "$HOSTGROUPALIAS$\n$HOSTGROUPMEMBERS$\n$SERVICEGROUPALIAS$\n$SERVICEGROUPMEMBERS$\n"

#define CONTACT_MACROS \
  "$CONTACTNAME$\n$CONTACTALIAS$\n$CONTACTEMAIL$\n$CONTACTPAGER$\n"\
  "$CONTACTADDRESS1$\n$CONTACTADDRESS2$\n$CONTACTADDRESS3$\n"\
  "$CONTACTADDRESS4$\n$CONTACTADDRESS5$\n$CONTACTADDRESS6$\n"\
  "$CONTACTGROUPALIAS$\n$CONTACTGROUPMEMBERS$\n"

#define NOTIFICATION_MACROS \
  "$NOTIFICATIONTYPE$\n$NOTIFICATIONRECIPIENTS$\n$HOSTNOTIFICATIONNUMBER$\n"\
  "$SERVICENOTIFICATIONNUMBER$\n"

#define MACRO_LIST \
  "\"" TIME_MACROS HOST_MACROS SERVICE_MACROS COUNTING_MACROS NOTIFICATION_MACROS GROUP_MACROS CONTACT_MACROS "\""

static const double epsilon = 0.000000001;

struct macros_struct {
  enum macro_type {
    null,
    string,
    integer,
    function,
    between
  };

  macro_type type;

  // Not using an union because no designated initializer in the C++ standard.
  const char* str;
  int         num;
  bool        (*f)(std::string const&);
  double      value1;
  double      value2;

  const char* macro_name;
};

/**
 *  @brief Validate the macros.
 *
 *  Throw on error.
 *
 *  @param[in] macros  The macros.
 */
void validate_macros(
       std::string const& macros_string,
       macros_struct* macros,
       unsigned int num_macros) {
  // Validate that all macros were correctly processed.
  if (macros_string.find_first_of('$') != std::string::npos)
    throw (exceptions::msg() << "a macro wasn't replaced");

  // Validate each macro manually.
  unsigned int index = 0;
  unsigned int i = 0;
  try {
    for (; i < num_macros; ++i) {
      size_t next = macros_string.find_first_of('\n', index);
      if (next == std::string::npos)
        throw (exceptions::msg() << "not enough macro: expected " << num_macros << " macros.");
      std::string substr = macros_string.substr(index, next - index);

      if (macros[i].type == macros_struct::null)
        ; //pass
      else if (macros[i].type == macros_struct::string) {
        if (substr != macros[i].str)
          throw (exceptions::msg()
                   << "invalid macro: expected '" << macros[i].str
                   << "' got '" << substr << "'");
      }
      else if (macros[i].type == macros_struct::integer) {
        std::stringstream ss;
        int num;
        ss << substr;
        ss >> num;
        if (!ss.eof())
          throw (exceptions::msg()
                   << "couldn't parse '" << substr << "' into an int");
        if (num != macros[i].num)
          throw (exceptions::msg()
                   << "invalid macro: expected '" << macros[i].num
                   << "'' got '" << num << "'");
      }
      else if (macros[i].type == macros_struct::function) {
        if (!macros[i].f(substr))
          throw (exceptions::msg()
                   << "could not validate '" << substr << "'");
      }
      else if (macros[i].type == macros_struct::between) {
        std::stringstream ss;
        double value;
        ss << substr;
        ss >> value;
        if (!ss.eof())
          throw (exceptions::msg()
                   << "couldn't parse '" << substr << "' into a double");
        if (value < macros[i].value1 - epsilon || value > macros[i].value2 + epsilon)
          throw (exceptions::msg()
                  << "value outside of bound " << macros[i].value1
                  << " and " << macros[i].value2 << " in '" << substr << "'");
      }
      index = next + 1;
    }
  }
 catch (std::exception const& e) {
      throw (exceptions::msg()
               << "error while trying to validate macro "
               << macros[i].macro_name << ": " << e.what());
  }
}

/**
 *  Check that notification is properly enabled.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  // Variables that need cleaning.
  std::list<host> hosts;
  std::list<service> services;
  std::list<hostgroup> hostgroups;
  std::list<servicegroup> servicegroups;
  std::list<command> commands;
  std::string engine_config_path(tmpnam(NULL));
  std::string flag_file(tmpnam(NULL));
  std::string node_cache_file(tmpnam(NULL));
  external_command commander;
  engine monitoring;
  test_file broker_cfg;
  test_db db;

  try {
    // Log some info.
    std::cout << "flag file: " << flag_file << "\n";
    std::cout << "node cache: " << node_cache_file << "\n";

    // Prepare database.
    db.open(NULL, NULL, DB_NAME);

    // Prepare monitoring engine configuration parameters.
    generate_host_groups(hostgroups, 2);
    unsigned int i = 1;
    for (std::list<hostgroup>::iterator
           it(hostgroups.begin()),
           end(hostgroups.end());
         it != end;
         ++it, ++i) {
      delete [] it->group_name;
      char name[32];
      ::snprintf(name, 31, "HostGroup%i", i);
      name[31] = '\0';
      it->group_name = ::strdup(name);
    }
    generate_service_groups(servicegroups, 2);
    i = 1;
    for (std::list<servicegroup>::iterator
           it(servicegroups.begin()),
           end(servicegroups.end());
         it != end;
         ++it, ++i) {
      delete [] it->group_name;
      char name[32];
      ::snprintf(name, 31, "ServiceGroup%i", i);
      name[31] = '\0';
      it->group_name = ::strdup(name);
    }
    generate_commands(commands, 1);
    delete [] commands.begin()->name;
    commands.begin()->name = ::strdup("service_command_1");
    generate_hosts(hosts, 1);
    hosts.begin()->display_name = ::strdup("DisplayName1");
    delete [] hosts.begin()->alias;
    hosts.begin()->alias = ::strdup("HostAlias1");
    hosts.begin()->checks_enabled = 0;
    hosts.begin()->accept_passive_host_checks = 1;
    link(*hosts.begin(), *hostgroups.begin());
    link(*hosts.begin(), *(++hostgroups.begin()));
    generate_services(services, hosts, 2);
    for (std::list<service>::iterator
           it(services.begin()),
           end(services.end());
         it != end;
         ++it) {
      it->checks_enabled = 0;
      it->accept_passive_service_checks = 1;
      it->max_attempts = 1;
      link(*it, *servicegroups.begin());
      link(*it, *(++servicegroups.begin()));
      delete [] it->display_name;
      char display_name[32];
      ::snprintf(display_name, 31, "ServiceDisplayName%s", it->description);
      display_name[31] = '\0';
      it->display_name = ::strdup(display_name);
      delete [] it->service_check_command;
      it->service_check_command = ::strdup("service_command_1");
    }
    set_custom_variable(
      services.back(),
      "FLAGFILE",
      flag_file.c_str());

    // Populate database.
    db.centreon_run(
         "INSERT INTO cfg_hosts (host_id, host_name, display_name, host_alias)"
         "  VALUES (1, 'Host1', 'DisplayName1', 'HostAlias1')",
         "could not create host");
    db.centreon_run(
         "INSERT INTO cfg_services (service_id,"
         "            service_description)"
         "  VALUES (1, 'Service1'), (2, 'Service2')",
         "could not create services");
    db.centreon_run(
         "INSERT INTO cfg_hosts_services_relations (host_host_id,"
         "            service_service_id)"
         "  VALUES (1, 1), (1, 2)",
         "could not link host and services");
    db.centreon_run(
         "INSERT INTO cfg_hostgroups (hg_name) VALUES('HostGroup1')",
         "could not create the host group");
    db.centreon_run(
         "INSERT INTO cfg_servicegroups (sg_name)"
          "           VALUES('ServiceGroup1'), ('ServiceGroup2')",
         "could not create the service group");

    // Create contact in DB.
    db.centreon_run(
         "INSERT INTO cfg_contacts (contact_id, contact_name)"
         "  VALUES (1, 'Contact1')",
         "could not create contact");
    db.centreon_run(
         "INSERT INTO cfg_contacts_services_relations (contact_id,"
         "            service_service_id)"
         "  VALUES (1, 1), (1, 2)",
         "could not link services and contact");

    // Create notification command in DB.
    db.centreon_run(
         "INSERT INTO cfg_commands (command_id, command_name,"
         "            command_line)"
         "  VALUES (1, 'NotificationCommand1', '"UTIL_FILE_WRITER" "MACRO_LIST" $_SERVICEFLAGFILE$')",
         "could not create notification command");

    // Create notification rules in DB.
    db.centreon_run(
         "INSERT INTO cfg_notification_methods (method_id,"
         "            name, command_id, `interval`)"
         "  VALUES (1, 'NotificationMethod', 1, 300)",
         "could not create notification method");
    db.centreon_run(
         "INSERT INTO cfg_notification_rules (rule_id, method_id, "
         "            timeperiod_id, owner_id, contact_id, host_id,"
         "            service_id, enabled)"
         "  VALUES (1, 1, NULL, 1, 1, 1, 2, 1)",
         "could not create notification rule (cfg)");
    db.centreon_run(
         "INSERT INTO rt_notification_rules (rule_id, method_id,"
         "            timeperiod_id, contact_id, host_id,"
         "            service_id)"
         "  VALUES (1, 1, NULL, 1, 1, 2)",
         "could not create notification rule (rt)");

    // Generate configuration.
    broker_cfg.set_template(
      PROJECT_SOURCE_DIR "/test/cfg/notification.xml.in");
    broker_cfg.set("NODE_CACHE_FILE", node_cache_file);
    commander.set_file(tmpnam(NULL));
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config()
          << "broker_module=" << CBMOD_PATH << " "
          << broker_cfg.generate() << "\n";
      additional_config = oss.str();
    }
    config_write(
      engine_config_path.c_str(),
      additional_config.c_str(),
      &hosts,
      &services,
      &commands,
      &hostgroups,
      &servicegroups);

    // Start monitoring.
    time_t start(time(NULL));
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);
    commander.execute(
      "PROCESS_HOST_CHECK_RESULT;1;0;Host Check Ok");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;1;0;Submitted by unit test");
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;2;0;Submitted by unit test");
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Make service 2 CRITICAL.
    commander.execute(
      "PROCESS_SERVICE_CHECK_RESULT;1;2;2;Critical submitted by unit test");
    commander.execute(
      "PROCESS_HOST_CHECK_RESULT;1;0;Host Check Ok");
    sleep_for(5 * MONITORING_ENGINE_INTERVAL_LENGTH);

    sleep_for(15 * MONITORING_ENGINE_INTERVAL_LENGTH);

    // Check file creation.
    std::ifstream filestream(flag_file.c_str());

    if ((error = !filestream.is_open()))
      throw exceptions::msg() << "Flag file doesn't exist";

    filestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::ostringstream ss;

    ss << filestream.rdbuf();

    std::cout
      <<  "content of " << flag_file << ": "
      << ss.str() << std::endl;

    time_t now(::time(NULL));

    macros_struct macros[] = {
      {macros_struct::null, NULL, 0, NULL, 0, 0, "LONGDATETIME"}, //to parse
      {macros_struct::null, NULL, 0, NULL, 0, 0, "SHORTDATETIME"}, //to parse
      {macros_struct::null, NULL, 0, NULL, 0, 0, "DATE"}, //to parse
      {macros_struct::null, NULL, 0, NULL, 0, 0, "TIME"}, //to parse
      {macros_struct::between, NULL, 0, NULL, start, now, "TIMET"},
      {macros_struct::string, "1", 0, NULL, 0, 0, "HOSTNAME"},
      {macros_struct::string, "DisplayName1", 0, NULL, 0, 0, "HOSTDISPLAYNAME"},
      {macros_struct::string, "HostAlias1", 0, NULL, 0, 0, "HOSTALIAS"},
      {macros_struct::string, "localhost", 0, NULL, 0, 0, "HOSTADDRESS"},
      {macros_struct::string, "UP", 0, NULL, 0, 0, "HOSTSTATE"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "HOSTSTATEID"},
      {macros_struct::string, "HARD", 0, NULL, 0, 0, "HOSTSTATETYPE"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "HOSTATTEMPT"},
      {macros_struct::integer, NULL, 3, NULL, 0, 0, "MAXHOSTATTEMPS"},
      {macros_struct::between, NULL, 0, NULL, 0.0000005, 1.20, "HOSTLATENCY"},
      {macros_struct::between, NULL, 0, NULL, 0, 0, "HOSTEXECUTIONTIME"},
      {macros_struct::null, NULL, 0, NULL, 0, 0, "HOSTDURATION"}, //to parse
      {macros_struct::between, NULL, 0, NULL, 1, ::difftime(now, start), "HOSTDURATIONSEC"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "HOSTDOWNTIME"},
      {macros_struct::between, NULL, 0, NULL, 0, 0, "HOSTPERCENTCHANGE"},
      {macros_struct::string, "HostGroup1", 0, NULL, 0, 0, "HOSTGROUPNAME"},
      {macros_struct::string, "HostGroup1, HostGroup2", 0, NULL, 0, 0, "HOSTGROUPNAMES"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTHOSTCHECK"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTHOSTSTATECHANGE"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTHOSTUP"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTHOSTDOWN"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTHOSTUNREACHABLE"},
      {macros_struct::string, "Host Check Ok", 0, NULL, 0, 0, "HOSTOUTPUT"},
      {macros_struct::string, "", 0, NULL, 0, 0, "LONGHOSTOUTPUT"},
      {macros_struct::string, "", 0, NULL, 0, 0, "HOSTPERFDATA"},
      {macros_struct::string, "default_command", 0, NULL, 0, 0, "HOSTCHECKCOMMAND"},
      {macros_struct::integer, NULL, 2, NULL, 0, 0, "TOTALHOSTSERVICES"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALHOSTSERVICESOK"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSERVICESWARNING"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSERVICESUNKNOWN"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALHOSTSERVICESCRITICAL"},
      {macros_struct::string, "2", 0, NULL, 0, 0, "SERVICEDESC"},
      {macros_struct::string, "ServiceDisplayName2", 0, NULL, 0, 0, "SERVICEDISPLAYNAME"},
      {macros_struct::string, "CRITICAL", 0, NULL, 0, 0, "SERVICESTATE"},
      {macros_struct::integer, NULL, 2, NULL, 0, 0, "SERVICESTATEID"},
      {macros_struct::string, "CRITICAL", 0, NULL, 0, 0, "LASTSERVICESTATE"},
      {macros_struct::integer, NULL, 2, NULL, 0, 0, "LASTSERVICESTATEID"},
      {macros_struct::string, "HARD", 0, NULL, 0, 0, "SERVICESTATETYPE"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "SERVICEATTEMPT"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "MAXSERVICEATTEMPTS"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "SERVICEISVOLATILE"},
      {macros_struct::between, NULL, 0, NULL, 0.0000005, 1.20, "SERVICELATENCY"},
      {macros_struct::between, NULL, 0, NULL, 0, 0, "SERVICEEXECUTIONTIME"},
      {macros_struct::null, NULL, 0, NULL, 0, 0, "SERVICEDURATION"}, //to parse
      {macros_struct::between, NULL, 0, NULL, 1, ::difftime(now, start), "SERVICEDURATIONSEC"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "SERVICEDOWNTIME"},
      {macros_struct::between, NULL, 0, NULL, 6.25, 6.25, "SERVICEPERCENTCHANGE"},
      {macros_struct::string, "ServiceGroup1", 0, NULL, 0, 0, "SERVICEGROUPNAME"},
      {macros_struct::string, "ServiceGroup1, ServiceGroup2", 0, NULL, 0, 0, "SERVICEGROUPNAMES"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTSERVICECHECK"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTSERVICESTATECHANGE"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTSERVICEOK"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTSERVICEWARNING"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTSERVICEUNKNOWN"},
      {macros_struct::between, NULL, 0, NULL, start, now, "LASTSERVICECRITICAL"},
      {macros_struct::string, "Critical submitted by unit test", 0, NULL, 0, 0, "SERVICEOUTPUT"},
      {macros_struct::string, "", 0, NULL, 0, 0, "LONGSERVICEOUTPUT"},
      {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEPERFDATA"},
      {macros_struct::string, "service_command_1", 0, NULL, 0, 0, "SERVICECHECKCOMMAND"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALHOSTSUP"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSDOWN"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSUNREACHABLE"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSDOWNUNHANDLED"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSUNREACHABLEUNHANDLED"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTPROBLEMS"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTPROBLEMSUNHANDLED"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICESOK"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESWARNING"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICESCRITICAL"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESUNKNOWN"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESWARNINGUNHANDLED"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICESCRITICALUNHANDLED"},
      {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESUNKNOWNUNHANDLED"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICEPROBLEMS"},
      {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICEPROBLEMSUNHANDLED"}
      };

    validate_macros(ss.str(), macros, sizeof(macros) / sizeof(*macros));

  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    error = true;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
    error = true;
  }

  // Cleanup.
  monitoring.stop();
  sleep_for(3 * MONITORING_ENGINE_INTERVAL_LENGTH);
  ::remove(flag_file.c_str());
  ::remove(node_cache_file.c_str());
  //std::cout << engine_config_path << std::endl;
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_host_groups(hostgroups);
  free_service_groups(servicegroups);
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
