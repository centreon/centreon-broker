/*
** Copyright 2018 Centreon
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

#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include "../../core/test/test_server.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/module.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

class ConflictManagerTest : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }
  }
  void TearDown() override {
    config::applier::deinit();
  }
};

TEST_F(ConflictManagerTest, OpenClose) {
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "root", "root",
                        "centreon_storage", 5, true, 5);
  uint32_t loop_timeout = 5;
  uint32_t instance_timeout = 5;

  ASSERT_FALSE(conflict_manager::init_storage(true, 100000, 18));
  ASSERT_NO_THROW(
      conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout));
  ASSERT_TRUE(conflict_manager::init_storage(true, 100000, 18));
  conflict_manager::close();
}

TEST_F(ConflictManagerTest, InstanceStatement) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  uint32_t loop_timeout = 5;
  uint32_t instance_timeout = 5;
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "root", "root",
                         "centreon_storage", 5, true, 5);
  ASSERT_NO_THROW(
      conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout));

  std::shared_ptr<neb::instance> inst{std::make_shared<neb::instance>()};
  inst->poller_id = 1;
  inst->name = "Central";
  inst->program_start = time(nullptr) - 100;
  inst->program_end = time(nullptr) - 1;
  inst->version = "1.8.1";

  conflict_manager::instance().send_event(conflict_manager::sql, inst);

  std::shared_ptr<neb::instance> inst2{std::make_shared<neb::instance>()};
  inst2->poller_id = 2;
  inst2->name = "Central2";
  inst2->program_start = time(nullptr) - 100;
  inst2->program_end = time(nullptr) - 1;
  inst2->version = "1.8.1";

  conflict_manager::instance().send_event(conflict_manager::sql, inst2);

  conflict_manager::close();
}
