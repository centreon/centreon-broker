/*
** Copyright 2012-2019 Centreon
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

#include <cmath>
#include <gtest/gtest.h>
#include "com/centreon/broker/neb/host.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class HostTest : public ::testing::Test {
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

TEST_F(HostTest, Assignment) {
  // Object #1.
  neb::host h1;
  std::vector<randval> randvals1;
  randomize(h1, &randvals1);

  // Object #2.
  neb::host h2;
  randomize(h2);

  // Assignment.
  h2 = h1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(h1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(h1 == randvals2);
  ASSERT_TRUE(h2 == randvals1);
}

TEST_F(HostTest, CopyConstructor) {
  // Object #1.
  neb::host h1;
  std::vector<randval> randvals1;
  randomize(h1, &randvals1);

  // Object #2.
  neb::host h2(h1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(h1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(h1 == randvals2);
  ASSERT_TRUE(h2 == randvals1);
}

TEST_F(HostTest, DefaultConstructor) {
  // Object.
  neb::host h;

  // Check.
  ASSERT_EQ(h.source_id, 0);
  ASSERT_EQ(h.destination_id, 0);
  ASSERT_FALSE(h.acknowledged);
  ASSERT_EQ(h.acknowledgement_type, 0);
  ASSERT_TRUE(h.action_url.empty());
  ASSERT_FALSE(h.active_checks_enabled);
  ASSERT_TRUE(h.address.empty());
  ASSERT_TRUE(h.alias.empty());
  ASSERT_TRUE(h.check_command.empty());
  ASSERT_FALSE((fabs(h.check_interval) > 0.0001));
  ASSERT_FALSE(h.check_freshness);
  ASSERT_FALSE(!h.check_period.empty());
  ASSERT_FALSE((h.check_type != 0));
  ASSERT_FALSE((h.current_check_attempt != 0));
  ASSERT_FALSE((h.current_state != 4));
  ASSERT_FALSE(h.default_active_checks_enabled);
  ASSERT_FALSE(h.default_event_handler_enabled);
  ASSERT_FALSE(h.default_flap_detection_enabled);
  ASSERT_FALSE(h.default_notifications_enabled);
  ASSERT_FALSE(h.default_passive_checks_enabled);
  ASSERT_FALSE((h.downtime_depth != 0));
  ASSERT_FALSE(!h.enabled);
  ASSERT_FALSE(!h.event_handler.empty());
  ASSERT_FALSE(h.event_handler_enabled);
  ASSERT_FALSE((fabs(h.execution_time) > 0.0001));
  ASSERT_FALSE((fabs(h.first_notification_delay) > 0.0001));
  ASSERT_FALSE(h.flap_detection_enabled);
  ASSERT_FALSE(h.flap_detection_on_down);
  ASSERT_FALSE(h.flap_detection_on_unreachable);
  ASSERT_FALSE(h.flap_detection_on_up);
  ASSERT_FALSE((fabs(h.freshness_threshold) > 0.0001));
  ASSERT_FALSE(h.has_been_checked);
  ASSERT_FALSE((fabs(h.high_flap_threshold) > 0.0001));
  ASSERT_FALSE((h.host_id != 0));
  ASSERT_FALSE(!h.host_name.empty());
  ASSERT_FALSE(!h.icon_image.empty());
  ASSERT_FALSE(!h.icon_image_alt.empty());
  ASSERT_FALSE(h.is_flapping);
  ASSERT_FALSE((h.last_check != 0));
  ASSERT_FALSE((h.last_hard_state != 4));
  ASSERT_FALSE((h.last_hard_state_change != 0));
  ASSERT_FALSE((h.last_notification != 0));
  ASSERT_FALSE((h.last_state_change != 0));
  ASSERT_FALSE((h.last_time_down != 0));
  ASSERT_FALSE((h.last_time_unreachable != 0));
  ASSERT_FALSE((h.last_time_up != 0));
  ASSERT_FALSE((h.last_update != 0));
  ASSERT_FALSE((fabs(h.latency) > 0.0001));
  ASSERT_FALSE((fabs(h.low_flap_threshold) > 0.0001));
  ASSERT_FALSE((h.max_check_attempts != 0));
  ASSERT_FALSE((h.next_check != 0));
  ASSERT_FALSE((h.next_notification != 0));
  ASSERT_FALSE(h.no_more_notifications);
  ASSERT_FALSE(!h.notes.empty());
  ASSERT_FALSE(!h.notes_url.empty());
  ASSERT_FALSE((h.notification_number != 0));
  ASSERT_FALSE(h.notifications_enabled);
  ASSERT_FALSE((fabs(h.notification_interval) > 0.0001));
  ASSERT_FALSE(!h.notification_period.empty());
  ASSERT_FALSE(h.notify_on_down);
  ASSERT_FALSE(h.notify_on_downtime);
  ASSERT_FALSE(h.notify_on_flapping);
  ASSERT_FALSE(h.notify_on_recovery);
  ASSERT_FALSE(h.notify_on_unreachable);
  ASSERT_FALSE(h.obsess_over);
  ASSERT_FALSE(!h.output.empty());
  ASSERT_FALSE(h.passive_checks_enabled);
  ASSERT_FALSE((fabs(h.percent_state_change) > 0.0001));
  ASSERT_FALSE(!h.perf_data.empty());
  ASSERT_FALSE(h.retain_nonstatus_information);
  ASSERT_FALSE(h.retain_status_information);
  ASSERT_FALSE((fabs(h.retry_interval) > 0.0001));
  ASSERT_FALSE(h.should_be_scheduled);
  ASSERT_FALSE(h.stalk_on_down);
  ASSERT_FALSE(h.stalk_on_unreachable);
  ASSERT_FALSE(h.stalk_on_up);
  ASSERT_FALSE((h.state_type != 0));
  ASSERT_FALSE(!h.statusmap_image.empty());
}
