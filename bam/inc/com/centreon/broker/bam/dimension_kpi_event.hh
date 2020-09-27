/*
** Copyright 2014,2019-2020 Centreon
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

#ifndef CCB_BAM_DIMENSION_KPI_EVENT_HH
#define CCB_BAM_DIMENSION_KPI_EVENT_HH

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
 *  @class dimension_kpi_event dimension_kpi_event.hh
 * "com/centreon/broker/bam/dimension_kpi_event.hh"
 *  @brief Dimension Kpi event
 *
 */
class dimension_kpi_event : public io::data {
 public:
  dimension_kpi_event();
  dimension_kpi_event(dimension_kpi_event const& other);
  ~dimension_kpi_event();
  dimension_kpi_event& operator=(dimension_kpi_event const& other);
  bool operator==(dimension_kpi_event const& other) const;
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam,
                                 bam::de_dimension_kpi_event>::value;
  }

  unsigned kpi_id;
  uint32_t ba_id;
  std::string ba_name;
  uint32_t host_id;
  std::string host_name;
  uint32_t service_id;
  std::string service_description;
  uint32_t kpi_ba_id;
  std::string kpi_ba_name;
  uint32_t meta_service_id;
  std::string meta_service_name;
  uint32_t boolean_id;
  std::string boolean_name;
  double impact_warning;
  double impact_critical;
  double impact_unknown;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(dimension_kpi_event const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_KPI_EVENT_HH
