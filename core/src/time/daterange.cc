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
 *  Constructor.
 *
 *  @param[in] type The date range type.
 */
daterange::daterange(type_range type)
    : _type(type),
      _skip_interval(0) {}

/**
 *  Set skip_interval value.
 *
 *  @param[in] value The new skip_interval value.
 */
void daterange::skip_interval(uint32_t value) {
  _skip_interval = value;
}

/**
 *  Get skip_interval value.
 *
 *  @return The skip_interval value.
 */
uint32_t daterange::skip_interval() const noexcept {
  return _skip_interval;
}

/**
 *  Get timeranges value.
 *
 *  @return The timeranges value.
 */
void daterange::timeranges(std::list<timerange>&& value) {
  _timeranges = std::move(value);
}

/**
 *  Get timeranges value.
 *
 *  @return The timeranges value.
 */
//void daterange::timeranges(std::list<timerange> const& value) {
//  _timeranges = value;
//}

/**
 *  Set timeranges value.
 *
 *  @param[in] value The new timeranges value.
 */
std::list<timerange> const& daterange::timeranges() const noexcept {
  return _timeranges;
}

/**
 *  Get type value.
 *
 *  @return The type value.
 */
daterange::type_range daterange::type() const noexcept {
  return _type;
}

// UTILITIES

/**
 *  Get the month id.
 *
 *  @param[in]  name The month name.
 *  @param[out] id   The id to fill.
 *
 *  @return True on success, otherwise false.
 */
static bool _get_month_id(std::string const& name, uint32_t& id) {
  static std::string const months[] = {
      "january", "february", "march",     "april",   "may",      "june",
      "july",    "august",   "september", "october", "november", "december"};
  for (id = 0; id < sizeof(months) / sizeof(months[0]); ++id)
    if (name == months[id])
      return true;
  return false;
}

/**
 *  Get the week day id.
 *
 *  @param[in]  name The week day name.
 *  @param[out] id   The id to fill.
 *
 *  @return True on success, otherwise false.
 */
static bool _get_day_id(std::string const& name, uint32_t& id) {
  static std::string const days[] = {"sunday",    "monday",   "tuesday",
                                     "wednesday", "thursday", "friday",
                                     "saturday"};
  for (id = 0; id < sizeof(days) / sizeof(days[0]); ++id)
    if (name == days[id])
      return true;
  return false;
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
                                    std::vector<std::list<daterange> >& list) {
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

    list[daterange::calendar_date].emplace_front(daterange::calendar_date);
    daterange& range{list[daterange::calendar_date].front()};
    range.skip_interval(skip_interval);
    range.timeranges(std::move(timeranges));

    return true;
  }
  return false;
}

/**
 *  Build a non-calendar date.
 *
 *  @param[in] line   The string to parse.
 *  @param[out] list  A vector of dateranges to be filled.
 *
 *  @return           True if the line was correctly parsed.
 */
bool daterange::build_other_date(std::string const& line,
                                 std::vector<std::list<daterange> >& list) {
  int pos(0);
  daterange::type_range type(daterange::none);
  uint32_t month_start(0);
  uint32_t month_end(0);
  int month_day_start(0);
  int month_day_end(0);
  uint32_t skip_interval(0);
  uint32_t week_day_start(0);
  uint32_t week_day_end(0);
  int week_day_start_offset(0);
  int week_day_end_offset(0);
  char buffer[4][4096];

  if (line.size() > 1024)
    return false;

  if (sscanf(line.c_str(), "%[a-z] %d %[a-z] - %[a-z] %d %[a-z] / %u %n",
             buffer[0], &week_day_start_offset, buffer[1], buffer[2],
             &week_day_end_offset, buffer[3], &skip_interval, &pos) == 7) {
    // wednesday 1 january - thursday 2 july / 3
    if (_get_day_id(buffer[0], week_day_start) &&
        _get_month_id(buffer[1], month_start) &&
        _get_day_id(buffer[2], week_day_end) &&
        _get_month_id(buffer[3], month_end))
      type = daterange::month_week_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d - %[a-z] %d / %u %n", buffer[0],
                    &month_day_start, buffer[1], &month_day_end, &skip_interval,
                    &pos) == 5) {
    // monday 2 - thursday 3 / 2
    if (_get_day_id(buffer[0], week_day_start) &&
        _get_day_id(buffer[1], week_day_end)) {
      week_day_start_offset = month_day_start;
      week_day_end_offset = month_day_end;
      type = daterange::week_day;
    }
    // february 1 - march 15 / 3
    else if (_get_month_id(buffer[0], month_start) &&
             _get_month_id(buffer[1], month_end))
      type = daterange::month_date;
    // day 4 - 6 / 2
    else if (!strcmp(buffer[0], "day") && !strcmp(buffer[1], "day"))
      type = daterange::month_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d - %d / %u %n", buffer[0],
                    &month_day_start, &month_day_end, &skip_interval,
                    &pos) == 4) {
    // thursday 2 - 4
    if (_get_day_id(buffer[0], week_day_start)) {
      week_day_start_offset = month_day_start;
      week_day_end_offset = month_day_end;
      type = daterange::week_day;
    }
    // february 3 - 5
    else if (_get_month_id(buffer[0], month_start)) {
      month_end = month_start;
      type = daterange::month_date;
    }
    // day 1 - 4
    else if (!strcmp(buffer[0], "day"))
      type = daterange::month_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d %[a-z] - %[a-z] %d %[a-z] %n",
                    buffer[0], &week_day_start_offset, buffer[1], buffer[2],
                    &week_day_end_offset, buffer[3], &pos) == 6) {
    // wednesday 1 january - thursday 2 july
    if (_get_day_id(buffer[0], week_day_start) &&
        _get_month_id(buffer[1], month_start) &&
        _get_day_id(buffer[2], week_day_end) &&
        _get_month_id(buffer[3], month_end))
      type = daterange::month_week_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d - %d %n", buffer[0],
                    &month_day_start, &month_day_end, &pos) == 3) {
    // thursday 2 - 4
    if (_get_day_id(buffer[0], week_day_start)) {
      week_day_start_offset = month_day_start;
      week_day_end = week_day_start;
      week_day_end_offset = month_day_end;
      type = daterange::week_day;
    }
    // february 3 - 5
    else if (_get_month_id(buffer[0], month_start)) {
      month_end = month_start;
      type = daterange::month_date;
    }
    // day 1 - 4
    else if (!strcmp(buffer[0], "day"))
      type = daterange::month_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d - %[a-z] %d %n", buffer[0],
                    &month_day_start, buffer[1], &month_day_end, &pos) == 4) {
    // monday 2 - thursday 3
    if (_get_day_id(buffer[0], week_day_start) &&
        _get_day_id(buffer[1], week_day_end)) {
      week_day_start_offset = month_day_start;
      week_day_end_offset = month_day_end;
      type = daterange::week_day;
    }
    // february 1 - march 15
    else if (_get_month_id(buffer[0], month_start) &&
             _get_month_id(buffer[1], month_end))
      type = daterange::month_date;
    // day 1 - day 5
    else if (!strcmp(buffer[0], "day") && !strcmp(buffer[1], "day"))
      type = daterange::month_day;
  } else if (sscanf(line.c_str(), "%[a-z] %d %[a-z] %n", buffer[0],
                    &week_day_start_offset, buffer[1], &pos) == 3) {
    // thursday 3 february
    if (_get_day_id(buffer[0], week_day_start) &&
        _get_month_id(buffer[1], month_start)) {
      month_end = month_start;
      week_day_end = week_day_start;
      week_day_end_offset = week_day_start_offset;
      type = daterange::month_week_day;
    }
  } else if (sscanf(line.c_str(), "%[a-z] %d %n", buffer[0], &month_day_start,
                    &pos) == 2) {
    // thursday 2
    if (_get_day_id(buffer[0], week_day_start)) {
      week_day_start_offset = month_day_start;
      week_day_end = week_day_start;
      week_day_end_offset = week_day_start_offset;
      type = daterange::week_day;
    }
    // february 3
    else if (_get_month_id(buffer[0], month_start)) {
      month_end = month_start;
      month_day_end = month_day_start;
      type = daterange::month_date;
    }
    // day 1
    else if (!strcmp(buffer[0], "day")) {
      month_day_end = month_day_start;
      type = daterange::month_day;
    }
  }

  if (type != daterange::none) {
    auto range = list[type].emplace(list[type].begin(), type);
    switch (type) {
      case daterange::month_day:
        break;
      case daterange::month_week_day:
        break;
      case daterange::week_day:
        break;
      case daterange::month_date:
        break;
      default:
        log_v2::core()->warn("daterange: build_other_date: type {} not managed.", type);
        // should not occure.
    }
    range->skip_interval(skip_interval);

    std::list<timerange> timeranges;
    if (!timerange::build_timeranges_from_string(line.substr(pos), timeranges))
      return false;

    range->timeranges(std::move(timeranges));
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
    std::vector<std::list<daterange> >& list) {
  return build_calendar_date(value, list) || build_other_date(value, list);
}
