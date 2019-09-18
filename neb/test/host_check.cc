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

#include <gtest/gtest.h>
#include <cmath>
#include "com/centreon/broker/neb/host.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class HostCheckTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    randomize_init();
  }

  void TearDown() override {
    // Cleanup.
    randomize_cleanup();
  }
};

TEST_F(HostCheckTest, Assignment) {
  // Object #1.
  neb::host_check hchk1;
  std::vector<randval> randvals1;
  randomize(hchk1, &randvals1);

  // Object #2.
  neb::host_check hchk2;
  randomize(hchk2);

  // Assignment.
  hchk2 = hchk1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hchk1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hchk1 == randvals2);
  ASSERT_TRUE(hchk2 == randvals1);
}

TEST_F(HostCheckTest, CopyConstructor) {
  // Object #1.
  neb::host_check hchk1;
  std::vector<randval> randvals1;
  randomize(hchk1, &randvals1);

  // Object #2.
  neb::host_check hchk2(hchk1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hchk1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hchk1 == randvals2);
  ASSERT_TRUE(hchk2 == randvals1);
}

TEST_F(HostCheckTest, DefaultConstructor) {
  // Object.
  neb::host_check hchk;

  // Check.
  ASSERT_EQ(hchk.source_id, 0u);
  ASSERT_EQ(hchk.destination_id, 0u);
  ASSERT_FALSE(hchk.active_checks_enabled);
  ASSERT_TRUE(hchk.command_line.empty());
  ASSERT_FALSE((hchk.host_id != 0u));
  ASSERT_FALSE((hchk.next_check != 0));
  ASSERT_FALSE(
      (hchk.type() !=
       (io::events::data_type<io::events::neb, neb::de_host_check>::value)));
}
