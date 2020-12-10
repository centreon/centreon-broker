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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_HH
#define CCB_BAM_DIMENSION_TIMEPERIOD_HH

#include <string>

#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class dimension_timeperiod dimension_timeperiod.hh
 * "com/centreon/broker/bam/dimension_timeperiod.hh"
 *  @brief Timeperiod dimension
 *
 */
class dimension_timeperiod : public io::data {
 public:
  uint32_t id;
  std::string name;
  std::string monday;
  std::string tuesday;
  std::string wednesday;
  std::string thursday;
  std::string friday;
  std::string saturday;
  std::string sunday;

  dimension_timeperiod(uint32_t id, const std::string& name);
  ~dimension_timeperiod() noexcept = default;
  dimension_timeperiod(const dimension_timeperiod&) = delete;
  dimension_timeperiod& operator=(const dimension_timeperiod&) = delete;
  bool operator==(const dimension_timeperiod&) const = delete;
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod>::value;
  }

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_TIMEPERIOD_HH
