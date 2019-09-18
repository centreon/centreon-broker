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

class HostDependencyTest : public ::testing::Test {
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

TEST_F(HostDependencyTest, Assignment) {
  // Object #1.
  neb::host_dependency hdep1;
  std::vector<randval> randvals1;
  randomize(hdep1, &randvals1);

  // Object #2.
  neb::host_dependency hdep2;
  randomize(hdep2);

  // Assignment.
  hdep2 = hdep1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hdep1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hdep1 == randvals2);
  ASSERT_TRUE(hdep2 == randvals1);
}

TEST_F(HostDependencyTest, CopyConstructor) {
  // Object #1.
  neb::host_dependency hdep1;
  std::vector<randval> randvals1;
  randomize(hdep1, &randvals1);

  // Object #2.
  neb::host_dependency hdep2(hdep1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hdep1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hdep1 == randvals2);
  ASSERT_TRUE(hdep2 == randvals1);
}

TEST_F(HostDependencyTest, DefaultConstructor) {
  // Object.
  neb::host_dependency hdep;

  // Check.
  ASSERT_EQ(hdep.source_id, 0u);
  ASSERT_EQ(hdep.destination_id, 0u);
  ASSERT_TRUE(hdep.dependency_period == "");
  ASSERT_EQ(hdep.dependent_host_id, 0u);
  ASSERT_TRUE(hdep.enabled);
  ASSERT_TRUE(hdep.execution_failure_options.empty());
  ASSERT_EQ(hdep.host_id, 0u);
  ASSERT_FALSE(hdep.inherits_parent);
  ASSERT_TRUE(hdep.notification_failure_options.empty());
  ASSERT_FALSE(
      hdep.type() !=
      (io::events::data_type<io::events::neb, neb::de_host_dependency>::value));
}
