/*
** Copyright 2012-2013 Centreon
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
#include "com/centreon/broker/neb/service_check.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class ServiceCheck : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};

/**
 *  Check service_check's assignment operator.
 */
TEST_F(ServiceCheck, Assign) {
  // Object #1.
  neb::service_check schk1;
  std::vector<randval> randvals1;
  randomize(schk1, &randvals1);

  // Object #2.
  neb::service_check schk2;
  randomize(schk2);

  // Assignment.
  schk2 = schk1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(schk1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(schk1 != randvals2);
  ASSERT_FALSE(schk2 != randvals1);
}

/**
 *  Check service_check's copy constructor.
 */
TEST_F(ServiceCheck, CopyCtor) {
  // Object #1.
  neb::service_check schk1;
  std::vector<randval> randvals1;
  randomize(schk1, &randvals1);

  // Object #2.
  neb::service_check schk2(schk1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(schk1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(schk1 != randvals2);
  ASSERT_FALSE(schk2 != randvals1);
}

/**
 *  Check service_check's default constructor.
 */
TEST_F(ServiceCheck, DefaultCtor) {
  // Object.
  neb::service_check schk;

  // Check.
  ASSERT_FALSE(schk.source_id != 0);
  ASSERT_FALSE(schk.destination_id != 0);
  ASSERT_FALSE(schk.active_checks_enabled);
  ASSERT_FALSE(schk.command_line != "");
  ASSERT_FALSE(schk.host_id != 0);
  ASSERT_FALSE(schk.next_check != 0);
  ASSERT_FALSE(schk.service_id != 0);
  auto val(io::events::data_type<io::events::neb, neb::de_service_check>::value);
  ASSERT_FALSE(schk.type() != val);
}
