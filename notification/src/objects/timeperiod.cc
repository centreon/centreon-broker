/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <ctime>
#include "com/centreon/broker/notification/objects/timezone_locker.hh"
#include "com/centreon/broker/notification/objects/timeperiod.hh"

using namespace com::centreon::broker::notification::objects;

static time_t _get_next_valid_time_per_timeperiod(
              time_t preferred_time,
              time_t current_time,
              timeperiod const& tperiod);

/**
 *  Default constructor.
 */
timeperiod::timeperiod()
{
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
 *  Assignment operator for timeperiod.
 *
 *  @param[in] obj  The object to copy.
 *  @return         A reference to this object.
 */
timeperiod timeperiod::operator=(timeperiod const& obj) {
  if (this != &obj) {
    _alias = obj._alias;
    _exceptions = obj._exceptions;
    _exclude = obj._exclude;
    _timeperiod_name = obj._timeperiod_name;
    _timeranges = obj._timeranges;
    _timezone = obj._timezone;
  }
  return (*this);
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
 *  Add a new timeperiod exception.
 *
 *  @param val The exception to add.
 */
void timeperiod::add_exceptions(std::list<daterange> const& val) {
  _exceptions.push_back(val);
}

/**
 *  Get the excluded timeperiods.
 *
 *  @return The excluded timeperiods.
 */
group const& timeperiod::get_exclude() const throw() {
  return (_exclude);
}

/**
 *  Set the excluded timeperiods.
 *
 *  @param val The new excluded timeperiods.
 */
void timeperiod::set_exclude(group const& val) {
  _exclude = val;
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
 *  Set the timeperiod timeranges.
 *
 *  @param val The timeranges to be added.
 */
void timeperiod::add_timerange(std::list<timerange> const& val) {
  _timeranges.push_back(val);
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
 *  Set the timerange of a particular day.
 *
 *  @param val The new timeranges to set.
 *  @param day The day (from 0 to 6).
 */
void timeperiod::set_timerange(std::list<timerange> const& val,
                               int day) {
  _timeranges[day] = val;
}

void timeperiod::set_timerange(std::string const& timerange_text,
                               int day) {
  timerange::build_timeranges_from_string(timerange_text, _timeranges[day]);
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
 *  @param preferred_time The preferred time.
 *  @return The next valid time.
 */
time_t timeperiod::get_next_valid(time_t preferred_time) const {
  // Preferred time must be now or in the future.
  time_t current_time(time(NULL));
  preferred_time = std::max(preferred_time, current_time);

  // First check for possible timeperiod exclusions
  // before getting a valid_time.
  timezone_locker tzlock(_timezone.c_str());
  return (_get_next_valid_time_per_timeperiod(
          preferred_time,
          current_time,
          *this));
}

/**
 *  Add a round number of days (expressed in seconds) to a date.
 *
 *  @param[in] middnight  Midnight of base day.
 *  @param[in] skip       Number of days to skip (in seconds).
 *
 *  @return Midnight of the day in skip seconds.
 */
static time_t _add_round_days_to_midnight(time_t midnight, time_t skip) {
  // Compute expected time with no DST.
  time_t next_day_time(midnight + skip);
  struct tm next_day;
  localtime_r(&next_day_time, &next_day);

  // There was a DST shift in between.
  if (next_day.tm_hour || next_day.tm_min || next_day.tm_sec) {
    /*
    ** The trick here is to move from midnight to noon, add the skip
    ** seconds and break time down in a tm structure. We're now sure to
    ** be in the proper day (DST shift is +-1h) we only have to reset
    ** time to midnight, convert back and we're done.
    */
    next_day_time += 12 * 60 * 60 + skip;
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
    time_t next_day(_add_round_days_to_midnight(
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
      drange_start_time = _add_round_days_to_midnight(
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
 *  @param[in]  current_time    The current time.
 *  @param[in]  tperiod         The time period to use.
 */
static time_t _get_next_valid_time_per_timeperiod(
              time_t preferred_time,
              time_t current_time,
              timeperiod const& tperiod) {

  // If no time can be found, the original preferred time will be returned
  time_t original_preferred_time(preferred_time);

  // Do not compute more than one year ahead (we might compute forever).
  time_t earliest_time((time_t)-1);
  time_t in_one_year(preferred_time + 366 * 24 * 60 * 60);
  while ((earliest_time == (time_t)-1)
         && (preferred_time < in_one_year)) {
    // XXX : handle range end reached
    // Browse all date range.
    for (std::vector<std::list<daterange> >::const_iterator
         it(tperiod.get_exceptions().begin()),
         end(tperiod.get_exceptions().end()); it != end; ++it) {
      for (std::list<daterange>::const_iterator drange(it->begin()),
           drange_end(it->end()); drange != drange_end; ++drange) {
        // Get range limits.
        time_t daterange_start_time((time_t)-1);
        time_t daterange_end_time((time_t)-1);
        if (drange->to_time_t(preferred_time,
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
            for (std::list<timerange>::const_iterator trange(drange->timeranges().begin()),
                 trange_end(drange->timeranges().end()); trange != trange_end; ++trange) {
              // Get range limits.
              time_t range_start((time_t)-1);
              time_t range_end((time_t)-1);
              if (trange->to_time_t(midnight,
                                    range_start,
                                    range_end)) {
                time_t potential_time((time_t)-1);
                // Range is out of bound.
                if (preferred_time <= range_end) {
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
    time_t now(time(NULL));
    struct tm curtime;
    struct tm preftime;
    time_t midnight;
    localtime_r(&now, &curtime);
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
      time_t day_start(_add_round_days_to_midnight(
                         midnight,
                         days_into_the_future * 24 * 60 * 60));
      struct tm day_midnight;
      localtime_r(&day_start, &day_midnight);

      // Check all time ranges for this day of the week.
      for (std::list<timerange>::const_iterator
           trange(tperiod.get_timeranges_by_day(weekday).begin()),
           trange_end(tperiod.get_timeranges_by_day(weekday).end());
           trange != trange_end && !got_earliest_time;
           ++trange) {
        // Get range limits.
        time_t range_start((time_t)-1);
        time_t range_end((time_t)-1);
        if (trange->to_time_t(day_midnight,
                              range_start,
                              range_end)) {
          time_t potential_time((time_t)-1);
          // Range is out of bound.
          if (preferred_time <= range_end) {
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

    /*// Check exclusions.
    if (earliest_time != (time_t)-1) {
      timeperiodexclusion* first_exclusion(tperiod->exclusions);
      tperiod->exclusions = NULL;
      time_t max_invalid((time_t)-1);
      for (timeperiodexclusion* exclusion(first_exclusion);
           exclusion;
           exclusion = exclusion->next) {
        time_t invalid((time_t)-1);
        _get_min_invalid_time_per_timeperiod(
          earliest_time,
          &invalid,
          current_time,
          exclusion->timeperiod_ptr);
        if ((invalid != (time_t)-1)
            && (((time_t)-1 == max_invalid)
                || (invalid > max_invalid)))
          max_invalid = invalid;
      }
      tperiod->exclusions = first_exclusion;
      if ((max_invalid != (time_t)-1)
          && (max_invalid != earliest_time)) {
        earliest_time = (time_t)-1;
        preferred_time = max_invalid;
      }
    }
    // Increment preferred time to next day.
    else
      preferred_time = _add_round_days_to_midnight(
                         midnight,
                         24 * 60 * 60);*/
  }

  // If we couldn't find a time period there must be none defined.
  if (earliest_time == (time_t)-1)
    return (original_preferred_time);
  else
    return (earliest_time);
}
