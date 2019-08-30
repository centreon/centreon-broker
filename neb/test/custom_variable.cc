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

#include <gtest/gtest.h>
#include "com/centreon/broker/neb/custom_variable.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class CVarTest : public ::testing::Test {
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

TEST_F(CVarTest, Assignment) {
  // Object #1.
  neb::custom_variable cvar1;
  std::vector<randval> randvals1;
  randomize(cvar1, &randvals1);

  // Object #2.
  neb::custom_variable cvar2;
  randomize(cvar2);

  // Assignment.
  cvar2 = cvar1;

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(cvar1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(cvar1 == randvals2);
  ASSERT_TRUE(cvar2 == randvals1);
}

TEST_F(CVarTest, CopyConstructor) {
  // Object #1.
  neb::custom_variable cvar1;
  std::vector<randval> randvals1;
  randomize(cvar1, &randvals1);

  // Object #2.
  neb::custom_variable cvar2(cvar1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(cvar1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(cvar1 == randvals2);
  ASSERT_TRUE(cvar2 == randvals1);
}

TEST_F(CVarTest, DefaultConstructor) {
  // Object.
  neb::custom_variable cvar;

  // Check.
  ASSERT_EQ(cvar.source_id, 0);
  ASSERT_EQ(cvar.destination_id, 0);
  ASSERT_TRUE(cvar.default_value.empty());
  ASSERT_TRUE(cvar.enabled);
  ASSERT_EQ(cvar.host_id, 0);
  ASSERT_EQ(cvar.modified, false);
  ASSERT_EQ(cvar.name, "");
  ASSERT_EQ(cvar.service_id, 0);
  ASSERT_EQ(cvar.update_time, 0);
  ASSERT_EQ(cvar.value, "");
  ASSERT_EQ(cvar.var_type, 0);

  ASSERT_TRUE(
      cvar.type() ==
      (io::events::data_type<io::events::neb, neb::de_custom_variable>::value));
}
