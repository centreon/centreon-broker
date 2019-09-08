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
#include "com/centreon/broker/neb/event_handler.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class FlappingStatus : public ::testing::Test {
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

TEST_F(FlappingStatus, Assignment) {
  // Object #1.
  neb::flapping_status flappy1;
  std::vector<randval> randvals1;
  randomize(flappy1, &randvals1);

  // Object #2.
  neb::flapping_status flappy2;
  randomize(flappy2);

  // Assignment.
  flappy2 = flappy1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(flappy1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(flappy1 == randvals2);
  ASSERT_TRUE(flappy2 == randvals1);
}

TEST_F(FlappingStatus, CopyConstructor) {
  // Object #1.
  neb::flapping_status flappy1;
  std::vector<randval> randvals1;
  randomize(flappy1, &randvals1);

  // Object #2.
  neb::flapping_status flappy2(flappy1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(flappy1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(flappy1 == randvals2);
  ASSERT_TRUE(flappy2 == randvals1);
}

TEST_F(FlappingStatus, DefaultConstructor) {
  // Object.
  neb::flapping_status flappy;

  // Check.
  ASSERT_EQ(flappy.source_id, 0);
  ASSERT_EQ(flappy.destination_id, 0);
  ASSERT_EQ(flappy.event_time, 0);
  ASSERT_EQ(flappy.event_type, 0);
  ASSERT_EQ(flappy.flapping_type, 0);
  ASSERT_TRUE(fabs(flappy.high_threshold) < 0.000001);
  ASSERT_EQ(flappy.host_id, 0);
  ASSERT_TRUE(fabs(flappy.low_threshold) < 0.000001);
  ASSERT_TRUE(fabs(flappy.percent_state_change) < 0.000001);
  ASSERT_EQ(flappy.reason_type, 0);
  ASSERT_EQ(flappy.service_id, 0);
  ASSERT_TRUE(
      flappy.type() ==
      (io::events::data_type<io::events::neb, neb::de_flapping_status>::value));
}
