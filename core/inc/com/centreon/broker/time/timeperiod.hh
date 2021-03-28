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

#ifndef CCB_CORE_TIME_TIMEPERIOD_HH
#define CCB_CORE_TIME_TIMEPERIOD_HH

#include <array>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/daterange.hh"
#include "com/centreon/broker/time/timerange.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class timeperiod timeperiod.hh "com/centreon/broker/time/timeperiod.hh"
 *  @brief Timeperiod object.
 *
 *  The object containing a timeperiod.
 */
class timeperiod {
  const uint32_t _id;
  std::string _alias;

 public:
  DECLARE_SHARED_PTR(timeperiod);

  class exclusion_backup {
    timeperiod* _tp;
    std::vector<timeperiod::ptr> _exclusions;

   public:
    exclusion_backup(timeperiod* tp) {
      _tp = tp;
      _exclusions = _tp->_exclude;
      _tp->_exclude.clear();
    }

    ~exclusion_backup() { _tp->_exclude = _exclusions; }

    std::vector<timeperiod::ptr>::const_iterator begin() {
      return (_exclusions.begin());
    }

    std::vector<timeperiod::ptr>::const_iterator end() {
      return (_exclusions.end());
    }
  };

  friend class exclusion_backup;

  timeperiod(uint32_t id);
  timeperiod(uint32_t id,
             const std::string& name,
             const std::string& alias,
             const std::string& sunday,
             const std::string& monday,
             const std::string& tuesday,
             const std::string& wednesday,
             const std::string& thursday,
             const std::string& friday,
             const std::string& saturday);
  timeperiod(timeperiod const& obj);
  timeperiod operator=(timeperiod const& obj);

  uint32_t get_id() const noexcept;

  const std::string& get_alias() const noexcept;
  void set_alias(const std::string& value);

  std::vector<std::list<daterange> > const& get_exceptions() const noexcept;
  std::list<daterange> const& get_exceptions_from_type(int type) const;
  void add_exceptions(std::list<daterange> const& val);
  bool add_exception(const std::string& days, const std::string& range);

  std::vector<timeperiod::ptr> const& get_included() const noexcept;
  void add_included(ptr val);
  std::vector<ptr> const& get_excluded() const noexcept;
  void add_excluded(ptr val);

  const std::string& get_name() const noexcept;
  void set_name(const std::string& value);

  std::array<std::list<timerange>, 7> const& get_timeranges() const noexcept;
  bool set_timerange(const std::string& timerange_text, int day);
  std::list<timerange> const& get_timeranges_by_day(int day) const noexcept;

  const std::string& get_timezone() const noexcept;
  void set_timezone(const std::string& tz);

  bool is_valid(time_t preferred_time) const;
  time_t get_next_valid(time_t preferred_time) const;
  time_t get_next_invalid(time_t preferred_time) const;

  uint32_t duration_intersect(time_t start_time, time_t end_time) const;

  static time_t add_round_days_to_midnight(time_t midnight, long long skip);

 private:
  std::vector<std::list<daterange> > _exceptions;
  std::vector<ptr> _exclude;
  std::vector<ptr> _include;
  std::string _timeperiod_name;
  std::array<std::list<timerange>, 7> _timeranges;
  std::string _timezone;
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_TIMEPERIOD_HH
