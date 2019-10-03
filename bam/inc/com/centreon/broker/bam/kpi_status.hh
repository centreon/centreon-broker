/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_KPI_STATUS_HH
#define CCB_BAM_KPI_STATUS_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class kpi_status kpi_status.hh "com/centreon/broker/bam/kpi_status.hh"
 *  @brief Update status of a KPI.
 *
 *  Update the status of a KPI, used to update the cfg_bam_kpi table.
 */
class kpi_status : public io::data {
 public:
  kpi_status();
  kpi_status(kpi_status const& other);
  ~kpi_status();
  kpi_status& operator=(kpi_status const& other);
  uint32_t type() const;
  static uint32_t static_type();

  uint32_t kpi_id;
  bool in_downtime;
  double level_acknowledgement_hard;
  double level_acknowledgement_soft;
  double level_downtime_hard;
  double level_downtime_soft;
  double level_nominal_hard;
  double level_nominal_soft;
  short state_hard;
  short state_soft;
  timestamp last_state_change;
  double last_impact;
  bool valid;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(kpi_status const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_KPI_STATUS_HH
