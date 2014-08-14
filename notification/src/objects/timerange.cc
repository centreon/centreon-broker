/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstring>
#include "com/centreon/broker/notification/objects/timerange.hh"

using namespace com::centreon::broker::notification;

/**
 *  Constructor.
 *
 *  @param[in] start The start time.
 *  @param[in] end   The end time.
 */
timerange::timerange(unsigned long start, unsigned long end)
  : _end(end),
    _start(start) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
timerange::timerange(timerange const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
timerange::~timerange() throw () {

}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
timerange& timerange::operator=(timerange const& right) {
  if (this != &right) {
    _end = right._end;
    _start = right._start;
  }
  return (*this);
}

/**
 *  Equal constructor.
 *
 *  @param[in] right The object to compare.
 *
 *  @return True if the object are the same, otherwise false.
 */
bool timerange::operator==(timerange const& right) const throw () {
  return (_start == right._start && _end == right._end);
}

/**
 *  Not equal constructor.
 *
 *  @param[in] right The object to compare.
 *
 *  @return True if the object are not the same, otherwise false.
 */
bool timerange::operator!=(timerange const& right) const throw () {
  return (!operator==(right));
}

/**
 *  Less-than operator.
 *
 *  @param[in] right Object to compare to.
 *
 *  @return True if this object is less than right.
 */
bool timerange::operator<(timerange const& right) const throw () {
  if (_start != right._start)
    return (_start < right._start);
  return (_end < right._end);
}

/**
 *  Get the end time.
 *
 *  @return The end time.
 */
unsigned long timerange::end() const throw () {
  return (_end);
}

/**
 *  Set the end time.
 *
 *  @param[in] value The end time.
 */
void timerange::end(unsigned long value) {
  _end = value;
}

/**
 *  Get the start time.
 *
 *  @return The start time.
 */
unsigned long timerange::start() const throw () {
  return (_start);
}

/**
 *  Set the start time.
 *
 *  @param[in] value The strart time.
 */
void timerange::start(unsigned long value) {
  _start = value;
}

/**
 *  Get the hour when the timerange starts.
 *
 *  @return The hour when the timerange starts.
 */
unsigned long timerange::start_hour() const throw() {
  return (_start / (60 * 60));
}

/**
 *  Get the minute when the timerange starts.
 *
 *  @return The minute when the timerange starts.
 */
unsigned long timerange::start_minute() const throw() {
  return ((_start / 60) % 60);
}

/**
 *  Get the hour when the timerange ends.
 *
 *  @return The hour when the timerange ends.
 */
unsigned long timerange::end_hour() const throw() {
  return (_end / (60 * 60));
}

/**
 *  Get the minute when the timerange ends.
 *
 *  @return The minute when the timerange ends.
 */
unsigned long timerange::end_minute() const throw() {
  return ((_end / 60) % 60);
}

/**
 *  Get time range limits.
 *
 *  @param[in]  midnight     Midnight of day.
 *  @param[out] range_start  Start of time range in this specific day.
 *  @param[out] range_end    End of time range in this specific day.
 *
 *  @return True upon successful conversion.
 */
bool timerange::to_time_t(struct tm const& midnight,
                          time_t& range_start,
                          time_t& range_end) const {
  struct tm my_tm;
  memcpy(&my_tm, &midnight, sizeof(my_tm));
  my_tm.tm_hour = start_hour();
  my_tm.tm_min = start_minute();
  range_start = mktime(&my_tm);
  my_tm.tm_hour = end_hour();
  my_tm.tm_min = end_minute();
  range_end = mktime(&my_tm);
  return (true);
}
