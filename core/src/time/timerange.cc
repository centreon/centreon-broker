/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/time/timerange.hh"
#include <cstring>
#include <sstream>
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::time;

/**
 *  Constructor.
 *
 *  @param[in] start The start time.
 *  @param[in] end   The end time.
 */
timerange::timerange(unsigned long start, unsigned long end)
    : _end(end), _start(start) {}

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
timerange::~timerange() throw() {}

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
bool timerange::operator==(timerange const& right) const throw() {
  return (_start == right._start && _end == right._end);
}

/**
 *  Not equal constructor.
 *
 *  @param[in] right The object to compare.
 *
 *  @return True if the object are not the same, otherwise false.
 */
bool timerange::operator!=(timerange const& right) const throw() {
  return (!operator==(right));
}

/**
 *  Less-than operator.
 *
 *  @param[in] right Object to compare to.
 *
 *  @return True if this object is less than right.
 */
bool timerange::operator<(timerange const& right) const throw() {
  if (_start != right._start)
    return (_start < right._start);
  return (_end < right._end);
}

/**
 *  Get the end time.
 *
 *  @return The end time.
 */
unsigned long timerange::end() const throw() {
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
unsigned long timerange::start() const throw() {
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

static bool _build_time_t(std::string const& time_str, unsigned long& ret) {
  std::size_t pos(time_str.find(':'));
  if (pos == std::string::npos)
    return (false);
  unsigned long hours;
  if (!misc::string::to(time_str.substr(0, pos).c_str(), hours))
    return (false);
  unsigned long minutes;
  if (!misc::string::to(time_str.substr(pos + 1).c_str(), minutes))
    return (false);
  ret = hours * 3600 + minutes * 60;
  return (true);
}

bool timerange::build_timeranges_from_string(std::string const& line,
                                             std::list<timerange>& timeranges) {
  std::list<std::string> timeranges_str;
  misc::string::split(line, timeranges_str, ',');
  for (std::list<std::string>::const_iterator it(timeranges_str.begin()),
       end(timeranges_str.end());
       it != end; ++it) {
    std::size_t pos(it->find('-'));
    if (pos == std::string::npos)
      return (false);
    unsigned long start_time;
    if (!_build_time_t(it->substr(0, pos), start_time))
      return (false);
    unsigned long end_time;
    if (!_build_time_t(it->substr(pos + 1), end_time))
      return (false);
    timeranges.push_front(timerange(start_time, end_time));
  }
  return (true);
}

std::string timerange::to_string() const {
  std::ostringstream oss;
  oss << (_start / 3600) << ":" << (_start % 3600) / 60 << "-" << (_end / 3600)
      << ":" << (_end % 3600) / 60;
  return (oss.str());
}

std::string timerange::build_string_from_timeranges(
    std::list<timerange> const& timeranges) {
  std::ostringstream oss;
  for (std::list<time::timerange>::const_iterator it = timeranges.begin(),
                                                  end = timeranges.end();
       it != end; ++it) {
    if (!oss.str().empty())
      oss << ",";
    oss << it->to_string();
  }
  return (oss.str());
}
