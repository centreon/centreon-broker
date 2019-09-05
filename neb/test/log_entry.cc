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
#include "com/centreon/broker/neb/log_entry.hh"
#include "randomize.hh"

using namespace com::centreon::broker;

class LogEntry : public ::testing::Test {
  void SetUp() {
    randomize_init();
  };

  void TearDown() {
    randomize_cleanup();
  };
};


/**
 *  Check log_entry's assignment operator.
 */
TEST_F(LogEntry, Assign) {
  // Object #1.
  neb::log_entry le1;
  std::vector<randval> randvals1;
  randomize(le1, &randvals1);

  // Object #2.
  neb::log_entry le2;
  randomize(le2);

  // Assignment.
  le2 = le1;

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(le1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(le1 != randvals2);
  ASSERT_FALSE(le2 != randvals1);
}

/**
 *  Check log_entry's copy constructor.
 */
TEST_F(LogEntry,CopyCtor) {
  // Object #1.
  neb::log_entry le1;
  std::vector<randval> randvals1;
  randomize(le1, &randvals1);

  // Object #2.
  neb::log_entry le2(le1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(le1, &randvals2);

  // Compare objects with expected results.
  ASSERT_FALSE(le1 != randvals2);
  ASSERT_FALSE(le2 != randvals1);
}

/**
 *  Check log_entry's default constructor.
 */
TEST_F(LogEntry,DefaultCtor) {
  // Object.
  neb::log_entry le;

  auto val(io::events::data_type<io::events::neb, neb::de_log_entry>::value);
  // Check.
  ASSERT_FALSE(le.source_id != 0);
  ASSERT_FALSE(le.destination_id != 0);
  ASSERT_FALSE(le.c_time != 0);
  ASSERT_FALSE(le.host_id != 0);
  ASSERT_FALSE(!le.host_name.empty());
  ASSERT_FALSE(le.issue_start_time != 0);
  ASSERT_FALSE(le.log_type != 0);
  ASSERT_FALSE(le.msg_type != 5);
  ASSERT_FALSE(!le.notification_cmd.empty());
  ASSERT_FALSE(!le.notification_contact.empty());
  ASSERT_FALSE(!le.output.empty());
  ASSERT_FALSE(!le.poller_name.empty());
  ASSERT_FALSE(le.retry != 0);
  ASSERT_FALSE(!le.service_description.empty());
  ASSERT_FALSE(le.service_id != 0);
  ASSERT_FALSE(le.status != 0);
  ASSERT_FALSE(le.type() != val);
}
