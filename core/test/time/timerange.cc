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
  
using namespace com::centreon::broker::time;


TEST(Timerange, parseTimerange) {
  std::unique_ptr<timeperiod> tp;
  
  // Here we tests weird timeranges but they should not crash
  // Timerange parser must understands theses

  ASSERT_NO_THROW(tp.reset(new timeperiod(2, "test", "alias",

      "\r \r08:00\r-12:00\r",

      "\n\n08:00 - 12:00\r",

      "08:00 -12:00\r",

      "08:00 - \n12:00\r",

      "08:00\t-\t12:00\r",

      "08:00-12:00\n",

      "08:00-12:00")));

  auto& v = tp->get_timeranges();
  ASSERT_EQ(v[0].front().start(), 28800);
  ASSERT_EQ(v[0].front().end(), 43200);

  /*ASSERT_NO_THROW(tp.reset(new timeperiod(2, "test", "alias",
      "08:00-12:00abs",

      "08:00-12:00abs",

      "08:00-12:00abs",

      "08:00-12:00abs",

      "08:00-12:00abs",

      "08:00-12:00abs",

      "08:00-12:00abs")));

  auto& v2 = tp->get_timeranges();
  ASSERT_EQ(v2[0].front().start(), 28800);
  ASSERT_EQ(v2[0].front().end(), 43200);
  */
}
