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

#include <cstdlib>
#include <iostream>
#include <sstream>
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

#define TEST_NAME "rtmonitoring_v2_hostgroups"
#define DB_NAME "broker_" TEST_NAME

// Check count.
static int check_number(0);

/**
 *  Precheck routine.
 */
static void precheck(test::time_points& tpoints, char const* name) {
  ++check_number;
  std::cout << "check #" << check_number << " (" << name << ")\n";
  tpoints.store();
  return;
}

/**
 *  Postcheck routine.
 */
static void postcheck(test::centengine& engine,
                      test::time_points& tpoints,
                      test::db& db,
                      test::predicate expected_groups[][2],
                      test::predicate expected_members[][2]) {
  static std::string group_query(
      "SELECT hostgroup_id, name"
      "  FROM hostgroups"
      "  ORDER BY hostgroup_id ASC");
  static std::string member_query(
      "SELECT hostgroup_id, host_id"
      "  FROM hosts_hostgroups"
      "  ORDER BY hostgroup_id ASC, host_id ASC");
  engine.reload();
  test::sleep_for(3);
  tpoints.store();
  db.check_content(group_query, expected_groups);
  db.check_content(member_query, expected_members);
  std::cout << "  passed\n";
  return;
}

/**
 *  Check that the hostgroups and the hosts_hostgroups tables are
 *  working properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(true);

  try {
    // Database.
    char const* tables[] = {"instances", "hosts", "hostgroups",
                            "hosts_hostgroups", NULL};
    test::db db(DB_NAME, tables);

    // Monitoring broker.
    test::file cbd_cfg;
    cbd_cfg.set_template(PROJECT_SOURCE_DIR "/test/cfg/sql.xml.in");
    cbd_cfg.set("BROKER_ID", "84");
    cbd_cfg.set("BROKER_NAME", TEST_NAME "-cbd");
    cbd_cfg.set("POLLER_ID", "42");
    cbd_cfg.set("POLLER_NAME", "my-poller");
    cbd_cfg.set("TCP_PORT", "5573");
    cbd_cfg.set("DB_NAME", DB_NAME);
    cbd_cfg.set("SQL_ADDITIONAL",
                "<write_filters>"
                "  <category>neb:instance</category>"
                "  <category>neb:instance_status</category>"
                "  <category>neb:host</category>"
                "  <category>neb:host_check</category>"
                "  <category>neb:host_status</category>"
                "  <category>neb:host_group</category>"
                "  <category>neb:host_group_member</category>"
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
    cbmod_cfg.set("TCP_PORT", "5573");
    test::centengine_config engine_config;
    engine_config.generate_hosts(10);
    engine_config.generate_services(2);
    for (int i(0); i < 5; ++i) {
      test::centengine_object group(test::centengine_object::hostgroup_type);
      {
        std::ostringstream group_name;
        group_name << "group" << i + 1;
        group.set("hostgroup_name", group_name.str());
      }
      {
        std::ostringstream group_id;
        group_id << i + 1;
        group.set("hostgroup_id", group_id.str());
      }
      {
        std::ostringstream members;
        members << i * 2 + 1 << "," << i * 2 + 2;
        group.set("members", members.str());
      }
      engine_config.get_host_groups().push_back(group);
    }
    engine_config.set_cbmod_cfg_file(cbmod_cfg.generate());
    test::centengine engine(&engine_config);

    // Time points.
    test::time_points tpoints;

    // Check default entries.
    precheck(tpoints, "hostgroups.insert");
    engine.start();
    test::sleep_for(1);
    test::predicate expected_groups[][2] = {{1, "group1"}, {2, "group2"},
                                            {3, "group3"}, {4, "group4"},
                                            {5, "group5"}, {test::predicate()}};
    test::predicate expected_members[20][2] = {{1, 1},
                                               {1, 2},
                                               {2, 3},
                                               {2, 4},
                                               {3, 5},
                                               {3, 6},
                                               {4, 7},
                                               {4, 8},
                                               {5, 9},
                                               {5, 10},
                                               {test::predicate()}};
    postcheck(engine, tpoints, db, expected_groups, expected_members);

    // Change host group name.
    precheck(tpoints, "hostgroups.update");
    test::centengine_object& hg(engine_config.get_host_groups().front());
    hg.set("hostgroup_name", "renamed");
    expected_groups[0][1] = "renamed";
    postcheck(engine, tpoints, db, expected_groups, expected_members);

    // Add members.
    precheck(tpoints, "hosts_hostgroups.insert");
    hg.set("members", "1,3,5,7,9");
    // The first 5 entries will be the members of group 1. All other
    // entries will remain valid but have to be moved at the end of the
    // array.
    for (int i(7); i >= 0; --i) {
      expected_members[i + 5][0] = expected_members[i + 2][0];
      expected_members[i + 5][1] = expected_members[i + 2][1];
    }
    for (int i(0); i < 5; ++i) {
      expected_members[i][0] = 1;
      expected_members[i][1] = i * 2 + 1;
    }
    expected_members[13][0] = test::predicate();
    postcheck(engine, tpoints, db, expected_groups, expected_members);

    // Remove members.
    precheck(tpoints, "hosts_hostgroups.delete");
    hg.set("members", "5");
    // Move entries.
    for (int i(0); i < 8; ++i) {
      expected_members[i + 1][0] = expected_members[i + 5][0];
      expected_members[i + 1][1] = expected_members[i + 5][1];
    }
    expected_members[0][0] = 1;
    expected_members[0][1] = 5;
    expected_members[9][0] = test::predicate();
    postcheck(engine, tpoints, db, expected_groups, expected_members);

    // Remove all members.
    precheck(tpoints, "hostgroups.delete");
    for (test::centengine_config::objlist::iterator
             it(engine_config.get_host_groups().begin()),
         end(engine_config.get_host_groups().end());
         it != end; ++it)
      it->set("members", "");
    expected_groups[0][0] = test::predicate();
    expected_members[0][0] = test::predicate();
    postcheck(engine, tpoints, db, expected_groups, expected_members);

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
