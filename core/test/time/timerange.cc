/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
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
#include "com/centreon/broker/time/timerange.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include <iostream>

using namespace com::centreon::broker::time;


TEST(Timerange, ParseWeirdTimerange) {
  std::unique_ptr<timeperiod> tp;

  // Here we tests weird timeranges but they should not throw
  // Timerange parser must understands theses

  ASSERT_NO_THROW(tp.reset(new timeperiod(2, "test", "alias",
      "\r \r08:00\r-12:00\r",
      "\n\n08:00 - 12:00\r",
      "08:00 -12:00\r",
      "08:00 - \n12:00\r",
      "08:00\t-\t12:00\r",
      "08:00-12:00,09:00-12:00,10:00-12:00",
      "08:00-12:00 , 09:00-12:00, \r\n 10:00-12:00")));

  auto& v = tp->get_timeranges();
  ASSERT_EQ(v[0].front().start(), 28800);
  ASSERT_EQ(v[0].front().end(), 43200);
  ASSERT_EQ(v[0].front().to_string(), "08:00-12:00");
  ASSERT_EQ(v[5].front().start(), 36000);
  ASSERT_EQ(v[5].front().end(), 43200);
  ASSERT_EQ(v[5].back().start(), 28800);
  ASSERT_EQ(v[5].front().end(), 43200);
  ASSERT_EQ(timerange::build_string_from_timeranges(v[5]),
      "08:00-12:00, 09:00-12:00, 10:00-12:00");
  ASSERT_EQ(v[6].front().start(), 36000);
  ASSERT_EQ(v[6].front().end(), 43200);
  ASSERT_EQ(v[6].back().start(), 28800);
  ASSERT_EQ(v[6].front().end(), 43200);
  ASSERT_EQ(timerange::build_string_from_timeranges(v[6]),
      "08:00-12:00, 09:00-12:00, 10:00-12:00");

}

TEST(Timerange, ParseWrongTimerange) {
  timerange t(10, 60);
  std::list<timerange> l;
  ASSERT_FALSE(t.build_timeranges_from_string("08:00-12:00abc", l));
  ASSERT_FALSE(t.build_timeranges_from_string("08:00-12:00 abc", l));
  ASSERT_FALSE(t.build_timeranges_from_string("08:00abc-12:00", l));
  ASSERT_FALSE(t.build_timeranges_from_string("08:00 abc-12:00", l));
  ASSERT_FALSE(t.build_timeranges_from_string("abc08:00-12:00", l));
  ASSERT_FALSE(t.build_timeranges_from_string("\n   abc08:00-12:00", l));
  ASSERT_FALSE(t.build_timeranges_from_string("08abcd:00-12:00", l));
  ASSERT_FALSE(t.build_timeranges_from_string("  :00-12:00", l));
}
