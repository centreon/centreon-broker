/*
** Copyright 2012-2013,2015 Centreon
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
#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/instance_status.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class InstanceStatus : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};


/**
 *  Check instance_status's assignment operator.
 */
TEST_F(InstanceStatus, Assign) {
  // Object #1.
  neb::instance_status is1;
  std::vector<randval> randvals1;
  randomize(is1, &randvals1);

  // Object #2.
  neb::instance_status is2;
  randomize(is2);

  // Assignment.
  is2 = is1;

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(is1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(is1 != randvals2);
  ASSERT_FALSE(is2 != randvals1);
}


/**
 *  Check instance_status's copy constructor.
 */
TEST_F(InstanceStatus, CopyCtor) {
  // Object #1.
  neb::instance_status is1;
  std::vector<randval> randvals1;
  randomize(is1, &randvals1);

  // Object #2.
  neb::instance_status is2(is1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(is1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(is1 != randvals2);
  ASSERT_FALSE(is2 != randvals1);
}

/**
 *  Check instance_status's default constructor.
 */
TEST_F(InstanceStatus, DefaultCtor) {
  // Object.
  neb::instance_status is;

  auto val(io::events::data_type<io::events::neb,neb::de_instance_status>::value);

  // Check.
  ASSERT_FALSE(is.source_id != 0);
  ASSERT_FALSE(is.destination_id != 0);
  ASSERT_FALSE(is.active_host_checks_enabled != false);
  ASSERT_FALSE(is.active_service_checks_enabled != false);
  ASSERT_FALSE(is.check_hosts_freshness != false);
  ASSERT_FALSE(is.check_services_freshness != false);
  ASSERT_FALSE(is.event_handler_enabled != false);
  ASSERT_FALSE(is.flap_detection_enabled != false);
  ASSERT_FALSE(is.global_host_event_handler != "");
  ASSERT_FALSE(is.global_service_event_handler != "");
  ASSERT_FALSE(is.last_alive != (time_t)-1);
  ASSERT_FALSE(is.last_command_check != (time_t)-1);
  ASSERT_FALSE(is.notifications_enabled != false);
  ASSERT_FALSE(is.obsess_over_hosts != false);
  ASSERT_FALSE(is.obsess_over_services != false);
  ASSERT_FALSE(is.passive_host_checks_enabled != false);
  ASSERT_FALSE(is.passive_service_checks_enabled != false);
  ASSERT_FALSE(is.type() != val);
}
