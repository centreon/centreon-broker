/*
** Copyright 2009-2011,2015 Centreon
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
#include <cmath>
#include <cstdlib>
#include "com/centreon/broker/neb/service_status.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class ServiceStatus : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};

/**
 *  Check service_status' assignment operator.
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(ServiceStatus, Assign) {
  // Object #1.
  neb::service_status ss1;
  std::vector<randval> randvals1;
  randomize(ss1, &randvals1);

  // Object #2.
  neb::service_status ss2;
  randomize(ss2);

  // Assignment.
  ss2 = ss1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(ss1, &randvals2);

  ASSERT_FALSE(ss1 != randvals2);
  ASSERT_FALSE(ss2 != randvals1);
}

/**
 *  Check service_status' copy constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST_F(ServiceStatus, CopyCtor) {
  // Object #1.
  neb::service_status ss1;
  std::vector<randval> randvals1;
  randomize(ss1, &randvals1);

  // Object #2.
  neb::service_status ss2(ss1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(ss1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(ss1 != randvals2);
  ASSERT_FALSE(ss2 != randvals1);
}


/**
 *  Check service_status' default constructor.
 */
TEST_F(ServiceStatus, DefaultCtor) {
  // Object.
  neb::service_status ss;

  // Check.
  ASSERT_FALSE((ss.source_id != 0));
  ASSERT_FALSE((ss.destination_id != 0));
  ASSERT_FALSE(ss.acknowledged);
  ASSERT_FALSE((ss.acknowledgement_type != 0));
  ASSERT_FALSE(ss.active_checks_enabled);
  ASSERT_FALSE(!ss.check_command.empty());
  ASSERT_FALSE((fabs(ss.check_interval) > 0.001));
  ASSERT_FALSE(!ss.check_period.empty());
  ASSERT_FALSE((ss.check_type != 0));
  ASSERT_FALSE((ss.current_check_attempt != 0));
  ASSERT_FALSE((ss.current_state != 4));
  ASSERT_FALSE((ss.downtime_depth != 0));
  ASSERT_FALSE(!ss.enabled);
  ASSERT_FALSE(!ss.event_handler.empty());
  ASSERT_FALSE(ss.event_handler_enabled);
  ASSERT_FALSE((fabs(ss.execution_time) > 0.001));
  ASSERT_FALSE(ss.flap_detection_enabled);
  ASSERT_FALSE(ss.has_been_checked);
  ASSERT_FALSE((ss.host_id != 0));
  ASSERT_FALSE(!ss.host_name.empty());
  ASSERT_FALSE(ss.is_flapping);
  ASSERT_FALSE((ss.last_check != 0));
  ASSERT_FALSE((ss.last_hard_state != 4));
  ASSERT_FALSE((ss.last_hard_state_change != 0));
  ASSERT_FALSE((ss.last_notification != 0));
  ASSERT_FALSE((ss.last_state_change != 0));
  ASSERT_FALSE((ss.last_time_critical != 0));
  ASSERT_FALSE((ss.last_time_ok != 0));
  ASSERT_FALSE((ss.last_time_unknown != 0));
  ASSERT_FALSE((ss.last_time_warning != 0));
  ASSERT_FALSE((ss.last_update != 0));
  ASSERT_FALSE((fabs(ss.latency) > 0.001));
  ASSERT_FALSE((ss.max_check_attempts != 0));
  ASSERT_FALSE((ss.next_check != 0));
  ASSERT_FALSE((ss.next_notification != 0));
  ASSERT_FALSE(ss.no_more_notifications);
  ASSERT_FALSE((ss.notification_number != 0));
  ASSERT_FALSE(ss.notifications_enabled);
  ASSERT_FALSE(ss.obsess_over);
  ASSERT_FALSE(!ss.output.empty());
  ASSERT_FALSE(ss.passive_checks_enabled);
  ASSERT_FALSE((fabs(ss.percent_state_change) > 0.001));
  ASSERT_FALSE(!ss.perf_data.empty());
  ASSERT_FALSE((fabs(ss.retry_interval) > 0.001));
  ASSERT_FALSE(ss.should_be_scheduled);
  ASSERT_FALSE((ss.state_type != 0));
  ASSERT_FALSE(!ss.service_description.empty());
  ASSERT_FALSE((ss.service_id != 0));
}

