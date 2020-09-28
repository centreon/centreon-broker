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

#ifndef CCB_BAM_DIMENSION_BA_EVENT_HH
#define CCB_BAM_DIMENSION_BA_EVENT_HH

#include <string>

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
 *  @class dimension_ba_event dimension_ba_event.hh
 * "com/centreon/broker/bam/dimension_ba_event.hh"
 *  @brief Dimension Ba event
 *
 */
class dimension_ba_event : public io::data {
 public:
  dimension_ba_event();
  dimension_ba_event(dimension_ba_event const& other);
  ~dimension_ba_event();
  dimension_ba_event& operator=(dimension_ba_event const& other);
  bool operator==(dimension_ba_event const& other) const;
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam,
                                 bam::de_dimension_ba_event>::value;
  }

  uint32_t ba_id;
  std::string ba_name;
  std::string ba_description;
  double sla_month_percent_crit;
  double sla_month_percent_warn;
  uint32_t sla_duration_crit;
  uint32_t sla_duration_warn;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(dimension_ba_event const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_BA_EVENT_HH
