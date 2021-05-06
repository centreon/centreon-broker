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
#include <fmt/format.h>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::time;

/**
 *  Constructor.
 *
 *  @param[in] start The start time.
 *  @param[in] end   The end time.
 */
timerange::timerange(uint64_t start, uint64_t end) : _end(end), _start(start) {}

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
uint64_t timerange::end() const throw() {
  return (_end);
}

/**
 *  Set the end time.
 *
 *  @param[in] value The end time.
 */
void timerange::end(uint64_t value) {
  _end = value;
}

/**
 *  Get the start time.
 *
 *  @return The start time.
 */
uint64_t timerange::start() const throw() {
  return (_start);
}

/**
 *  Set the start time.
 *
 *  @param[in] value The strart time.
 */
void timerange::start(uint64_t value) {
  _start = value;
}

/**
 *  Get the hour when the timerange starts.
 *
 *  @return The hour when the timerange starts.
 */
uint64_t timerange::start_hour() const throw() {
  return (_start / (60 * 60));
}

/**
 *  Get the minute when the timerange starts.
 *
 *  @return The minute when the timerange starts.
 */
uint64_t timerange::start_minute() const throw() {
  return ((_start / 60) % 60);
}

/**
 *  Get the hour when the timerange ends.
 *
 *  @return The hour when the timerange ends.
 */
uint64_t timerange::end_hour() const throw() {
  return (_end / (60 * 60));
}

/**
 *  Get the minute when the timerange ends.
 *
 *  @return The minute when the timerange ends.
 */
uint64_t timerange::end_minute() const throw() {
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

static bool _build_time_t(const fmt::string_view& time_str, uint64_t& ret) {
  const char* endc = time_str.data() + time_str.size();
  const char* begin_str = time_str.data();
  char* endptr;
  char* endptr1;

  // move cursor while we meet blanks
  while (std::isspace(*begin_str)) { begin_str++; }

  uint64_t hours = strtoull(begin_str, &endptr, 10);
  
  if (endptr == begin_str || endptr + 2 >= endc || *endptr != ':') {
    log_v2::core()->error("parser timeranges: error while reading hours '{}' at {}.",
                          begin_str, endptr - begin_str);
    return false;
  }

  uint64_t minutes = strtoull(endptr + 1, &endptr1, 10);

  if (endptr1 == endptr + 1) {
    log_v2::core()->error("parser timeranges: error while reading minutes '{}' at {}.",
                          begin_str, endptr1 - begin_str);
    return false;
  }

  // move cursor while we meet blanks
  while (endptr1 < endc && std::isspace(*endptr1)) { endptr1++; }

  if (endptr1 != endc) {
    log_v2::core()->error("parser timeranges: error while reading end "
                          "of your timerange '{}' at {}.", begin_str,
                          endptr1 - begin_str);
    return false;
  }

  ret = hours * 3600 + minutes * 60;
  return true;
}

bool timerange::build_timeranges_from_string(const std::string& line,
                                             std::list<timerange>& timeranges) {
  if (line.empty())
    return true;

  std::list<fmt::string_view> timeranges_str{misc::string::split_sv(line, ',')};
  for (auto& t : timeranges_str) {
    const char* ret = strchr(t.data(), '-');
    if (ret == NULL)
      return false;
    uint64_t start_time;
    if (!_build_time_t(fmt::string_view(t.data(), ret - t.data()), start_time))
      return false;
    uint64_t end_time;
    if (!_build_time_t(
            fmt::string_view(ret + 1, t.size() - (ret - t.data()) - 1),
            end_time))
      return false;
    timeranges.emplace_front(start_time, end_time);
  }
  return true;
}

std::string timerange::to_string() const {
  return fmt::format("{:02d}:{:02d}-{:02d}:{:02d}", _start / 3600,
                     (_start % 3600) / 60, _end / 3600, (_end % 3600) / 60);
}

std::string timerange::build_string_from_timeranges(
    std::list<timerange> const& timeranges) {
  std::vector<std::string> v;
  for (auto it = timeranges.rbegin(); it != timeranges.rend(); ++it)
    v.emplace_back(it->to_string());
  return fmt::format("{}", fmt::join(v, ", "));
}
