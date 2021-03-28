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
timerange::timerange(time_t start, time_t end) : _start{start}, _end{end} {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
timerange::timerange(const timerange& right)
    : _start{right._start}, _end{right._end} {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
timerange& timerange::operator=(const timerange& right) {
  if (this != &right) {
    _start = right._start;
    _end = right._end;
  }
  return *this;
}

/**
 *  Equal operator.
 *
 *  @param[in] right The object to compare.
 *
 *  @return True if the object are the same, otherwise false.
 */
bool timerange::operator==(const timerange& right) const noexcept {
  return _start == right._start && _end == right._end;
}

/**
 *  Not equal operator.
 *
 *  @param[in] right The object to compare.
 *
 *  @return True if the object are not the same, otherwise false.
 */
bool timerange::operator!=(const timerange& right) const noexcept {
  return !operator==(right);
}

/**
 *  Less-than operator.
 *
 *  @param[in] right Object to compare to.
 *
 *  @return True if this object is less than right.
 */
bool timerange::operator<(const timerange& right) const noexcept {
  if (_start != right._start)
    return _start < right._start;
  return _end < right._end;
}

/**
 *  Get the end time.
 *
 *  @return The end time.
 */
time_t timerange::end() const noexcept {
  return _end;
}

/**
 *  Get the start time.
 *
 *  @return The start time.
 */
uint64_t timerange::start() const noexcept {
  return _start;
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
bool timerange::to_time_t(const struct tm& midnight,
                          time_t& range_start,
                          time_t& range_end) const {
  struct tm my_tm;
  memcpy(&my_tm, &midnight, sizeof(my_tm));
  my_tm.tm_sec += _start;
  range_start = mktime(&my_tm);
  memcpy(&my_tm, &midnight, sizeof(my_tm));
  my_tm.tm_sec += _end;
  range_end = mktime(&my_tm);
  return true;
}

static bool _build_time_t(const std::string& time_str, uint64_t& ret) {
  const char* endc = time_str.data() + time_str.size();
  char* endptr;
  char* endptr1;
  uint64_t hours = strtoull(time_str.data(), &endptr, 10);
  if (endptr == time_str.c_str() || endptr + 2 >= endc || *endptr != ':')
    return false;
  uint64_t minutes = strtoull(endptr + 1, &endptr1, 10);
  if (endptr1 == endptr + 1)
    return false;
  ret = hours * 3600 + minutes * 60;
  return true;
}

bool timerange::build_timeranges_from_string(const std::string& line,
                                             std::list<timerange>& timeranges) {
  if (line.empty())
    return true;

  std::list<std::string> timeranges_str;
  timeranges_str = misc::string::split(line, ',');
  for (std::list<std::string>::const_iterator it(timeranges_str.begin()),
       end(timeranges_str.end());
       it != end; ++it) {
    std::size_t pos(it->find('-'));
    if (pos == std::string::npos)
      return false;
    uint64_t start_time;
    if (!_build_time_t(it->substr(0, pos), start_time))
      return false;
    uint64_t end_time;
    if (!_build_time_t(it->substr(pos + 1), end_time))
      return false;
    timeranges.push_front(timerange(start_time, end_time));
  }
  return true;
}

std::string timerange::to_string() const {
  return fmt::format("{}:{}-{}:{}", _start / 3600, (_start % 3600) / 60,
                     _end / 3600, (_end % 3600) / 60);
}

std::string timerange::build_string_from_timeranges(
    const std::list<timerange>& timeranges) {
  std::ostringstream oss;
  bool first = true;
  for (std::list<time::timerange>::const_iterator it = timeranges.begin(),
                                                  end = timeranges.end();
       it != end; ++it) {
    if (first)
      first = false;
    else
      oss << ",";
    oss << it->to_string();
  }
  return oss.str();
}
