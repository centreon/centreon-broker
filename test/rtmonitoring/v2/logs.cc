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

#include <QSqlQuery>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/db.hh"
#include "test/file.hh"
#include "test/misc.hh"
#include "test/predicate.hh"
#include "test/time_points.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

#define TEST_NAME "rtmonitoring_v2_logs"
#define DB_NAME "broker_" TEST_NAME

// Check count.
static int check_number(0);

/**
 *  Precheck routine.
 */
static void precheck(char const* name) {
  ++check_number;
  std::cout << "check #" << check_number << " (" << name << ")\n";
  return;
}

/**
 *  Postcheck routine.
 */
static void postcheck(test::db& db,
                      std::string const& type_list,
                      test::predicate expected[][14]) {
  static std::string check_query_template_1(
      "SELECT ctime, host_id, host_name, instance_name, issue_id,"
      "       msg_type, notification_cmd, notification_contact, output,"
      "       retry, service_description, service_id, status, type"
      "  FROM logs"
      "  WHERE msg_type IN ");
  static std::string check_query_template_2(
      "  ORDER BY host_name ASC, service_description ASC");
  std::ostringstream check_query;
  check_query << check_query_template_1 << " (" << type_list << ")"
              << check_query_template_2;
  db.check_content(check_query.str(), expected);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the logs table work properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "logs", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5576");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:log_entry</category>"
                "</write_filters>");
    test::cbd broker;
    broker.set_config_file(cbd_cfg.generate());
    broker.start();
    test::sleep_for(1);

    // Monitoring engine.
    test::file cbmod_cfg;
    cbmod_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/tcp.xml.in");
    cbmod_cfg.set("BROKER_ID", "83");
    cbmod_cfg.set("BROKER_NAME", TEST_NAME "-cbmod");
    cbmod_cfg.set("POLLER_ID", "42");
    cbmod_cfg.set("POLLER_NAME", "my-poller");
    cbmod_cfg.set("TCP_HOST", "localhost");
    cbmod_cfg.set("TCP_PORT", "5576");
    test::centengine_config engine_config;
    {
      test::centengine_object cmd(test::centengine_object::command_type);
      cmd.set("command_name", "test_command");
      cmd.set("command_line", MY_PLUGIN_PATH
              " $ARG1$"
              "$ARG2$");
      engine_config.get_commands().push_back(cmd);
    }
    {
      test::centengine_object cntct(test::centengine_object::contact_type);
      cntct.set("contact_name", "test_contact");
      cntct.set("email", "test@contact");
      cntct.set("host_notifications_enabled", "1");
      cntct.set("service_notifications_enabled", "1");
      cntct.set("host_notification_period", "default_timeperiod");
      cntct.set("service_notification_period", "default_timeperiod");
      cntct.set("host_notification_commands",
                "test_command!0!notificationhost");
      cntct.set("service_notification_commands",
                "test_command!0!notificationservice");
      cntct.set("host_notification_options", "a");
      cntct.set("service_notification_options", "a");
      engine_config.get_contacts().push_back(cntct);
    }
    engine_config.generate_hosts(2);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_hosts().begin()),
         end(engine_config.get_hosts().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("check_command", "test_command!0!outputhost");
      it->set("contacts", "test_contact");
      it->set("max_check_attempts", "1");
      it->set("notifications_enabled", "1");
      it->set("notification_options", "a");
      it->set("notification_period", "default_timeperiod");
      it->set("passive_checks_enabled", "1");
    }
    engine_config.generate_services(1);
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_services().begin()),
         end(engine_config.get_services().end());
         it != end; ++it) {
      it->set("active_checks_enabled", "0");
      it->set("check_command", "test_command!0!outputservice");
      it->set("contacts", "test_contact");
      it->set("max_check_attempts", "1");
      it->set("notifications_enabled", "1");
      it->set("notification_options", "a");
      it->set("notification_period", "default_timeperiod");
      it->set("passive_checks_enabled", "1");
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    engine_config.set_directive("accept_passive_host_checks", "1");
    engine_config.set_directive("accept_passive_service_checks", "1");
    engine_config.set_directive("execute_host_checks", "1");
    engine_config.set_directive("execute_service_checks", "1");
    engine_config.set_directive("log_initial_states", "1");
    engine_config.set_directive("notifications_enabled", "1");
    // Used to generate a warning log.
    engine_config.set_directive("child_processes_fork_twice", "1");
    test::centengine engine(&engine_config);
    test::time_points tpoints;
    tpoints.store();
    engine.start();
    test::sleep_for(1);
    engine.reload();
    test::sleep_for(2);

    // Check configuration warning.
    {
      precheck("configuration warning");
      tpoints.store();
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1),
           test::predicate(test::predicate::type_null), "", "my-poller",
           test::predicate(test::predicate::type_null), 4, "", "",
           "child_processes_fork_twice variable ignored", 0, "",
           test::predicate(test::predicate::type_null), 0, 0},
          {test::predicate()}};
      postcheck(db, "4", expected);
    }

    // Check initial host state.
    {
      precheck("initial host state");
      test::predicate expected[][14] = {
          // #1.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1", "",
           test::predicate(test::predicate::type_null), 9, "", "",
           "INITIAL HOST STATE: 1;UP;HARD;1;", 1, "",
           test::predicate(test::predicate::type_null), 0, 1},
          // #2.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 2, "2", "",
           test::predicate(test::predicate::type_null), 9, "", "",
           "INITIAL HOST STATE: 2;UP;HARD;1;", 1, "",
           test::predicate(test::predicate::type_null), 0, 1},
          // Default host.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1),
           test::predicate(test::predicate::type_null), "default_host", "",
           test::predicate(test::predicate::type_null), 9, "", "",
           "INITIAL HOST STATE: default_host;UP;HARD;1;", 1, "",
           test::predicate(test::predicate::type_null), 0, 1},
          {test::predicate()}};
      postcheck(db, "9", expected);
    }

    // Check initial service state.
    {
      precheck("initial service state");
      test::predicate expected[][14] = {
          // #1.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1", "",
           test::predicate(test::predicate::type_null), 8, "", "",
           "INITIAL SERVICE STATE: 1;1;OK;HARD;1;", 1, "1", 1, 0, 1},
          // #2.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 2, "2", "",
           test::predicate(test::predicate::type_null), 8, "", "",
           "INITIAL SERVICE STATE: 2;2;OK;HARD;1;", 1, "2", 2, 0, 1},
          // Default service.
          {test::predicate(tpoints.prelast(), tpoints.last() + 1),
           test::predicate(test::predicate::type_null), "default_host", "",
           test::predicate(test::predicate::type_null), 8, "", "",
           "INITIAL SERVICE STATE: default_host;default_service;OK;HARD;1;", 1,
           "default_service", test::predicate(test::predicate::type_null), 0,
           1},
          {test::predicate()}};
      postcheck(db, "8", expected);
    }

    // External command will generate service
    // alert and service notification.
    tpoints.store();
    engine.extcmd().execute("PROCESS_SERVICE_CHECK_RESULT;1;1;2;output");
    test::sleep_for(2);
    tpoints.store();

    // Check service alert.
    {
      precheck("service alert");
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 0, "", "",
           "output", 1, "1", 1, 2, 1},
          {test::predicate()}};
      postcheck(db, "0", expected);
    }

    // Check service notification.
    {
      precheck("service notification");
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 2,
           "test_command", "test_contact", "output", 0, "1", 1, 2, 0},
          {test::predicate()}};
      postcheck(db, "2", expected);
    }

    // Check service acknowledgement.
    {
      precheck("service acknowledgement");
      engine.extcmd().execute(
          "ACKNOWLEDGE_SVC_PROBLEM;1;1;0;0;1;admin;comment");
      test::sleep_for(2);
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 10, "",
           "admin", "comment", 0, "1", 1, 0, 0},
          {test::predicate()}};
      postcheck(db, "10", expected);
    }

    // External command will generate host alert and host notification.
    tpoints.store();
    engine.extcmd().execute("PROCESS_HOST_CHECK_RESULT;1;1;output");
    test::sleep_for(2);
    tpoints.store();

    // Check host alert.
    {
      precheck("host alert");
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 1, "", "",
           "output", 1, "", test::predicate(test::predicate::type_null), 1, 1},
          {test::predicate()}};
      postcheck(db, "1", expected);
    }

    // Check host notification.
    {
      precheck("host notification");
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 3,
           "test_command", "test_contact", "output", 0, "",
           test::predicate(test::predicate::type_null), 1, 0},
          {test::predicate()}};
      postcheck(db, "3", expected);
    }

    // Check host acknowledgement.
    {
      precheck("host acknowledgement");
      engine.extcmd().execute("ACKNOWLEDGE_HOST_PROBLEM;1;0;0;1;admin;comment");
      test::sleep_for(2);
      test::predicate expected[][14] = {
          {test::predicate(tpoints.prelast(), tpoints.last() + 1), 1, "1",
           "my-poller", test::predicate(test::predicate::type_null), 11, "",
           "admin", "comment", 0, "",
           test::predicate(test::predicate::type_null), 0, 0},
          {test::predicate()}};
      postcheck(db, "11", expected);
    }

    // Check other (external command).
    {
      precheck("other");
      QSqlQuery q(*db.get_db());
      if (!q.exec("SELECT COUNT(*) FROM logs WHERE msg_type=5") || !q.next() ||
          (q.value(0).toInt() < 4))
        throw(exceptions::msg()
              << "not enough 'other' entries in logs table (msg_type 5): got "
              << q.value(0).toInt() << ", expected at least 4");
      std::cout << "  passed\n";
    }

    // Success.
    error = false;
    db.set_remove_db_on_close(true);
    broker.stop();
  } catch (std::exception const& e) {
    std::cout << "  " << e.what() << "\n";
  } catch (...) {
    std::cout << "  unknown exception\n";
  }

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
