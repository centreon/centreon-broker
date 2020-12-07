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

#ifndef CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH
#define CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH

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
 *  @class dimension_truncate_table_signal dimension_truncate_table_signal.hh
 * "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
 *  @brief Dimension truncate table signal
 *
 *  This empty data signals the reporting stream
 *  to truncate the dimension tables.
 */
class dimension_truncate_table_signal : public io::data {
 public:
  bool update_started;

  dimension_truncate_table_signal(bool update);
  dimension_truncate_table_signal(const dimension_truncate_table_signal&) =
      delete;
  ~dimension_truncate_table_signal() noexcept = default;
  dimension_truncate_table_signal& operator=(
      const dimension_truncate_table_signal&) = delete;
  bool operator==(const dimension_truncate_table_signal&) const = delete;
  constexpr static uint32_t static_type() {
    return io::events::data_type<
        io::events::bam, bam::de_dimension_truncate_table_signal>::value;
  }

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_TRUNCATE_TABLE_SIGNAL_HH
