/*
** Copyright 2011-2014,2017 Centreon
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

#include <stdexcept>
#include <sstream>
#include <ctime>
#include "com/centreon/broker/bam/time/timezone_locker.hh"
#include "com/centreon/broker/bam/time/timeperiod.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::bam::time;

static time_t _get_next_valid_time_per_timeperiod(
                time_t preferred_time,
                timeperiod const& tperiod);
static time_t _get_min_invalid_time_per_timeperiod(
                time_t preferred_time,
                timeperiod const& tperiod);

/**
 *  Default constructor.
 */
timeperiod::timeperiod() :
  _id(0) {
  _timeranges.resize(7);
  _exceptions.resize(daterange::daterange_types);
}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
timeperiod::timeperiod(timeperiod const& obj) {
  timeperiod::operator=(obj);
}

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
timeperiod::timeperiod(
      unsigned int id,
      std::string const& name,
      std::string const& alias,
      std::string const& sunday,
      std::string const& monday,
      std::string const& tuesday,
      std::string const& wednesday,
      std::string const& thursday,
      std::string const& friday,
      std::string const& saturday)
  : _id(id),
    _alias(alias),
    _timeperiod_name(name) {
  _timeranges.resize(7);
  _exceptions.resize(daterange::daterange_types);
  std::vector<bool> success;
  if (!set_timerange(sunday, 0))
    throw (exceptions::msg()
           << "BAM: could not parse sunday for time period: " << id);
  if (!set_timerange(monday, 1))
    throw (exceptions::msg()
           << "BAM: could not parse monday for time period: " << id);
  if (!set_timerange(tuesday, 2))
    throw (exceptions::msg()
           << "BAM: could not parse tuesday for time period: " << id);
  if (!set_timerange(wednesday, 3))
    throw (exceptions::msg()
           << "BAM: could not parse wednesday for time period: " << id);
  if (!set_timerange(thursday, 4))
    throw (exceptions::msg()
           << "BAM: could not parse thursday for time period: " << id);
  if (!set_timerange(friday, 5))
    throw (exceptions::msg()
           << "BAM: could not parse friday for time period: " << id);
  if (!set_timerange(saturday, 6))
    throw (exceptions::msg()
           << "BAM: could not parse saturday for time period: " << id);
}

timeperiod timeperiod::operator=(timeperiod const& obj) {
  if (this != &obj) {
    _id = obj._id;
    _alias = obj._alias;
    _exceptions = obj._exceptions;
    _include = obj._include;
    _exclude = obj._exclude;
    _timeperiod_name = obj._timeperiod_name;
    _timeranges = obj._timeranges;
    _timezone = obj._timezone;
  }
  return (*this);
}

/**
 *  Get the id of the timeperiod.
 *
 *  @return  The id of the timeperiod.
 */
unsigned int timeperiod::get_id() const throw() {
  return (_id);
}

/**
 *  Set the id of the timeperiod.
 *
 *  @param[in] id  The id of the timeperiod.
 */
void timeperiod::set_id(unsigned int id) throw() {
  _id = id;
}

/**
 *  Get the alias of the timeperiod.
 *
 *  @return The alias of the timeperiod.
 */
std::string const& timeperiod::get_alias() const throw() {
  return (_alias);
}

/**
 *  Set the alias of the timeperiod.
 *
 *  @param value The new alias' value.
 */
void timeperiod::set_alias(std::string const& value) {
  _alias = value;
}

/**
 *  Get the timeperiod exceptions.
 *
 *  @return The timeperiod exceptions.
 */
std::vector<std::list<daterange> > const&
  timeperiod::get_exceptions() const throw() {
  return (_exceptions);
}

/**
 *  Get the timeperiods exceptions from their type.
 *
 *  @param[in] type  The type of the exceptions.
 *
 *  @return          A lit of exceptions.
 */
std::list<daterange> const&
    timeperiod::get_exceptions_from_type(int type) const {
  if (type < 0 || type > daterange::daterange_types)
    throw (std::out_of_range("get_exceptions_from_type(): out of range"));
  else
    return (_exceptions[type]);
}

/**
 *  Add a new timeperiod exception.
 *
 *  @param val The exception to add.
 */
void timeperiod::add_exceptions(std::list<daterange> const& val) {
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
bool timeperiod::add_exception(
                   std::string const& days,
                   std::string const& range) {
  // Concatenate days and range.
  std::stringstream ss;
  ss << days << " " << range;

  // Parse everything.
  return (daterange::build_dateranges_from_string(ss.str(), _exceptions));
}

/**
 *  Get the included timeperiods.
 *
 *  @return The included timeperiods.
 */
std::vector<timeperiod::ptr> const& timeperiod::get_included() const throw() {
  return (_include);
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
std::vector<timeperiod::ptr> const& timeperiod::get_excluded() const throw() {
  return (_exclude);
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
std::string const& timeperiod::get_name() const throw() {
  return (_timeperiod_name);
}

/**
 *  Set the timeperiod name.
 *
 * @param value The new name.
 */
void timeperiod::set_name(std::string const& value) {
  _timeperiod_name = value;
}

/**
 *  Get the timeperiod timeranges.
 *
 *  @return The timeperiod timeranges.
 */
std::vector<std::list<timerange> > const&
  timeperiod::get_timeranges() const throw() {
  return (_timeranges);
}

/**
 *  Get the timerange of a particular day.
 *
 *  @param day The day (from 0 to 6).
 *  @return The timerange on this day.
 */
std::list<timerange> const&
  timeperiod::get_timeranges_by_day(int day) const throw() {
  return (_timeranges[day]);
}

/**
 *  Build the timerange of a day from a string.
 *
 *  @param[in] timerange_text  The timerange to build.
 *  @param[in] day             The day.
 *
 *  @return  True if the string is valid.
 */
bool timeperiod::set_timerange(
                   std::string const& timerange_text,
                   int day) {
  return (timerange::build_timeranges_from_string(
                       timerange_text,
                       _timeranges[day]));
}

/**
 *  Get the timezone of this timeperiod.
 *
 *  @return The timezone.
 */
std::string const& timeperiod::get_timezone() const throw() {
  return (_timezone);
}

/**
 *  Set the timezone of this timeperiod.
 *
 *  @param tz The timezone to set.
 */
void timeperiod::set_timezone(std::string const& tz) {
  _timezone = tz;
}

/**
 *  Check if the preferred time is valid in this timeperiod.
 *
 *  @param preferred_time The time to check.
 *  @return True if it is valid.
 */
bool timeperiod::is_valid(time_t preferred_time) const {
  return (get_next_valid(preferred_time) == preferred_time);
}

/**
 *  Get the next valid time from preferred time in this timeperiod.
 *
 *  @param[in] preferred_time The preferred time.
 *  @return The next valid time.
 */
time_t timeperiod::get_next_valid(time_t preferred_time) const {
  // First check for possible timeperiod exclusions
  // before getting a valid_time.
  timezone_locker tzlock(_timezone.empty() ? NULL : _timezone.c_str());
  return (_get_next_valid_time_per_timeperiod(
          preferred_time,
          *this));
}

/**
 *  Get the next invalid time from preferred time in this timeperiod.
 *
 *  @param[in] preferred_time  The preferred time.
 *
 *  @return                    The next invalid time.
 */
time_t timeperiod::get_next_invalid(time_t preferred_time) const {
  timezone_locker tzlock(_timezone.empty() ? NULL : _timezone.c_str());

  return (_get_min_invalid_time_per_timeperiod(preferred_time,
                                               *this));
}

/**
 *  @brief Get the intersection of a timeperiod and a range.
 *
 *  @param[in] start_time     The start of the range.
 *  @param[in] end_time       The end of the range.
 *
 *  @return                   The duration intersected from the tp and the range.
 */
unsigned int timeperiod::duration_intersect(
                           time_t start_time,
                           time_t end_time) const {
  unsigned int duration(0);
  time_t current_start_time(start_time);
  time_t current_end_time(current_start_time);

  if (end_time < start_time)
    return (0);

  // We iterate on the range, going from next valid times to next invalid times.
  while (true) {
    current_start_time = get_next_valid(current_end_time);
    current_end_time = get_next_invalid(current_start_time);
    if ((current_start_time == (time_t)-1)
        || (current_end_time == (time_t)-1)
        || (current_start_time > end_time))
      break ;
    if (current_end_time > end_time) {
      duration += std::difftime(end_time, current_start_time);
      break ;
    }
    else
      duration += std::difftime(current_end_time, current_start_time);
  }
  return (duration);
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

  return (next_day_time);
}


/**
 *  Get the earliest midnight of day that includes preferred time or
 *  occurs later.
 *
 *  @param[in] preferred_time     Preferred time.
 *  @param[in] drange             Date range.
 *  @param[in] drange_start_time  Date range start time.
 *  @param[in] drange_end_time    Date range end time.
 *
 *  @return Earliest midnight.
 */
static time_t _earliest_midnight_in_daterange(
                time_t preferred_time,
                daterange const& drange,
                time_t drange_start_time,
                time_t drange_end_time) {
  // XXX : handle full day skipping directly (from preferred_time to next midnight)
  while ((drange_start_time < drange_end_time)
         || (drange_end_time == (time_t)-1)) {
    // Next day at midnight.
    time_t next_day(timeperiod::add_round_days_to_midnight(
                      drange_start_time,
                      24 * 60 * 60));

    // Check range.
    if ((preferred_time < drange_start_time)
        || ((preferred_time >= drange_start_time)
            && (preferred_time < next_day)))
      return (drange_start_time);

    // Move to next day.
    if (drange.skip_interval() <= 1)
      drange_start_time = next_day;
    else
      drange_start_time = timeperiod::add_round_days_to_midnight(
                           drange_start_time,
                           drange.skip_interval() * 24 * 60 * 60);
  }
  return ((time_t)-1);
}

/**
 *  Get the next valid time within a time period.
 *
 *  @param[in]  preferred_time  The preferred time to check.
 *  @param[out] valid_time      Variable to fill.
 *  @param[in]  tperiod         The time period to use.
 *
 *  @return Next valid time after preferred_time in this timeperiod. If
 *          no valid time could be found, (time_t)-1 will be returned.
 */
static time_t _get_next_valid_time_per_timeperiod(
                time_t preferred_time,
                timeperiod const& tperiod) {
  // Do not compute more than one year ahead (we might compute forever).
  time_t earliest_time((time_t)-1);
  time_t in_one_year(preferred_time + 366 * 24 * 60 * 60);
  while ((earliest_time == (time_t)-1)
         && (preferred_time < in_one_year)) {
    // XXX : handle range end reached
    // Browse all date range.
    for (std::vector<std::list<daterange> >::const_iterator
           it(tperiod.get_exceptions().begin()),
           end(tperiod.get_exceptions().end());
         it != end;
         ++it) {
      for (std::list<daterange>::const_iterator
             drange(it->begin()),
             drange_end(it->end());
           drange != drange_end;
           ++drange) {
        // Get range limits.
        time_t daterange_start_time((time_t)-1);
        time_t daterange_end_time((time_t)-1);
        if (drange->to_time_t(
                      preferred_time,
                      daterange_start_time,
                      daterange_end_time)
            && ((preferred_time < daterange_end_time)
                || ((time_t)-1 == daterange_end_time))) {
          // Check that date is within range.
          time_t earliest_midnight(_earliest_midnight_in_daterange(
                                     preferred_time,
                                     *drange,
                                     daterange_start_time,
                                     daterange_end_time));
          if (earliest_midnight != (time_t)-1) {
            // Midnight.
            struct tm midnight;
            localtime_r(&earliest_midnight, &midnight);

            // Browse all time range of date range.
            for (std::list<timerange>::const_iterator
                   trange(drange->timeranges().begin()),
                   trange_end(drange->timeranges().end());
                 trange != trange_end;
                 ++trange) {
              // Get range limits.
              time_t range_start((time_t)-1);
              time_t range_end((time_t)-1);
              if (trange->to_time_t(
                            midnight,
                            range_start,
                            range_end)) {
                time_t potential_time((time_t)-1);
                // Range is out of bound.
                if (preferred_time < range_end) {
                  // Preferred time occurs before range start, so use
                  // range start time as earliest potential time.
                  if (range_start >= preferred_time)
                    potential_time = range_start;
                  // Preferred time occurs between range start/end, so
                  // use preferred time as earliest potential time.
                  else
                    potential_time = preferred_time;

                  // Is this the earliest time found thus far ?
                  if ((earliest_time == (time_t)-1)
                      || (potential_time < earliest_time))
                    earliest_time = potential_time;
                }
              }
            }
          }
        }
      }
    }


    /*
    ** Find next available time from normal, weekly rotating schedule.
    ** We do not need to check more than 8 days (today plus 7 days
    ** ahead) because time ranges are recurring the same way every week.
    */
    bool got_earliest_time(false);
    struct tm preftime;
    time_t midnight;
    localtime_r(&preferred_time, &preftime);
    preftime.tm_sec = 0;
    preftime.tm_min = 0;
    preftime.tm_hour = 0;
    midnight = mktime(&preftime);
    for (int weekday(preftime.tm_wday), days_into_the_future(0);
         (days_into_the_future <= 7) && !got_earliest_time;
         ++weekday, ++days_into_the_future) {
      if (weekday >= 7)
        weekday -= 7;

      // Calculate start of this future weekday.
      time_t day_start(timeperiod::add_round_days_to_midnight(
                         midnight,
                         days_into_the_future * 24 * 60 * 60));
      struct tm day_midnight;
      localtime_r(&day_start, &day_midnight);

      // Check all time ranges for this day of the week.
      for (std::list<timerange>::const_iterator
           trange(tperiod.get_timeranges_by_day(weekday).begin()),
           trange_end(tperiod.get_timeranges_by_day(weekday).end());
           trange != trange_end;
           ++trange) {
        // Get range limits.
        time_t range_start((time_t)-1);
        time_t range_end((time_t)-1);
        if (trange->to_time_t(
                      day_midnight,
                      range_start,
                      range_end)) {
          time_t potential_time((time_t)-1);
          // Range is out of bound.
          if (preferred_time < range_end) {
            // Preferred time occurs before range start, so use
            // range start time as earliest potential time.
            if (range_start >= preferred_time)
              potential_time = range_start;
            // Preferred time occurs between range start/end, so
            // use preferred time as earliest potential time.
            else
              potential_time = preferred_time;

            // Is this the earliest time found thus far ?
            if ((earliest_time == (time_t)-1)
                || (potential_time < earliest_time)) {
              earliest_time = potential_time;
              got_earliest_time = true;
            }
          }
        }
      }
    }

    // Check exclusions.
    if (earliest_time != (time_t)-1) {
      time_t max_invalid((time_t)-1);
      for (std::vector<timeperiod::ptr>::const_iterator
             exclusion(tperiod.get_excluded().begin()),
             end(tperiod.get_excluded().end());
           exclusion != end;
           ++exclusion) {
        time_t invalid((time_t)-1);
        invalid = _get_min_invalid_time_per_timeperiod(
          earliest_time,
          **exclusion);
        if ((invalid != (time_t)-1)
            && (((time_t)-1 == max_invalid)
                || (invalid > max_invalid)))
          max_invalid = invalid;
      }
      if ((max_invalid != (time_t)-1)
          && (max_invalid != earliest_time)) {
        earliest_time = (time_t)-1;
        preferred_time = max_invalid;
      }
    }
    // Increment preferred time to next day.
    else
      preferred_time = timeperiod::add_round_days_to_midnight(
                         midnight,
                         24 * 60 * 60);
  }

  return (earliest_time);
}

/**
 *  This function is for timeperiod exclusions,
 *
 *  @param[in]  preferred_time  The preferred time to check.
 *  @param[in]  tperiod         The time period to use.
 *
 *  @return                     The min invalid time.
 */
static time_t _get_min_invalid_time_per_timeperiod(
                time_t preferred_time,
                timeperiod const& tperiod) {
  // Do not compute more than one year ahead (we might compute forever).
  time_t earliest_time(preferred_time);
  time_t in_one_year(preferred_time + 366 * 24 * 60 * 60);
  while ((earliest_time != (time_t)-1)
         && (preferred_time < in_one_year)) {
    preferred_time = earliest_time;
    earliest_time = (time_t)-1;
    // XXX: handle range end reached.
    // Browse all date range.
    for (std::vector<std::list<daterange> >::const_iterator
           it(tperiod.get_exceptions().begin()),
           end(tperiod.get_exceptions().end());
         it != end;
         ++it) {
      for (std::list<daterange>::const_iterator
             drange(it->begin()),
             drange_end(it->end());
           drange != drange_end;
           ++drange) {
        // Get range limits.
        time_t daterange_start_time((time_t)-1);
        time_t daterange_end_time((time_t)-1);
        if (drange->to_time_t(
                      preferred_time,
                      daterange_start_time,
                      daterange_end_time)
            && ((preferred_time < daterange_end_time)
                || ((time_t)-1 == daterange_end_time))) {
          // Check that date is within range.
          time_t earliest_midnight(_earliest_midnight_in_daterange(
                                     preferred_time,
                                     *drange,
                                     daterange_start_time,
                                     daterange_end_time));
          if (earliest_midnight != (time_t)-1) {
            // Midnight.
            struct tm midnight;
            localtime_r(&earliest_midnight, &midnight);

            // Browse all time range of date range.
            for (std::list<timerange>::const_iterator
                   trange(drange->timeranges().begin()),
                   trange_end(drange->timeranges().end());
                 trange != trange_end;
                 ++trange) {
              // Get range limits.
              time_t range_start((time_t)-1);
              time_t range_end((time_t)-1);
              if (trange->to_time_t(
                            midnight,
                            range_start,
                            range_end)
                  && (preferred_time >= range_start)
                  && (preferred_time < range_end))
                earliest_time = range_end;
            }
          }
        }
      }
    }

    /*
    ** Find next available time from normal, weekly rotating schedule.
    ** We do not need to check more than 8 days (today plus 7 days
    ** ahead) because time ranges are recurring the same way every week.
    */
    struct tm preftime;
    time_t midnight;
    localtime_r(&preferred_time, &preftime);
    preftime.tm_sec = 0;
    preftime.tm_min = 0;
    preftime.tm_hour = 0;
    midnight = mktime(&preftime);
    for (int weekday(preftime.tm_wday), days_into_the_future(0);
         (days_into_the_future <= 7) && ((time_t)-1 == earliest_time);
         ++weekday, ++days_into_the_future) {
      if (weekday >= 7)
        weekday -= 7;

      // Calculate start of this future weekday.
      time_t day_start(timeperiod::add_round_days_to_midnight(
                         midnight,
                         days_into_the_future * 24 * 60 * 60));
      struct tm day_midnight;
      localtime_r(&day_start, &day_midnight);

      // Check all time ranges for this day of the week.
      for (std::list<timerange>::const_iterator
             trange(tperiod.get_timeranges_by_day(weekday).begin()),
             trange_end(tperiod.get_timeranges_by_day(weekday).end());
           trange != trange_end;
           ++trange) {
        // Get range limits.
        time_t range_start((time_t)-1);
        time_t range_end((time_t)-1);
        if (trange->to_time_t(
                      day_midnight,
                      range_start,
                      range_end)
            && (preferred_time >= range_start)
            && (preferred_time < range_end))
          earliest_time = range_end;
      }
    }

    // Find next exclusion time.
    time_t next_exclusion((time_t)-1);
    {
      timeperiod::exclusion_backup
                    exclusions(const_cast<timeperiod*>(&tperiod));
      for (std::vector<timeperiod::ptr>::const_iterator
             exclusion(exclusions.begin()),
             exclusion_end(exclusions.end());
           exclusion != exclusion_end;
           ++exclusion) {
        time_t valid((time_t)-1);
        valid = (*exclusion)->get_next_valid(preferred_time);
        if ((valid != (time_t)-1)
            && (((time_t)-1 == next_exclusion)
                || (valid < next_exclusion)))
          next_exclusion = valid;
      }
    }

    /*
    ** If we got an earliest_time this means that current preferred time
    ** is in a perfectly valid range. earliest_time holds the range end
    ** which might still be valid thanks to another date/time range and
    ** need to be checked. This is valid only if no exclusion occurs
    ** before.
    */

    if ((next_exclusion != (time_t)-1)
        && (next_exclusion < timeperiod::add_round_days_to_midnight(
                               midnight,
                               24 * 60 * 60))
        && (((time_t)-1 == earliest_time)
            || (next_exclusion <= earliest_time))) {
      earliest_time = (time_t)-1;
      preferred_time = next_exclusion;
      break ; // We have our time, no need to search anymore.
    }
    if (earliest_time != (time_t)-1) {
      preferred_time = earliest_time;
      earliest_time = (time_t)-1;
    }
  }

  // If we couldn't find an invalid time there must be none.
  if (earliest_time != (time_t)-1)
    return ((time_t)-1);
  // Else use the calculated time.
  else
    return (preferred_time);
}
