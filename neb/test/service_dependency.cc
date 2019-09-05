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
#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_dependency.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class ServiceDependency : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};

/**
 *  Check service_dependency's assignment operator.
 */
TEST_F(ServiceDependency, Assign) {
  // Object #1.
  neb::service_dependency sdep1;
  std::vector<randval> randvals1;
  randomize(sdep1, &randvals1);

  // Object #2.
  neb::service_dependency sdep2;
  randomize(sdep2);

  // Assignment.
  sdep2 = sdep1;

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(sdep1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(sdep1 != randvals2);
  ASSERT_FALSE(sdep2 != randvals1);
}


/**
 *  Check service_dependency's copy constructor.
 */
TEST_F(ServiceDependency, CopyCtor) {
  // Object #1.
  neb::service_dependency sdep1;
  std::vector<randval> randvals1;
  randomize(sdep1, &randvals1);

  // Object #2.
  neb::service_dependency sdep2(sdep1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(sdep1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(sdep1 != randvals2);
  ASSERT_FALSE(sdep2 != randvals1);
}

/**
 *  Check service_dependency's default constructor.
 */
TEST_F(ServiceDependency, DefaultCtor) {
  // Object.
  neb::service_dependency sdep;

  // Check.
  ASSERT_FALSE(sdep.source_id != 0);
  ASSERT_FALSE(sdep.destination_id != 0);
  ASSERT_FALSE(sdep.dependency_period != "");
  ASSERT_FALSE(sdep.dependent_host_id != 0);
  ASSERT_FALSE(sdep.enabled != true);
  ASSERT_FALSE(!sdep.execution_failure_options.empty());
  ASSERT_FALSE(sdep.host_id != 0);
  ASSERT_FALSE(sdep.inherits_parent != false);
  ASSERT_FALSE(!sdep.notification_failure_options.empty());
  ASSERT_FALSE(sdep.service_id != 0);
  auto val(io::events::data_type<io::events::neb, neb::de_service_dependency>::value);
  ASSERT_FALSE(sdep.type() != val);
}
