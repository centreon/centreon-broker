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
#include "com/centreon/broker/time/daterange.hh"

using namespace com::centreon::broker::time;

TEST(Daterange, BuildOtherDate_Bad) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_FALSE(d.build_dateranges_from_string(
      "monday 1 january - wednesday 3 january / 1 0", list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges1) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "monday 1 january - wednesday 3 january / 1 08:00-12:00, 14:00-18:00",
      list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges2) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "monday 1 - wednesday 3 / 2 08:00-12:00, 14:00-18:00",
      list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges3) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "january 1 - march 3 / 2 08:00-12:00, 11:00-14:00",
      list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges4) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "day 10 - day 15 / 2 08:00-12:00, 11:00-14:00",
      list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges5) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "day 10 - 15 / 2 08:00-12:00, 11:00-14:00",
      list));
}

TEST(Daterange, BuildOtherDate_GoodTimeRanges6) {
  daterange d(daterange::calendar_date);
  ASSERT_EQ(d.type(), daterange::calendar_date);
  std::vector<std::list<daterange>> list;
  list.resize(daterange::daterange_types);

  // Here we have a skip value of 1 but then we should have a timerange, but
  // here it is only 0.
  ASSERT_TRUE(d.build_dateranges_from_string(
      "day 10 - 15 / 2 08:00-12:00, 11:00-14:00",
      list));
}
