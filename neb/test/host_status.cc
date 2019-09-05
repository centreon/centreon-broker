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

#include <cmath>
#include <gtest/gtest.h>
#include "com/centreon/broker/neb/host_status.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class HostStatusTest : public ::testing::Test {
 public:
  void SetUp() {
    // Initialization.
    randomize_init();
  }

  void TearDown() {
    // Cleanup.
    randomize_cleanup();
  }
};

TEST_F(HostStatusTest, Assignment) {
  // Object #1.
  neb::host_status hs1;
  std::vector<randval> randvals1;
  randomize(hs1, &randvals1);

  // Object #2.
  neb::host_status hs2;
  randomize(hs2);

  // Assignment.
  hs2 = hs1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hs1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hs1 == randvals2);
  ASSERT_TRUE(hs2 == randvals1);
}

TEST_F(HostStatusTest, CopyConstructor) {
  // Object #1.
  neb::host_status hs1;
  std::vector<randval> randvals1;
  randomize(hs1, &randvals1);

  // Object #2.
  neb::host_status hs2(hs1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(hs1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(hs1 == randvals2);
  ASSERT_TRUE(hs2 == randvals1);
}

TEST_F(HostStatusTest, DefaultConstructor) {
  // Object.
  neb::host_status hs;

  // Check.
  ASSERT_FALSE((hs.source_id != 0));
  ASSERT_FALSE((hs.destination_id != 0));
  ASSERT_FALSE(hs.acknowledged);
  ASSERT_FALSE((hs.acknowledgement_type != 0));
  ASSERT_FALSE(hs.active_checks_enabled);
  ASSERT_FALSE(!hs.check_command.empty());
  ASSERT_FALSE((fabs(hs.check_interval) > 0.0001));
  ASSERT_FALSE(!hs.check_period.empty());
  ASSERT_FALSE((hs.check_type != 0));
  ASSERT_FALSE((hs.current_check_attempt != 0));
  ASSERT_FALSE((hs.current_state != 4));
  ASSERT_FALSE((hs.downtime_depth != 0));
  ASSERT_FALSE(!hs.enabled);
  ASSERT_FALSE(!hs.event_handler.empty());
  ASSERT_FALSE(hs.event_handler_enabled);
  ASSERT_FALSE((fabs(hs.execution_time) > 0.0001));
  ASSERT_FALSE(hs.flap_detection_enabled);
  ASSERT_FALSE(hs.has_been_checked);
  ASSERT_FALSE((hs.host_id != 0));
  ASSERT_FALSE(hs.is_flapping);
  ASSERT_FALSE((hs.last_check != 0));
  ASSERT_FALSE((hs.last_hard_state != 4));
  ASSERT_FALSE((hs.last_hard_state_change != 0));
  ASSERT_FALSE((hs.last_notification != 0));
  ASSERT_FALSE((hs.last_state_change != 0));
  ASSERT_FALSE((hs.last_time_down != 0));
  ASSERT_FALSE((hs.last_time_unreachable != 0));
  ASSERT_FALSE((hs.last_time_up != 0));
  ASSERT_FALSE((hs.last_update != 0));
  ASSERT_FALSE((fabs(hs.latency) > 0.0001));
  ASSERT_FALSE((hs.max_check_attempts != 0));
  ASSERT_FALSE((hs.next_check != 0));
  ASSERT_FALSE((hs.next_notification != 0));
  ASSERT_FALSE(hs.no_more_notifications);
  ASSERT_FALSE((hs.notification_number != 0));
  ASSERT_FALSE(hs.notifications_enabled);
  ASSERT_FALSE(hs.obsess_over);
  ASSERT_FALSE(!hs.output.empty());
  ASSERT_FALSE(hs.passive_checks_enabled);
  ASSERT_FALSE((fabs(hs.percent_state_change) > 0.0001));
  ASSERT_FALSE(!hs.perf_data.empty());
  ASSERT_FALSE((fabs(hs.retry_interval) > 0.0001));
  ASSERT_FALSE(hs.should_be_scheduled);
  ASSERT_FALSE((hs.state_type != 0));
}
