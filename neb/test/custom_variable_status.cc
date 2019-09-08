/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/neb/custom_variable_status.hh"
#include <gtest/gtest.h>
#include "randomize.hh"

using namespace com::centreon::broker;

class CVarStatusTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    randomize_init();
  }

  void TearDown() override {
    // Cleanup.
    com::centreon::broker::randomize_cleanup();
  }
};

TEST_F(CVarStatusTest, Assignment) {
  // Object #1.
  neb::custom_variable_status cvar_status1;
  std::vector<randval> randvals1;
  randomize(cvar_status1, &randvals1);

  // Object #2.
  neb::custom_variable_status cvar_status2;
  randomize(cvar_status2);

  // Assignment.
  cvar_status2 = cvar_status1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(cvar_status1, &randvals2);

  // Compare objects with expected results.
  int retval((cvar_status1 != randvals2) || (cvar_status2 != randvals1));

  // Compare objects with expected results.
  ASSERT_TRUE(cvar_status1 == randvals2);
  ASSERT_TRUE(cvar_status2 == randvals1);
}

TEST_F(CVarStatusTest, CopyConstructor) {
  // Object #1.
  neb::custom_variable_status cvar_status1;
  std::vector<randval> randvals1;
  randomize(cvar_status1, &randvals1);

  // Object #2.
  neb::custom_variable_status cvar_status2(cvar_status1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(cvar_status1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(cvar_status1 == randvals2);
  ASSERT_TRUE(cvar_status2 == randvals1);
}

TEST_F(CVarStatusTest, DefaultConstructor) {
  // Object.
  neb::custom_variable_status cvar_status;

  // Check.
  ASSERT_EQ(cvar_status.source_id, 0);
  ASSERT_EQ(cvar_status.destination_id, 0);
  ASSERT_EQ(cvar_status.host_id, 0);
  ASSERT_EQ(cvar_status.modified, true);
  ASSERT_EQ(cvar_status.name, "");
  ASSERT_EQ(cvar_status.service_id, 0);
  ASSERT_EQ(cvar_status.update_time, 0);
  ASSERT_EQ(cvar_status.value, "");

  ASSERT_TRUE(cvar_status.type() ==
              (io::events::data_type<io::events::neb,
                                     neb::de_custom_variable_status>::value));
}
