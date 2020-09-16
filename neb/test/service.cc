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

#include "com/centreon/broker/neb/service.hh"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdlib>

#include "com/centreon/broker/database/table_max_size.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class Service : public ::testing::Test {
  void SetUp() override { randomize_init(); };

  void TearDown() override { randomize_cleanup(); };
};

/**
 *  Check service's assignment operator.
 */
TEST_F(Service, Assign) {
  // Object #1.
  neb::service s1;
  std::vector<randval> randvals1;
  randomize(s1, &randvals1);

  // Object #2.
  neb::service s2;
  randomize(s2);

  // Assignment.
  s2 = s1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(s1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(s1 != randvals2);
  ASSERT_FALSE(s2 != randvals1);
}

/**
 *  Check service's copy constructor.
 */
TEST_F(Service, CopyCtor) {
  // Object #1.
  neb::service s1;
  std::vector<randval> randvals1;
  randomize(s1, &randvals1);

  // Object #2.
  neb::service s2(s1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(s1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(s1 != randvals2);
  ASSERT_FALSE(s2 != randvals1);
}

/**
 *  Check service's default constructor.
 */
TEST_F(Service, DefaultCtor) {
  // Object.
  neb::service s;

  // Check.
  ASSERT_FALSE(s.source_id != 0);
  ASSERT_FALSE(s.destination_id != 0);
  ASSERT_FALSE(s.acknowledged);
  ASSERT_FALSE(s.acknowledgement_type != 0);
  ASSERT_FALSE(!s.action_url.empty());
  ASSERT_FALSE(s.active_checks_enabled);
  ASSERT_FALSE(!s.check_command.empty());
  ASSERT_FALSE(s.check_freshness);
  ASSERT_FALSE(fabs(s.check_interval) > 0.001);
  ASSERT_FALSE(!s.check_period.empty());
  ASSERT_FALSE(s.check_type != 0);
  ASSERT_FALSE(s.current_check_attempt != 0);
  ASSERT_FALSE(s.current_state != 4);
  ASSERT_FALSE(s.default_active_checks_enabled);
  ASSERT_FALSE(s.default_event_handler_enabled);
  ASSERT_FALSE(s.default_flap_detection_enabled);
  ASSERT_FALSE(s.default_notifications_enabled);
  ASSERT_FALSE(s.default_passive_checks_enabled);
  ASSERT_FALSE(s.downtime_depth != 0);
  ASSERT_FALSE(!s.enabled);
  ASSERT_FALSE(!s.event_handler.empty());
  ASSERT_FALSE(s.event_handler_enabled);
  ASSERT_FALSE(fabs(s.execution_time) > 0.001);
  ASSERT_FALSE(fabs(s.first_notification_delay) > 0.0001);
  ASSERT_FALSE(s.flap_detection_enabled);
  ASSERT_FALSE(s.flap_detection_on_critical);
  ASSERT_FALSE(s.flap_detection_on_ok);
  ASSERT_FALSE(s.flap_detection_on_unknown);
  ASSERT_FALSE(s.flap_detection_on_warning);
  ASSERT_FALSE(fabs(s.freshness_threshold) > 0.001);
  ASSERT_FALSE(s.has_been_checked);
  ASSERT_FALSE(fabs(s.high_flap_threshold) > 0.001);
  ASSERT_FALSE(s.host_id != 0);
  ASSERT_FALSE(!s.host_name.empty());
  ASSERT_FALSE(!s.icon_image.empty());
  ASSERT_FALSE(!s.icon_image_alt.empty());
  ASSERT_FALSE(s.is_flapping);
  ASSERT_FALSE(s.is_volatile);
  ASSERT_FALSE(s.last_check != 0);
  ASSERT_FALSE(s.last_hard_state != 4);
  ASSERT_FALSE(s.last_hard_state_change != 0);
  ASSERT_FALSE(s.last_notification != 0);
  ASSERT_FALSE(s.last_state_change != 0);
  ASSERT_FALSE(s.last_time_critical != 0);
  ASSERT_FALSE(s.last_time_ok != 0);
  ASSERT_FALSE(s.last_time_unknown != 0);
  ASSERT_FALSE(s.last_time_warning != 0);
  ASSERT_FALSE(s.last_update != 0);
  ASSERT_FALSE(fabs(s.latency) > 0.001);
  ASSERT_FALSE(fabs(s.low_flap_threshold) > 0.001);
  ASSERT_FALSE(s.max_check_attempts != 0);
  ASSERT_FALSE(s.next_check != 0);
  ASSERT_FALSE(s.next_notification != 0);
  ASSERT_FALSE(s.no_more_notifications);
  ASSERT_FALSE(!s.notes.empty());
  ASSERT_FALSE(!s.notes_url.empty());
  ASSERT_FALSE(s.notifications_enabled);
  ASSERT_FALSE(fabs(s.notification_interval) > 0.0001);
  ASSERT_FALSE(s.notification_number != 0);
  ASSERT_FALSE(!s.notification_period.empty());
  ASSERT_FALSE(s.notify_on_critical);
  ASSERT_FALSE(s.notify_on_downtime);
  ASSERT_FALSE(s.notify_on_flapping);
  ASSERT_FALSE(s.notify_on_recovery);
  ASSERT_FALSE(s.notify_on_unknown);
  ASSERT_FALSE(s.notify_on_warning);
  ASSERT_FALSE(s.obsess_over);
  ASSERT_FALSE(!s.output.empty());
  ASSERT_FALSE(s.passive_checks_enabled);
  ASSERT_FALSE(fabs(s.percent_state_change) > 0.001);
  ASSERT_FALSE(!s.perf_data.empty());
  ASSERT_FALSE(s.retain_nonstatus_information);
  ASSERT_FALSE(s.retain_status_information);
  ASSERT_FALSE(fabs(s.retry_interval) > 0.001);
  ASSERT_FALSE(s.should_be_scheduled);
  ASSERT_FALSE(s.stalk_on_critical);
  ASSERT_FALSE(s.stalk_on_ok);
  ASSERT_FALSE(s.stalk_on_unknown);
  ASSERT_FALSE(s.stalk_on_warning);
  ASSERT_FALSE(s.state_type != 0);
  ASSERT_FALSE(!s.service_description.empty());
  ASSERT_FALSE(s.service_id != 0);

  /* action_url has a max size */
  const mapping::entry& entry(neb::service::entries[2]);
  ASSERT_EQ(std::string(entry.get_name_v2()), "action_url");
  ASSERT_EQ(entry.get_type(), mapping::source::STRING);
  size_t max_len;
  std::string str(entry.get_string(s, &max_len));
  ASSERT_EQ(max_len, get_services_col_size(services_action_url));
  ASSERT_TRUE(str.empty());
}
