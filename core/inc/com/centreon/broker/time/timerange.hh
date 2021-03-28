/*
** Copyright 2011-2014, 2021 Centreon
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

#ifndef CCB_CORE_TIME_TIMERANGE_HH
#define CCB_CORE_TIME_TIMERANGE_HH

#include <ctime>
#include <list>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class timerange timerange.hh "com/centreon/broker/time/timerange.hh"
 *  @brief Timerange object.
 *
 *  The object containing a timerange.
 */
class timerange {
  time_t _start;
  time_t _end;

 public:
  timerange(time_t start, time_t end);
  ~timerange() noexcept = default;
  timerange(const timerange&) = delete;
  timerange& operator=(const timerange&) = delete;
  time_t end() const noexcept;
  uint64_t start() const noexcept;

  bool to_time_t(struct tm const& midnight,
                 time_t& range_start,
                 time_t& range_end) const;

  static bool build_timeranges_from_string(std::string const& line,
                                           std::list<timerange>& timeranges);

  std::string to_string() const;
  static std::string build_string_from_timeranges(
      const std::list<timerange>& timeranges);
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_TIMERANGE_HH
