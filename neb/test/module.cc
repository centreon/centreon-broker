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
#include "com/centreon/broker/neb/module.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class Module : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};


/**
 *  Check module's assignment operator.
 */
TEST_F(Module, Assign) {
  // Object #1.
  neb::module mod1;
  std::vector<randval> randvals1;
  randomize(mod1, &randvals1);

  // Object #2.
  neb::module mod2;
  randomize(mod2);

  // Assignment.
  mod2 = mod1;

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(mod1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(mod1 != randvals2);
  ASSERT_FALSE(mod2 != randvals1);
}


/**
 *  Check module's copy constructor.
 */
TEST_F(Module, CopyCtor) {
  // Object #1.
  neb::module mod1;
  std::vector<randval> randvals1;
  randomize(mod1, &randvals1);

  // Object #2.
  neb::module mod2(mod1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(mod1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(mod1 != randvals2);
  ASSERT_FALSE(mod2 != randvals1);
}


/**
 *  Check module's default constructor.
 */
TEST_F(Module, DefaultCtor) {
  // Object.
  neb::module mod;

  auto val(io::events::data_type<io::events::neb, neb::de_module>::value);
  // Check.
  ASSERT_FALSE(mod.source_id != 0);
  ASSERT_FALSE(mod.destination_id != 0);
  ASSERT_FALSE(mod.args != "");
  ASSERT_FALSE(mod.enabled != true);
  ASSERT_FALSE(mod.filename != "");
  ASSERT_FALSE(mod.loaded != false);
  ASSERT_FALSE(mod.should_be_loaded != false);
  ASSERT_FALSE(mod.type() != val);
}
