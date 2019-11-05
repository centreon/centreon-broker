/*
** Copyright 2011-2014 Centreon
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
 public:
  timerange(uint64_t start = 0, uint64_t end = 0);
  timerange(timerange const& right);
  ~timerange() throw();
  timerange& operator=(timerange const& right);
  bool operator==(timerange const& right) const throw();
  bool operator!=(timerange const& right) const throw();
  bool operator<(timerange const& right) const throw();
  uint64_t end() const throw();
  void end(uint64_t value);
  uint64_t start() const throw();
  void start(uint64_t value);
  uint64_t start_hour() const throw();
  uint64_t start_minute() const throw();
  uint64_t end_hour() const throw();
  uint64_t end_minute() const throw();

  bool to_time_t(struct tm const& midnight,
                 time_t& range_start,
                 time_t& range_end) const;

  static bool build_timeranges_from_string(std::string const& line,
                                           std::list<timerange>& timeranges);

  std::string to_string() const;
  static std::string build_string_from_timeranges(
      std::list<timerange> const& timeranges);

 private:
  uint64_t _end;
  uint64_t _start;
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_TIMERANGE_HH
