/*
** Copyright 2011-2014,2017, 2021 Centreon
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

#include "com/centreon/broker/time/timeperiod.hh"
#include <fmt/format.h>
#include <ctime>
#include <stdexcept>
#include "com/centreon/broker/time/timezone_locker.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::time;

timeperiod::timeperiod(uint32_t id) : _id(id) {
  _exceptions.resize(daterange::daterange_types);
}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
timeperiod::timeperiod(timeperiod const& obj)
    : _id{obj._id},
      _timeperiod_name{obj._timeperiod_name},
      _alias{obj._alias},
      _exceptions{obj._exceptions},
      _include{obj._include},
      _exclude{obj._exclude},
      _timeranges{obj._timeranges},
      _timezone{obj._timezone} {}

/**
 *  Construct the timeperiod from data.
 *
 *  @param[in] name        The name of the timeperiod.
 *  @param[in] alias       The alias of the timeperiod.
 *  @param[in] sunday      A string describing the sunday timerange.
 *  @param[in] monday      A string describing the monday timerange.
 *  @param[in] tuesday     A string describing the tuesday timerange.
 *  @param[in] wednesday   A string describing the wednesday timerange.
 *  @param[in] thursday    A string describing the thursday timerange.
 *  @param[in] friday      A string describing the friday timerange.
 *  @param[in] saturday    A string describing the saturday timerange.
 */
timeperiod::timeperiod(uint32_t id,
                       const std::string& name,
                       const std::string& alias,
                       const std::string& sunday,
                       const std::string& monday,
                       const std::string& tuesday,
                       const std::string& wednesday,
                       const std::string& thursday,
                       const std::string& friday,
                       const std::string& saturday)
    : _id(id), _timeperiod_name(name), _alias(alias) {
  _exceptions.resize(daterange::daterange_types);
  std::vector<bool> success;
  if (!set_timerange(sunday, 0))
    throw msg_fmt("BAM: could not parse sunday for time period: {} ", id);
  if (!set_timerange(monday, 1))
    throw msg_fmt("BAM: could not parse monday for time period: {} ", id);
  if (!set_timerange(tuesday, 2))
    throw msg_fmt("BAM: could not parse tuesday for time period: {} ", id);
  if (!set_timerange(wednesday, 3))
    throw msg_fmt("BAM: could not parse wednesday for time period: {} ", id);
  if (!set_timerange(thursday, 4))
    throw msg_fmt("BAM: could not parse thursday for time period: {} ", id);
  if (!set_timerange(friday, 5))
    throw msg_fmt("BAM: could not parse friday for time period: {} ", id);
  if (!set_timerange(saturday, 6))
    throw msg_fmt("BAM: could not parse saturday for time period: {} ", id);
}

/**
 *  Get the id of the timeperiod.
 *
 *  @return  The id of the timeperiod.
 */
uint32_t timeperiod::get_id() const noexcept {
  return _id;
}

/**
 *  Get the alias of the timeperiod.
 *
 *  @return The alias of the timeperiod.
 */
const std::string& timeperiod::get_alias() const noexcept {
  return _alias;
}

/**
 *  Get the timeperiod exceptions.
 *
 *  @return The timeperiod exceptions.
 */
std::vector<std::list<daterange> > const& timeperiod::get_exceptions()
    const noexcept {
  return _exceptions;
}

/**
 *  Get the timeperiods exceptions from their type.
 *
 *  @param[in] type  The type of the exceptions.
 *
 *  @return          A lit of exceptions.
 */
const std::list<daterange>& timeperiod::get_exceptions_from_type(
    int type) const {
  if (type < 0 || type > daterange::daterange_types)
    throw std::out_of_range("get_exceptions_from_type(): out of range");
  else
    return _exceptions[type];
}

/**
 *  Add a new timeperiod exception.
 *
 *  @param val The exception to add.
 */
void timeperiod::add_exceptions(const std::list<daterange>& val) {
  _exceptions.push_back(val);
}

/**
 *  Add a new timeperiod exception from text.
 *
 *  @param[in] days    The days to parse.
 *  @param[in] string  The range of the exception.
 *
 *  @return            True if the exception was correctly parsed.
 */
bool timeperiod::add_exception(const std::string& days,
                               const std::string& range) {
  // Concatenate days and range.
  std::string d{fmt::format("{} {}", days, range)};

  // Parse everything.
  return daterange::build_dateranges_from_string(d, _exceptions);
}

/**
 *  Get the included timeperiods.
 *
 *  @return The included timeperiods.
 */
const std::vector<timeperiod::ptr>& timeperiod::get_included() const noexcept {
  return _include;
}

/**
 *  Add a new included timeperiod.
 *
 *  @param val The new included timeperiod.
 */
void timeperiod::add_included(timeperiod::ptr val) {
  _include.push_back(val);
}

/**
 *  Get the excluded timeperiods.
 *
 *  @return The excluded timeperiods.
 */
const std::vector<timeperiod::ptr>& timeperiod::get_excluded() const noexcept {
  return _exclude;
}

/**
 *  Add a new excluded timeperiod.
 *
 *  @param val The new excluded timeperiod.
 */
void timeperiod::add_excluded(timeperiod::ptr val) {
  _exclude.push_back(val);
}

/**
 *  Get the timeperiod name.
 *
 *  @return The timeperiod name.
 */
const std::string& timeperiod::get_name() const noexcept {
  return _timeperiod_name;
}

/**
 *  Set the timeperiod name.
 *
 * @param value The new name.
 */
void timeperiod::set_name(const std::string& value) {
  _timeperiod_name = value;
}

/**
 *  Get the timeperiod timeranges.
 *
 *  @return The timeperiod timeranges.
 */
const std::array<std::list<timerange>, 7>& timeperiod::get_timeranges()
    const noexcept {
  return _timeranges;
}

/**
 *  Get the timerange of a particular day.
 *
 *  @param day The day (from 0 to 6).
 *  @return The timerange on this day.
 */
const std::list<timerange>& timeperiod::get_timeranges_by_day(
    int day) const noexcept {
  return _timeranges[day];
}

/**
 *  Build the timerange of a day from a string.
 *
 *  @param[in] timerange_text  The timerange to build.
 *  @param[in] day             The day.
 *
 *  @return  True if the string is valid.
 */
bool timeperiod::set_timerange(const std::string& timerange_text, int day) {
  return timerange::build_timeranges_from_string(timerange_text,
                                                 _timeranges[day]);
}

/**
 *  Get the timezone of this timeperiod.
 *
 *  @return The timezone.
 */
const std::string& timeperiod::get_timezone() const noexcept {
  return _timezone;
}

/**
 *  Set the timezone of this timeperiod.
 *
 *  @param tz The timezone to set.
 */
void timeperiod::set_timezone(const std::string& tz) {
  _timezone = tz;
}

/**
 *  Check if the preferred time is valid in this timeperiod.
 *
 *  @param preferred_time The time to check.
 *  @return True if it is valid.
 */
bool timeperiod::is_valid(time_t preferred_time) const {
  return preferred_time != (time_t)-1 &&
         get_next_valid(preferred_time) == preferred_time;
}

/**
 *  Get the next valid time from preferred time in this timeperiod.
 *
 *  @param[in] preferred_time The preferred time.
 *  @return The next valid time.
 */
time_t timeperiod::get_next_valid(time_t preferred_time) const {
  // Set timezone.
  timezone_locker tzlock(_timezone.empty() ? nullptr : _timezone.c_str());

  // Check preferred_time.
  if (preferred_time != (time_t)-1) {
    // Compute first weekday.
    time_t midnight;
    int weekday;

    struct tm preftime_midnight;
    localtime_r(&preferred_time, &preftime_midnight);
    weekday = preftime_midnight.tm_wday;
    preftime_midnight.tm_sec = 0;
    preftime_midnight.tm_min = 0;
    preftime_midnight.tm_hour = 0;
    midnight = mktime(&preftime_midnight);

    // Loop through the next 8 days (today which is
    // already started plus 7 days ahead).
    struct tm day_midnight;
    memcpy(&day_midnight, &preftime_midnight, sizeof(struct tm));

    for (int i = 0; i < 8;
         ++i, day_midnight.tm_wday++, day_midnight.tm_mday++) {
      // Compute current day's midnight.
      time_t day_start{mktime(&day_midnight)};

      // Check all time ranges for this day of the week.
      time_t earliest_time((time_t)-1);
      for (const timerange& trange : _timeranges[(weekday + i) % 7]) {
        // Get range limits.
        time_t range_start((time_t)-1);
        time_t range_end((time_t)-1);
        if (trange.to_time_t(day_midnight, range_start, range_end)) {
          // Range is out of bound.
          if (preferred_time < range_end) {
            time_t potential_time((time_t)-1);

            // Preferred time occurs before range start, so use
            // range start time as earliest potential time.
            if (range_start >= preferred_time)
              potential_time = range_start;
            // Preferred time occurs between range start/end, so
            // use preferred time as earliest potential time.
            else
              potential_time = preferred_time;

            // Is this the earliest time found thus far ?
            if ((earliest_time == (time_t)-1) ||
                (potential_time < earliest_time)) {
              earliest_time = potential_time;
            }
          }
        }
      }
      if (earliest_time != (time_t)-1)
        return earliest_time;
    }
  }
  return (time_t)-1;
}

/**
 *  Get the next invalid time from preferred time in this timeperiod.
 *
 *  @param[in] preferred_time  The preferred time.
 *
 *  @return                    The next invalid time.
 */
time_t timeperiod::get_next_invalid(time_t preferred_time) const {
  // Set timezone.
  timezone_locker tzlock(_timezone.empty() ? nullptr : _timezone.c_str());

  // Check preferred_time.
  if (preferred_time != (time_t)-1) {
    // Compute first weekday.
    time_t midnight;
    int weekday;

    struct tm preftime_midnight;
    localtime_r(&preferred_time, &preftime_midnight);
    weekday = preftime_midnight.tm_wday;
    preftime_midnight.tm_sec = 0;
    preftime_midnight.tm_min = 0;
    preftime_midnight.tm_hour = 0;
    midnight = mktime(&preftime_midnight);

    // Loop through the next 8 days (today which is
    // already started plus 7 days ahead).
    struct tm day_midnight;
    struct tm end_midnight;
    memcpy(&day_midnight, &preftime_midnight, sizeof(struct tm));
    time_t day_end{mktime(&day_midnight)};

    memcpy(&end_midnight, &preftime_midnight, sizeof(struct tm));
    end_midnight.tm_wday++;
    end_midnight.tm_mday++;

    for (int i = 0; i < 8;
         ++i, end_midnight.tm_wday++, end_midnight.tm_mday++) {
      // Compute current day's midnight.

      time_t day_start{day_end};
      time_t day_end{mktime(&end_midnight)};

      // Try to find an invalid time in all ranges.
      time_t earliest_time(preferred_time > day_start ? preferred_time
                                                      : day_start);
      while (earliest_time < day_end) {
        bool invalid_in_all_periods(true);
        for (std::list<timerange>::const_iterator
                 trange(get_timeranges_by_day((weekday + i) % 7).begin()),
             trange_end(get_timeranges_by_day((weekday + i) % 7).end());
             trange != trange_end; ++trange) {
          // Get range limits.
          time_t range_start((time_t)-1);
          time_t range_end((time_t)-1);
          if (trange->to_time_t(day_midnight, range_start, range_end) &&
              (earliest_time >= range_start) && (earliest_time < range_end)) {
            invalid_in_all_periods = false;
            earliest_time = range_end;
          }
        }
        if (invalid_in_all_periods)
          return earliest_time;
      }
      memcpy(&day_midnight, &end_midnight, sizeof(struct tm));
    }
  }
  return (time_t)-1;
}

/**
 *  @brief Get the intersection of a timeperiod and a range.
 *
 *  @param[in] start_time     The start of the range.
 *  @param[in] end_time       The end of the range.
 *
 *  @return                   The duration intersected from the tp and the
 * range.
 */
uint32_t timeperiod::duration_intersect(time_t start_time,
                                        time_t end_time) const {
  uint32_t duration{0u};
  time_t current_start_time{start_time};
  time_t current_end_time{current_start_time};

  if (end_time < start_time)
    return 0;

  // We iterate on the range, going from next valid times to next invalid times.
  for (;;) {
    current_start_time = get_next_valid(current_end_time);
    current_end_time = get_next_invalid(current_start_time);
    if (current_start_time == (time_t)-1 || current_start_time > end_time)
      break;
    if (current_end_time == (time_t)-1 || current_end_time > end_time) {
      duration += std::difftime(end_time, current_start_time);
      break;
    } else
      duration += std::difftime(current_end_time, current_start_time);
  }
  return duration;
}

/**
 *  @brief Add a round number of days (expressed in seconds) to a date.
 *
 *  The number of day added can be negative, effectively removing round days.
 *
 *  @param[in] middnight  Midnight of base day.
 *  @param[in] skip       Number of days to skip (in seconds).
 *
 *  @return Midnight of the day in skip seconds.
 */
time_t timeperiod::add_round_days_to_midnight(time_t midnight, long long skip) {
  // Compute expected time with no DST.
  time_t next_day_time(midnight + skip);
  struct tm next_day;
  localtime_r(&next_day_time, &next_day);

  // There was a DST shift in between.
  if (next_day.tm_hour || next_day.tm_min || next_day.tm_sec) {
    /*
    ** The trick here is to move from midnight to noon
    ** and break time down in a tm structure. We're now sure to
    ** be in the proper day (DST shift is +-1h) we only have to reset
    ** time to midnight, convert back and we're done.
    */
    next_day_time += 12 * 60 * 60;
    localtime_r(&next_day_time, &next_day);
    next_day.tm_hour = 0;
    next_day.tm_min = 0;
    next_day.tm_sec = 0;
    next_day_time = mktime(&next_day);
  }

  return next_day_time;
}
