/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_AVAILABILITY_BUILDER_HH
#define CCB_BAM_AVAILABILITY_BUILDER_HH

#include <map>
#include <memory>
#include <set>
#include <string>

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/timeperiod.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class availability_builder availability_builder.hh
 * "com/centreon/broker/bam/availability_builder.hh"
 *  @brief Availability builder.
 *
 *  Build availabilities based on events.
 *
 *  The goal of the builder is to fill the mod_bam_reporting_ba_availabilities
 *  table.
 *  Each row is indiced by a pair (ba_id, time_id) where time_id is the
 *  beginning of the day at 00:00AM.
 *
 *  Columns available, degraded, unavailable and unknown contain durations in
 *  seconds. The `available` column contains the duration the ba was available.
 *
 *  There are also the columns alert_unavailable_opened, alert_degraded_opened,
 *  alert_unknown_opened that are counters. Each one contain a number of
 *  events during the day, for example alert_unavailable_opened contains the
 *  number of `unavailable` events received during the day.
 */
class availability_builder {
 public:
  availability_builder(time_t ending_point, time_t starting_point = 0);
  ~availability_builder();
  availability_builder(const availability_builder&) = delete;
  availability_builder& operator=(const availability_builder&) = delete;

  void add_event(short status,
                 time_t start,
                 time_t end,
                 bool was_in_downtime,
                 time::timeperiod::ptr const& tp);

  int get_available() const;
  int get_unavailable() const;
  int get_degraded() const;
  int get_unknown() const;
  int get_downtime() const;
  int get_unavailable_opened() const;
  int get_degraded_opened() const;
  int get_unknown_opened() const;
  int get_downtime_opened() const;

  void set_timeperiod_is_default(bool val);
  bool get_timeperiod_is_default() const;

 private:
  time_t _start;
  time_t _end;
  int _available;
  int _unavailable;
  int _degraded;
  int _unknown;
  int _downtime;
  int _alert_unavailable_opened;
  int _alert_degraded_opened;
  int _alert_unknown_opened;
  int _nb_downtime;

  bool _timeperiods_is_default;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_AVAILABILITY_BUILDER_HH
