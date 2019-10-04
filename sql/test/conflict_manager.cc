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
#include <list>
#include <memory>
#include "../../core/test/test_server.hh"
#include "com/centreon/broker/config/applier/init.hh"
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

TEST_F(ConflictManagerTest, Bidon) {
  database_config dbcfg;
  uint32_t loop_timeout = 5;
  uint32_t instance_timeout = 5;

  conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout);
}
