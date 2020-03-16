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

#ifndef CCB_BAM_BA_DURATION_EVENT_HH
#define CCB_BAM_BA_DURATION_EVENT_HH

#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class ba_duration_event ba_duration_event.hh
 * "com/centreon/broker/bam/ba_duration_event.hh"
 *  @brief Ba duration event
 *
 */
class ba_duration_event : public io::data {
 public:
  ba_duration_event();
  ba_duration_event(ba_duration_event const& other);
  ~ba_duration_event();
  ba_duration_event& operator=(ba_duration_event const& other);
  bool operator==(ba_duration_event const& other) const;
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam,
                                 bam::de_ba_duration_event>::value;
  }

  uint32_t ba_id;
  timestamp real_start_time;
  timestamp end_time;
  timestamp start_time;
  uint32_t duration;
  uint32_t sla_duration;
  uint32_t timeperiod_id;
  bool timeperiod_is_default;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(ba_duration_event const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BA_DURATION_EVENT_HH
