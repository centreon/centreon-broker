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

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/broker_extcmd.hh"
#include "test/config.hh"
#include "test/engine.hh"
#include "test/engine_extcmd.hh"
#include "test/generate.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define DB_NAME "broker_notification"
#define UTIL_FILE_WRITER PROJECT_SOURCE_DIR "/build/util_write_into_file"
#define TIME_MACROS "$LONGDATETIME$\n$SHORTDATETIME$\n$DATE$\n$TIME$\n$TIMET$\n"
#define HOST_MACROS                                                           \
  "$HOSTNAME$\n$HOSTALIAS$\n$HOSTADDRESS$\n$HOSTSTATE$\n"                     \
  "$HOSTSTATEID$\n$HOSTSTATETYPE$\n$HOSTATTEMPT$\n$MAXHOSTATTEMPTS$\n$"       \
  "HOSTLATENCY$\n"                                                            \
  "$HOSTEXECUTIONTIME$\n$HOSTDURATION$\n$HOSTDURATIONSEC$\n$HOSTDOWNTIME$\n"  \
  "$HOSTPERCENTCHANGE$\n$HOSTGROUPNAME$\n$HOSTGROUPNAMES$\n$LASTHOSTCHECK$\n" \
  "$LASTHOSTSTATECHANGE$\n$LASTHOSTUP$\n$LASTHOSTDOWN$\n"                     \
  "$LASTHOSTUNREACHABLE$\n$HOSTOUTPUT$\n$LONGHOSTOUTPUT$\n$HOSTPERFDATA$\n"   \
  "$HOSTCHECKCOMMAND$\n$TOTALHOSTSERVICES$\n$TOTALHOSTSERVICESOK$\n"          \
  "$TOTALHOSTSERVICESWARNING$\n$TOTALHOSTSERVICESUNKNOWN$\n"                  \
  "$TOTALHOSTSERVICESCRITICAL$\n"
#define SERVICE_MACROS                                                \
  "$SERVICEDESC$\n$SERVICESTATE$\n$SERVICESTATEID$\n"                 \
  "$LASTSERVICESTATE$\n$LASTSERVICESTATEID$\n$SERVICESTATETYPE$\n"    \
  "$SERVICEATTEMPT$\n$MAXSERVICEATTEMPTS$\n$SERVICEISVOLATILE$\n"     \
  "$SERVICELATENCY$\n$SERVICEEXECUTIONTIME$\n$SERVICEDURATION$\n"     \
  "$SERVICEDURATIONSEC$\n$SERVICEDOWNTIME$\n$SERVICEPERCENTCHANGE$\n" \
  "$SERVICEGROUPNAME$\n$SERVICEGROUPNAMES$\n$LASTSERVICECHECK$\n"     \
  "$LASTSERVICESTATECHANGE$\n$LASTSERVICEOK$\n$LASTSERVICEWARNING$\n" \
  "$LASTSERVICEUNKNOWN$\n$LASTSERVICECRITICAL$\n$SERVICEOUTPUT$\n"    \
  "$LONGSERVICEOUTPUT$\n$SERVICEPERFDATA$\n$SERVICECHECKCOMMAND$\n"

#define COUNTING_MACROS                                                       \
  "$TOTALHOSTSUP$\n$TOTALHOSTSDOWN$\n$TOTALHOSTSUNREACHABLE$\n"               \
  "$TOTALHOSTSDOWNUNHANDLED$\n$TOTALHOSTSUNREACHABLEUNHANDLED$\n"             \
  "$TOTALHOSTPROBLEMS$\n$TOTALHOSTPROBLEMSUNHANDLED$\n$TOTALSERVICESOK$\n"    \
  "$TOTALSERVICESWARNING$\n$TOTALSERVICESCRITICAL$\n$TOTALSERVICESUNKNOWN$\n" \
  "$TOTALSERVICESWARNINGUNHANDLED$\n$TOTALSERVICESCRITICALUNHANDLED$\n"       \
  "$TOTALSERVICESUNKNOWNUNHANDLED$\n$TOTALSERVICEPROBLEMS$\n"                 \
  "$TOTALSERVICEPROBLEMSUNHANDLED$\n"

#define GROUP_MACROS                                             \
  "$HOSTGROUPALIAS$\n$HOSTGROUPMEMBERS$\n$SERVICEGROUPALIAS$\n$" \
  "SERVICEGROUPMEMBERS$\n"

#define CONTACT_MACROS                                              \
  "$CONTACTNAME$\n$CONTACTALIAS$\n$CONTACTEMAIL$\n$CONTACTPAGER$\n" \
  "$CONTACTADDRESS1$\n$CONTACTADDRESS2$\n$CONTACTADDRESS3$\n"       \
  "$CONTACTADDRESS4$\n$CONTACTADDRESS5$\n$CONTACTADDRESS6$\n"       \
  "$CONTACTGROUPALIAS$\n$CONTACTGROUPMEMBERS$\n"

#define NOTIFICATION_MACROS                                                  \
  "$NOTIFICATIONTYPE$\n$NOTIFICATIONRECIPIENTS$\n$HOSTNOTIFICATIONNUMBER$\n" \
  "$SERVICENOTIFICATIONNUMBER$\n"

#define MACRO_LIST                                            \
  "\"" TIME_MACROS HOST_MACROS SERVICE_MACROS COUNTING_MACROS \
      NOTIFICATION_MACROS GROUP_MACROS CONTACT_MACROS "\""

#define RECOVERY_FILE_CONTENT "\"$NOTIFICATIONTYPE$\n\""

#define DOWNTIME_FILE_CONTENT "\"$NOTIFICATIONTYPE$\n\""

#define ACK_FILE_CONTENT "\"$NOTIFICATIONTYPE$\n\""

static const double epsilon = 0.000000001;
static time_t start;
static time_t now;

struct macros_struct {
  enum macro_type { null, string, integer, function, between };

  macro_type type;

  // Not using an union because no designated initializer in the C++ standard.
  const char* str;
  int num;
  bool (*f)(std::string const&);
  double value1;
  double value2;

  const char* macro_name;
};

/**
 *  Del a c string and dup another.
 *
 *  @param[out] str    The string to del.
 *  @param[in] format  The format of the string to dup.
 *  @param[in] i       Param for the format.
 */
void del_and_dup(char** str, std::string const& format, int i) {
  delete[] * str;
  char name[32];
  ::snprintf(name, 31, format.c_str(), i);
  name[31] = '\0';
  *str = ::strdup(name);
}

/**
 *  Validate a date.
 *
 *  @param[in] str  The string to validate.
 *
 *  @return  True if valid.
 */
bool validate_date(std::string const& str) {
  int month, day, year;
  if (sscanf(str.c_str(), "%d-%d-%d", &month, &day, &year) != 3)
    return (false);

  struct tm t;
  time_t now_time = ::time(NULL);
  ::localtime_r(&now_time, &t);

  if (month != t.tm_mon + 1 || day != t.tm_mday || year != t.tm_year + 1900)
    return (false);

  return (true);
}

/**
 *  Validate a time.
 *
 *  @param[in] str  The string to validate.
 *
 *  @return  True if valid.
 */
bool validate_time(std::string const& str) {
  int hour, min, sec;

  if (sscanf(str.c_str(), "%d:%d:%d", &hour, &min, &sec) != 3)
    return (false);

  struct tm t;
  time_t now_time = ::time(NULL);
  ::localtime_r(&now_time, &t);
  t.tm_hour = hour;
  t.tm_min = min;
  t.tm_sec = sec;
  time_t res = ::mktime(&t);

  if (res < start || res > now)
    return (false);

  return (true);
}

/**
 *  Validate a long date time.
 *
 *  @param[in] str  The string to validate.
 *
 *  @return  True if valid.
 */
bool validate_long_date_time(std::string const& str) {
  (void)str;

  return (true);
}

/**
 *  Validate a short date time.
 *
 *  @param[in] str  The string to validate.
 *
 *  @return  True if valid.
 */
bool validate_short_date_time(std::string const& str) {
  size_t sub = str.find_first_of(' ');
  if (sub == std::string::npos)
    return (false);

  std::string date = str.substr(0, sub);
  std::string time = str.substr(sub + 1);

  return (validate_date(date) && validate_time(time));
}

/**
 *  Validate a duration string (xd x'h x''m x'''s)
 *
 *  @param[in] str  The string.
 *
 *  @return  True if valid.
 */
bool validate_durations(std::string const& str) {
  int day, hour, min, sec;
  if (sscanf(str.c_str(), "%id %ih %im %is", &day, &hour, &min, &sec) != 4)
    return (false);

  if (day != 0 || hour != 0 || min != 0 || sec <= 0)
    return (false);

  return (true);
}

/**
 *  @brief Validate the macros.
 *
 *  Throw on error.
 *
 *  @param[in] macros_string  The string containing all the macros.
 *  @param[in] macros         The model macros.
 *  @param[in] num_macros     The number of macros expected.
 */
void validate_macros(std::string const& macros_string,
                     macros_struct* macros,
                     uint32_t num_macros) {
  // Validate that all macros were correctly processed.
  if (macros_string.find_first_of('$') != std::string::npos)
    throw(exceptions::msg() << "a macro wasn't replaced");

  // Validate each macro manually.
  uint32_t index = 0;
  uint32_t i = 0;
  try {
    for (; i < num_macros; ++i) {
      size_t next = macros_string.find_first_of('\n', index);
      if (next == std::string::npos)
        throw(exceptions::msg()
              << "not enough macro: expected " << num_macros << " macros.");
      std::string substr = macros_string.substr(index, next - index);

      if (macros[i].type == macros_struct::null)
        ;  // pass
      else if (macros[i].type == macros_struct::string) {
        if (substr != macros[i].str)
          throw(exceptions::msg()
                << "invalid macro: expected '" << macros[i].str << "' got '"
                << substr << "'");
      } else if (macros[i].type == macros_struct::integer) {
        std::stringstream ss;
        int num;
        ss << substr;
        ss >> num;
        if (!ss.eof())
          throw(exceptions::msg()
                << "couldn't parse '" << substr << "' into an int");
        if (num != macros[i].num)
          throw(exceptions::msg() << "invalid macro: expected '"
                                  << macros[i].num << "'' got '" << num << "'");
      } else if (macros[i].type == macros_struct::function) {
        if (!macros[i].f(substr))
          throw(exceptions::msg() << "could not validate '" << substr << "'");
      } else if (macros[i].type == macros_struct::between) {
        std::stringstream ss;
        double value;
        ss << substr;
        ss >> value;
        if (!ss.eof())
          throw(exceptions::msg()
                << "couldn't parse '" << substr << "' into a double");
        if (value < macros[i].value1 - epsilon ||
            value > macros[i].value2 + epsilon)
          throw(exceptions::msg()
                << "value outside of bound " << macros[i].value1 << " and "
                << macros[i].value2 << " in '" << substr << "'");
      }
      index = next + 1;
    }
  } catch (std::exception const& e) {
    throw(exceptions::msg() << "error while trying to validate macro "
                            << macros[i].macro_name << ": " << e.what());
  }
}

/**
 *  Get a file.
 *
 *  @param[in] filename  The filename.
 *  @param[out] error    Error flag.
 *  @param[out] ss       The resulting string stream.
 */
void get_file(std::string const& filename,
              bool& error,
              std::ostringstream& ss) {
  ss.str("");
  std::ifstream filestream(filename.c_str());

  if ((error = !filestream.is_open()))
    throw(exceptions::msg() << "flag file '" << filename << "' doesn't exist");

  filestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  ss << filestream.rdbuf();

  std::cout << "content of " << filename << ": " << ss.str();
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
  std::list<command> commands;
  std::string engine_config_path(tmpnam(NULL));
  std::string flag_file(tmpnam(NULL));
  std::string flag_file2(tmpnam(NULL));
  std::string flag_file3(tmpnam(NULL));
  std::string flag_file4(tmpnam(NULL));
  std::string node_cache_file(tmpnam(NULL));
  engine_extcmd commander;
  broker_extcmd broker_commander;
  engine monitoring;
  test_file broker_cfg;
  test_db db;

  try {
    // Log some info.
    std::cout << "flag file 1 (normal notification): " << flag_file << "\n";
    std::cout << "flag file 2 (up notification): " << flag_file2 << "\n";
    std::cout << "flag file 3 (downtime notification): " << flag_file3 << "\n";
    std::cout << "flag file 4 (ack notification): " << flag_file4 << "\n";
    std::cout << "node cache: " << node_cache_file << "\n";

    // Prepare database.
    db.open(DB_NAME, NULL, true);

    // Prepare monitoring engine configuration parameters.
    generate_commands(commands, 1);
    del_and_dup(&commands.begin()->name, "service_command_%i", 1);
    generate_hosts(hosts, 1);
    del_and_dup(&hosts.begin()->name, "Host%i", 1);
    // XXX del_and_dup(&hosts.begin()->display_name, "%i", 1);
    del_and_dup(&hosts.begin()->alias, "HostAlias%i", 1);
    hosts.begin()->checks_enabled = 0;
    generate_services(services, hosts, 2);
    int i(1);
    for (std::list<service>::iterator it(services.begin()), end(services.end());
         it != end; ++it, ++i) {
      it->checks_enabled = 0;
      it->max_attempts = 1;
      del_and_dup(&it->description, "Service%i", i);
      // XXX del_and_dup(&it->display_name, "%i", i);
      del_and_dup(&it->service_check_command, "service_command_%i", 1);
      del_and_dup(&it->host_name, "Host%i", 1);
    }
    set_custom_variable(services.back(), "FLAGFILE", flag_file.c_str());
    set_custom_variable(services.back(), "FLAGFILE2", flag_file2.c_str());
    set_custom_variable(services.back(), "FLAGFILE3", flag_file3.c_str());
    set_custom_variable(services.back(), "FLAGFILE4", flag_file4.c_str());

    // Populate database.
    db.centreon_run(
        "INSERT INTO cfg_organizations (organization_id, name, shortname)"
        "  VALUES (1, '42', '42')",
        "could not create organization");

    db.centreon_run(
        "INSERT INTO cfg_hosts (host_id, host_name, host_alias, "
        "organization_id)"
        "  VALUES (1, 'Host1', 'HostAlias1', 1)",
        "could not create host");
    db.centreon_run(
        "INSERT INTO cfg_services (service_id,"
        "            service_description, organization_id)"
        "  VALUES (1, 'Service1', 1), (2, 'Service2', 1)",
        "could not create services");
    db.centreon_run(
        "INSERT INTO cfg_hosts_services_relations (host_host_id,"
        "            service_service_id)"
        "  VALUES (1, 1), (1, 2)",
        "could not link host and services");

    // Create contact in DB.
    db.centreon_run(
        "INSERT INTO cfg_contacts (contact_id, description)"
        "  VALUES (1, 'Contact1')",
        "could not create contact");
    db.centreon_run(
        "INSERT INTO cfg_contacts_infos (contact_id, info_key, info_value)"
        "  VALUES (1, 'email', 'ContactEmail1'), (1, 'pager', 'ContactPager1'),"
        "        (1, 'address1', 'ContactAddress1'), (1, 'address2', "
        "'ContactAddress2'),"
        "        (1, 'address3', 'ContactAddress3'), (1, 'address4', "
        "'ContactAddress4'),"
        "        (1, 'address5', 'ContactAddress5'), (1, 'address6', "
        "'ContactAddress6')",
        "could not create contact infos");

    // Create notification command in DB.
    db.centreon_run(
        "INSERT INTO cfg_commands (command_id, command_name,"
        "            command_line, organization_id)"
        "  VALUES (1, 'NotificationCommand1', '" UTIL_FILE_WRITER " " MACRO_LIST
        " $_SERVICEFLAGFILE$', 1),"
        "         (2, 'NotificationCommand2', '" UTIL_FILE_WRITER
        " " RECOVERY_FILE_CONTENT
        " $_SERVICEFLAGFILE2$', 1),"
        "         (3, 'NotificationCommand3', '" UTIL_FILE_WRITER
        " " DOWNTIME_FILE_CONTENT
        " $_SERVICEFLAGFILE3$', 1),"
        "         (4, 'NotificationCommand4', '" UTIL_FILE_WRITER
        " " ACK_FILE_CONTENT " $_SERVICEFLAGFILE4$', 1)",
        "could not create notification command");

    // Create notification rules in DB.
    db.centreon_run(
        "INSERT INTO cfg_notification_methods (method_id,"
        "            name, command_id, `interval`, types, status)"
        "  VALUES (1, 'NotificationMethod', 1, 300, 'n', 'w,c,u'),"
        "         (2, 'NotificationMethod2', 2, 300, 'r', 'o'),"
        "        (3, 'NotificationMethod3', 3, 300, 'd', 'o'),"
        "        (4, 'NotificationMethod4', 4, 300, 'a', 'o')",
        "could not create notification method");
    db.centreon_run(
        "INSERT INTO cfg_notification_rules (rule_id, method_id, "
        "            timeperiod_id, owner_id, contact_id, host_id,"
        "            service_id, enabled)"
        "  VALUES (1, 1, NULL, 1, 1, 1, 2, 1),"
        "         (2, 2, NULL, 1, 1, 1, 2, 1),"
        "         (3, 3, NULL, 1, 1, 1, 2, 1),"
        "         (4, 4, NULL, 1, 1, 1, 2, 1)",
        "could not create notification rule (cfg)");
    db.centreon_run(
        "INSERT INTO rt_notification_rules (rule_id, method_id,"
        "            timeperiod_id, contact_id, host_id,"
        "            service_id)"
        "  VALUES (1, 1, NULL, 1, 1, 2),"
        "         (2, 2, NULL, 1, 1, 2),"
        "         (3, 3, NULL, 1, 1, 2),"
        "         (4, 4, NULL, 1, 1, 2)",
        "could not create notification rule (rt)");

    // Generate configuration.
    broker_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/notification.xml.in");
    broker_cfg.set("NODE_CACHE_FILE", node_cache_file);
    commander.set_file(tmpnam(NULL));
    broker_commander.set_file(tmpnam(NULL));
    broker_cfg.set("BROKER_COMMAND_FILE", broker_commander.get_file());
    std::string additional_config;
    {
      std::ostringstream oss;
      oss << commander.get_engine_config() << "broker_module=" << CBMOD_PATH
          << " " << broker_cfg.generate() << "\n";
      additional_config = oss.str();
    }
    config_write(engine_config_path.c_str(), additional_config.c_str(), &hosts,
                 &services, &commands);

    // Start monitoring.
    start = ::time(NULL);
    std::string engine_config_file(engine_config_path);
    engine_config_file.append("/nagios.cfg");
    monitoring.set_config_file(engine_config_file);
    monitoring.start();
    sleep_for(3);
    commander.execute("PROCESS_HOST_CHECK_RESULT;Host1;0;Host Check Ok");
    commander.execute(
        "PROCESS_SERVICE_CHECK_RESULT;Host1;Service1;0;Submitted by unit test");
    commander.execute(
        "PROCESS_SERVICE_CHECK_RESULT;Host1;Service2;0;Submitted by unit test");
    sleep_for(5);

    // Make service 2 CRITICAL.
    commander.execute(
        "PROCESS_SERVICE_CHECK_RESULT;Host1;Service2;2;Critical submitted by "
        "unit test");
    commander.execute("PROCESS_HOST_CHECK_RESULT;Host1;0;Host Check Ok");
    sleep_for(7);

    // Check file creation.
    std::ostringstream ss;
    get_file(flag_file, error, ss);

    now = ::time(NULL);
    {
      macros_struct macros[] = {
          {macros_struct::function, NULL, 0, validate_long_date_time, 0, 0,
           "LONGDATETIME"},
          {macros_struct::function, NULL, 0, validate_short_date_time, 0, 0,
           "SHORTDATETIME"},
          {macros_struct::function, NULL, 0, validate_date, 0, 0, "DATE"},
          {macros_struct::function, NULL, 0, validate_time, 0, 0, "TIME"},
          {macros_struct::between, NULL, 0, NULL, start, now, "TIMET"},
          {macros_struct::string, "Host1", 0, NULL, 0, 0, "HOSTNAME"},
          {macros_struct::string, "HostAlias1", 0, NULL, 0, 0, "HOSTALIAS"},
          {macros_struct::string, "localhost", 0, NULL, 0, 0, "HOSTADDRESS"},
          {macros_struct::string, "UP", 0, NULL, 0, 0, "HOSTSTATE"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "HOSTSTATEID"},
          {macros_struct::string, "HARD", 0, NULL, 0, 0, "HOSTSTATETYPE"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "HOSTATTEMPT"},
          {macros_struct::integer, NULL, 3, NULL, 0, 0, "MAXHOSTATTEMPS"},
          {macros_struct::between, NULL, 0, NULL, 0.0000005, 1.20,
           "HOSTLATENCY"},
          {macros_struct::between, NULL, 0, NULL, 0, 0, "HOSTEXECUTIONTIME"},
          {macros_struct::function, NULL, 0, validate_durations, 0, 0,
           "HOSTDURATION"},
          {macros_struct::between, NULL, 0, NULL, 1, ::difftime(now, start),
           "HOSTDURATIONSEC"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "HOSTDOWNTIME"},
          {macros_struct::between, NULL, 0, NULL, 0, 0, "HOSTPERCENTCHANGE"},
          {macros_struct::string, "", 0, NULL, 0, 0, "HOSTGROUPNAME"},
          {macros_struct::string, "", 0, NULL, 0, 0, "HOSTGROUPNAMES"},
          {macros_struct::between, NULL, 0, NULL, start, now, "LASTHOSTCHECK"},
          {macros_struct::between, NULL, 0, NULL, start, now,
           "LASTHOSTSTATECHANGE"},
          {macros_struct::between, NULL, 0, NULL, start, now, "LASTHOSTUP"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTHOSTDOWN"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTHOSTUNREACHABLE"},
          {macros_struct::string, "Host Check Ok", 0, NULL, 0, 0, "HOSTOUTPUT"},
          {macros_struct::string, "", 0, NULL, 0, 0, "LONGHOSTOUTPUT"},
          {macros_struct::string, "", 0, NULL, 0, 0, "HOSTPERFDATA"},
          {macros_struct::string, "default_command", 0, NULL, 0, 0,
           "HOSTCHECKCOMMAND"},
          {macros_struct::integer, NULL, 2, NULL, 0, 0, "TOTALHOSTSERVICES"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALHOSTSERVICESOK"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTSERVICESWARNING"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTSERVICESUNKNOWN"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "TOTALHOSTSERVICESCRITICAL"},
          {macros_struct::string, "Service2", 0, NULL, 0, 0, "SERVICEDESC"},
          {macros_struct::string, "CRITICAL", 0, NULL, 0, 0, "SERVICESTATE"},
          {macros_struct::integer, NULL, 2, NULL, 0, 0, "SERVICESTATEID"},
          {macros_struct::string, "OK", 0, NULL, 0, 0, "LASTSERVICESTATE"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTSERVICESTATEID"},
          {macros_struct::string, "HARD", 0, NULL, 0, 0, "SERVICESTATETYPE"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "SERVICEATTEMPT"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "MAXSERVICEATTEMPTS"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "SERVICEISVOLATILE"},
          {macros_struct::between, NULL, 0, NULL, 0.0000005, 1.20,
           "SERVICELATENCY"},
          {macros_struct::between, NULL, 0, NULL, 0, 0, "SERVICEEXECUTIONTIME"},
          {macros_struct::function, NULL, 0, validate_durations, 0, 0,
           "SERVICEDURATION"},
          {macros_struct::between, NULL, 0, NULL, 1, ::difftime(now, start),
           "SERVICEDURATIONSEC"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "SERVICEDOWNTIME"},
          {macros_struct::between, NULL, 0, NULL, 6.25, 6.25,
           "SERVICEPERCENTCHANGE"},
          {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEGROUPNAME"},
          {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEGROUPNAMES"},
          {macros_struct::between, NULL, 0, NULL, start, now,
           "LASTSERVICECHECK"},
          {macros_struct::between, NULL, 0, NULL, start, now,
           "LASTSERVICESTATECHANGE"},
          {macros_struct::between, NULL, 0, NULL, start, now, "LASTSERVICEOK"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTSERVICEWARNING"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "LASTSERVICEUNKNOWN"},
          {macros_struct::between, NULL, 0, NULL, start, now,
           "LASTSERVICECRITICAL"},
          {macros_struct::string, "Critical submitted by unit test", 0, NULL, 0,
           0, "SERVICEOUTPUT"},
          {macros_struct::string, "", 0, NULL, 0, 0, "LONGSERVICEOUTPUT"},
          {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEPERFDATA"},
          {macros_struct::string, "service_command_1", 0, NULL, 0, 0,
           "SERVICECHECKCOMMAND"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALHOSTSUP"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTSDOWN"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTSUNREACHABLE"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTSDOWNUNHANDLED"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTSUNREACHABLEUNHANDLED"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALHOSTPROBLEMS"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALHOSTPROBLEMSUNHANDLED"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICESOK"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESWARNING"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "TOTALSERVICESCRITICAL"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0, "TOTALSERVICESUNKNOWN"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALSERVICESWARNINGUNHANDLED"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "TOTALSERVICESCRITICALUNHANDLED"},
          {macros_struct::integer, NULL, 0, NULL, 0, 0,
           "TOTALSERVICESUNKNOWNUNHANDLED"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0, "TOTALSERVICEPROBLEMS"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "TOTALSERVICEPROBLEMSUNHANDLED"},
          {macros_struct::string, "PROBLEM", 0, NULL, 0, 0, "NOTIFICATIONTYPE"},
          {macros_struct::string, "Contact1", 0, NULL, 0, 0,
           "NOTIFICATIONRECIPIENT"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "HOSTNOTIFICATIONNUMBER"},
          {macros_struct::integer, NULL, 1, NULL, 0, 0,
           "SERVICENOTIFICATIONNUMBER"},
          {macros_struct::string, "", 0, NULL, 0, 0, "HOSTGROUPALIAS"},
          {macros_struct::string, "", 0, NULL, 0, 0, "HOSTGROUPMEMBERS"},
          {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEGROUPALIAS"},
          {macros_struct::string, "", 0, NULL, 0, 0, "SERVICEGROUPMEMBERS"},
          {macros_struct::string, "Contact1", 0, NULL, 0, 0, "CONTACTNAME"},
          {macros_struct::string, "Contact1", 0, NULL, 0, 0, "CONTACTALIAS"},
          {macros_struct::string, "ContactEmail1", 0, NULL, 0, 0,
           "CONTACTEMAIL"},
          {macros_struct::string, "ContactPager1", 0, NULL, 0, 0,
           "CONTACTPAGER"},
          {macros_struct::string, "ContactAddress1", 0, NULL, 0, 0,
           "CONTACTADDRESS1"},
          {macros_struct::string, "ContactAddress2", 0, NULL, 0, 0,
           "CONTACTADDRESS2"},
          {macros_struct::string, "ContactAddress3", 0, NULL, 0, 0,
           "CONTACTADDRESS3"},
          {macros_struct::string, "ContactAddress4", 0, NULL, 0, 0,
           "CONTACTADDRESS4"},
          {macros_struct::string, "ContactAddress5", 0, NULL, 0, 0,
           "CONTACTADDRESS5"},
          {macros_struct::string, "ContactAddress6", 0, NULL, 0, 0,
           "CONTACTADDRESS6"},
          {macros_struct::string, "", 0, NULL, 0, 0, "CONTACTGROUPALIAS"},
          {macros_struct::string, "", 0, NULL, 0, 0, "CONTACTGROUPMEMBERS"}};

      validate_macros(ss.str(), macros, sizeof(macros) / sizeof(*macros));
    }

    // Make service 2 OK.
    start = ::time(NULL);
    commander.execute(
        "PROCESS_SERVICE_CHECK_RESULT;Host1;Service2;0;Critical submitted by "
        "unit test");
    sleep_for(10);

    // Check file creation.
    get_file(flag_file2, error, ss);
    {
      macros_struct macros[] = {{macros_struct::string, "RECOVERY", 0, NULL, 0,
                                 0, "NOTIFICATIONTYPE"}};

      validate_macros(ss.str(), macros, sizeof(macros) / sizeof(*macros));
    }

    // Check downtimes
    time_t start = ::time(NULL);
    time_t end = start + 5;
    ss.str("");
    ss << "EXECUTE;84;notification-nodeevents;SCHEDULE_SVC_DOWNTIME;Host1;"
          "Service2;"
       << start << ";" << end << ";1;0;5;test author;some comments;test;0";
    broker_commander.execute(ss.str());

    sleep_for(10);

    get_file(flag_file3, error, ss);
    {
      macros_struct macros[] = {{macros_struct::string, "DOWNTIME", 0, NULL, 0,
                                 0, "NOTIFICATIONTYPE"}};

      validate_macros(ss.str(), macros, sizeof(macros) / sizeof(*macros));
    }

    // Check acks
    broker_commander.execute(
        "EXECUTE;84;notification-nodeevents;ACKNOWLEDGE_SVC_PROBLEM;Host1;"
        "Service2;2;1;1;test author;some comments");

    sleep_for(3);
    get_file(flag_file4, error, ss);
    {
      macros_struct macros[] = {{macros_struct::string, "ACKNOWLEDGEMENT", 0,
                                 NULL, 0, 0, "NOTIFICATIONTYPE"}};

      validate_macros(ss.str(), macros, sizeof(macros) / sizeof(*macros));
    }

  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    error = true;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
    error = true;
  }

  // Cleanup.
  monitoring.stop();
  sleep_for(3);
  ::remove(flag_file.c_str());
  ::remove(flag_file2.c_str());
  ::remove(flag_file3.c_str());
  ::remove(flag_file4.c_str());
  ::remove(node_cache_file.c_str());
  config_remove(engine_config_path.c_str());
  free_hosts(hosts);
  free_services(services);
  free_commands(commands);

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
