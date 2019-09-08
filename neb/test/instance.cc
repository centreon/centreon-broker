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

#include "com/centreon/broker/neb/instance.hh"
#include <gtest/gtest.h>
#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class Instance : public ::testing::Test {
  void SetUp() override { randomize_init(); };

  void TearDown() override { randomize_cleanup(); };
};

/**
 *  Check instance's assignment operator.
 */
TEST_F(Instance, Assign) {
  // Object #1.
  neb::instance i1;
  std::vector<randval> randvals1;
  randomize(i1, &randvals1);

  // Object #2.
  neb::instance i2;
  randomize(i2);

  // Assignment.
  i2 = i1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(i1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(i1 != randvals2);
  ASSERT_FALSE(i2 != randvals1);
}

/**
 *  Check instance's copy constructor.
 */
TEST_F(Instance, CopyCtor) {
  // Object #1.
  neb::instance i1;
  std::vector<randval> randvals1;
  randomize(i1, &randvals1);

  // Object #2.
  neb::instance i2(i1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(i1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(i1 != randvals2);
  ASSERT_FALSE(i2 != randvals1);
}

/**
 *  Check instance's default constructor.
 */
TEST_F(Instance, DefaultCtor) {
  // Object.
  neb::instance i;

  auto val(io::events::data_type<io::events::neb, neb::de_instance>::value);

  // Check.
  ASSERT_FALSE(i.source_id != 0);
  ASSERT_FALSE(i.destination_id != 0);
  ASSERT_FALSE(i.engine != "");
  ASSERT_FALSE(i.is_running != true);
  ASSERT_FALSE(i.name != "");
  ASSERT_FALSE(i.pid != 0);
  ASSERT_FALSE(i.program_end != (time_t)-1);
  ASSERT_FALSE(i.program_start != (time_t)-1);
  ASSERT_FALSE(i.version != "");
  ASSERT_FALSE(i.type() != val);
}
