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
#include "com/centreon/broker/time/timeperiod.hh"

using namespace com::centreon::broker::time;

TEST(Timeperiod, Constructor) {
  timeperiod tp(12, "test", "alias");
  ASSERT_EQ(tp.get_id(), 12u);
  ASSERT_EQ(tp.get_name(), std::string("test"));
  ASSERT_EQ(tp.get_alias(), std::string("alias"));
  for (const auto& e : tp.get_exceptions())
    ASSERT_TRUE(e.empty());
}

TEST(Timeperiod, intersect) {
  timeperiod tp(2, "test", "alias");
  tp.set_timerange("08:00-12:00", 1);
  // Intersection with the timerange:
  // * lun. 05 avril 2021 07:00:00 CEST
  // * lun. 05 avril 2021 14:00:00 CEST
  //
  // duration should be 4h.
  auto duration = tp.duration_intersect(1617598800, 1617624000);
  ASSERT_EQ(duration, 4 * 3600);
}

TEST(Timeperiod, intersectReverseBounds) {
  timeperiod tp(2, "test", "alias");
  tp.set_timerange("08:00-12:00", 1);

  // Intersection with start > end
  auto duration = tp.duration_intersect(1617624000, 1617598800);
  ASSERT_EQ(duration, 0);
}
