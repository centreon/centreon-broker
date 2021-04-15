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

#ifndef CCB_BAM_BA_EVENT_HH
#define CCB_BAM_BA_EVENT_HH

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
 *  @class ba_event ba_event.hh "com/centreon/broker/bam/ba_event.hh"
 *  @brief Ba event
 *
 */
class ba_event : public io::data {
 public:
  ba_event();
  ba_event(ba_event const& other);
  ~ba_event() noexcept = default;
  ba_event& operator=(ba_event const& other);
  bool operator==(ba_event const& other) const;
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam, bam::de_ba_event>::value;
  }
  uint32_t ba_id;
  double first_level;
  timestamp end_time;
  bool in_downtime;
  timestamp start_time;
  short status;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(ba_event const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BA_EVENT_HH
