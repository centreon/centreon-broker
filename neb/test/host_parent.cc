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

#include "com/centreon/broker/neb/host_parent.hh"
#include <gtest/gtest.h>
#include <cmath>
#include "randomize.hh"

using namespace com::centreon::broker;

class HostParentTest : public ::testing::Test {
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

TEST_F(HostParentTest, Assignment) {
  // Object #1.
  neb::host_parent hprnt1;
  std::vector<randval> randvals1;
  randomize(hprnt1, &randvals1);

  // Object #2.
  neb::host_parent hprnt2;
  randomize(hprnt2);

  // Assignment.
  hprnt2 = hprnt1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hprnt1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hprnt1 == randvals2);
  ASSERT_TRUE(hprnt2 == randvals1);
}

TEST_F(HostParentTest, CopyConstructor) {
  // Object #1.
  neb::host_parent hprnt1;
  std::vector<randval> randvals1;
  randomize(hprnt1, &randvals1);

  // Object #2.
  neb::host_parent hprnt2(hprnt1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hprnt1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hprnt1 == randvals2);
  ASSERT_TRUE(hprnt2 == randvals1);
}

TEST_F(HostParentTest, DefaultConstructor) {
  // Object.
  neb::host_parent hprnt;

  // Check.
  ASSERT_EQ(hprnt.source_id, 0);
  ASSERT_EQ(hprnt.destination_id, 0);
  ASSERT_TRUE(hprnt.enabled);
  ASSERT_EQ(hprnt.host_id, 0);
  ASSERT_EQ(hprnt.parent_id, 0);
  ASSERT_FALSE(
      hprnt.type() !=
      (io::events::data_type<io::events::neb, neb::de_host_parent>::value));
}
