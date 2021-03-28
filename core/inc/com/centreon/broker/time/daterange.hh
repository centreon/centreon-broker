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

#ifndef CCB_CORE_TIME_DATERANGE_HH
#define CCB_CORE_TIME_DATERANGE_HH

#include <ctime>
#include <list>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/ptr_typedef.hh"
#include "com/centreon/broker/time/time_info.hh"
#include "com/centreon/broker/time/timerange.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class daterange daterange.hh "com/centreon/broker/time/daterange.hh"
 *  @brief Daterange object.
 *
 *  The object containing a daterange.
 */
class daterange {
 public:
  DECLARE_SHARED_PTR(daterange);

 private:
  std::list<timerange> _timeranges;

 public:
  daterange();
  ~daterange() noexcept = default;
  daterange(const daterange&) = delete;
  daterange& operator=(const daterange&) = delete;
  void timeranges(std::list<timerange>&& value);
  std::list<timerange> const& timeranges() const noexcept;

  static bool build_calendar_date(std::string const& line,
                                  std::list<daterange>& list);
  static bool build_dateranges_from_string(std::string const& value,
                                           std::list<daterange>& list);
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_DATERANGE_HH
