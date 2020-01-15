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

#include "com/centreon/broker/neb/event_handler.hh"
#include <gtest/gtest.h>
#include <cmath>
#include "randomize.hh"

using namespace com::centreon::broker;

class EventHandlerTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    randomize_init();
  }

  void TearDown() override {
    // Cleanup.
    randomize_cleanup();
  }
};

TEST_F(EventHandlerTest, Assignment) {
  // Object #1.
  neb::event_handler evnt_hndlr1;
  std::vector<randval> randvals1;
  randomize(evnt_hndlr1, &randvals1);

  // Object #2.
  neb::event_handler evnt_hndlr2;
  randomize(evnt_hndlr2);

  // Assignment.
  evnt_hndlr2 = evnt_hndlr1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(evnt_hndlr1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(evnt_hndlr1 == randvals2);
  ASSERT_TRUE(evnt_hndlr2 == randvals1);
}

TEST_F(EventHandlerTest, CopyConstructor) {
  // Object #1.
  neb::event_handler evnt_hndlr1;
  std::vector<randval> randvals1;
  randomize(evnt_hndlr1, &randvals1);

  // Object #2.
  neb::event_handler evnt_hndlr2(evnt_hndlr1);

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(evnt_hndlr1, &randvals2);

  // Compare objects with expected results.
  ASSERT_TRUE(evnt_hndlr1 == randvals2);
  ASSERT_TRUE(evnt_hndlr2 == randvals1);
}

TEST_F(EventHandlerTest, DefaultConstructor) {
  // Object.
  neb::event_handler evnt_hndlr;
  io::data::broker_id = 0;
  // Check.
  ASSERT_EQ(evnt_hndlr.source_id, 0u);
  ASSERT_EQ(evnt_hndlr.destination_id, 0u);
  ASSERT_EQ(evnt_hndlr.command_args, "");
  ASSERT_EQ(evnt_hndlr.command_line, "");
  ASSERT_EQ(evnt_hndlr.early_timeout, 0u);
  ASSERT_EQ(evnt_hndlr.end_time, 0u);
  ASSERT_TRUE(fabs(evnt_hndlr.execution_time) < 0.000001);
  ASSERT_EQ(evnt_hndlr.handler_type, 0);
  ASSERT_EQ(evnt_hndlr.host_id, 0u);
  ASSERT_EQ(evnt_hndlr.output, "");
  ASSERT_EQ(evnt_hndlr.return_code, 0);
  ASSERT_EQ(evnt_hndlr.service_id, 0u);
  ASSERT_EQ(evnt_hndlr.start_time, 0u);
  ASSERT_EQ(evnt_hndlr.state, 0);
  ASSERT_EQ(evnt_hndlr.state_type, 0);
  ASSERT_EQ(evnt_hndlr.timeout, 0);
  ASSERT_TRUE(
      evnt_hndlr.type() ==
      (io::events::data_type<io::events::neb, neb::de_event_handler>::value));
}
