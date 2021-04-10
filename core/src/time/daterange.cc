/*
** Copyright 2011-2013, 2021 Centreon
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

#include "com/centreon/broker/time/daterange.hh"
#include <cstdio>
#include <cstring>
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker::time;

/**
 *  Default constructor.
 *
 */
daterange::daterange() {}

/**
 *  Get timeranges value.
 *
 *  @return The timeranges value.
 */
void daterange::timeranges(std::list<timerange>&& value) {
  _timeranges = std::move(value);
}

/**
 *  Set timeranges value.
 *
 *  @param[in] value The new timeranges value.
 */
std::list<timerange> const& daterange::timeranges() const noexcept {
  return _timeranges;
}

/**
 *  Build a calendar date.
 *
 *  @param[in] line   The string to parse.
 *  @param[out] list  A vector of dateranges to be filled.
 *
 *  @return           True if the line was correctly parsed.
 */
bool daterange::build_calendar_date(std::string const& line,
                                    std::list<daterange>& list) {
  int ret(0);
  int pos(0);
  uint32_t month_start(0);
  uint32_t month_end(0);
  uint32_t month_day_start(0);
  uint32_t month_day_end(0);
  uint32_t year_start(0);
  uint32_t year_end(0);
  uint32_t skip_interval(0);

  if ((ret = sscanf(line.c_str(), "%4u-%2u-%2u - %4u-%2u-%2u / %u %n",
                    &year_start, &month_start, &month_day_start, &year_end,
                    &month_end, &month_day_end, &skip_interval, &pos)) == 7)
    ;
  else if ((ret = sscanf(line.c_str(), "%4u-%2u-%2u - %4u-%2u-%2u %n",
                         &year_start, &month_start, &month_day_start, &year_end,
                         &month_end, &month_day_end, &pos)) == 6)
    ;
  else if ((ret = sscanf(line.c_str(), "%4u-%2u-%2u / %u %n", &year_start,
                         &month_start, &month_day_start, &skip_interval,
                         &pos)) == 4) {
    year_end = 0;
    month_end = 0;
    month_day_end = 0;
  } else if ((ret = sscanf(line.c_str(), "%4u-%2u-%2u %n", &year_start,
                           &month_start, &month_day_start, &pos)) == 3) {
    year_end = year_start;
    month_end = month_start;
    month_day_end = month_day_start;
  }

  if (ret) {
    std::list<timerange> timeranges;
    if (!timerange::build_timeranges_from_string(line.substr(pos), timeranges))
      return false;

    list.emplace(list.begin());
    daterange& range{list.front()};
    range.timeranges(std::move(timeranges));

    return true;
  }
  return false;
}

/**
 *  Build a daterange from a string.
 *
 *  @param[in] value  A string containing a daterange.
 *
 *  @param[out] list  A vector of dateranges to be filled.
 *
 *  @return           True if the string was correctly parsed.
 */
bool daterange::build_dateranges_from_string(
    std::string const& value,
    std::list<daterange>& list) {
  return build_calendar_date(value, list);
}
