/*
** Copyright 2014-2015, 2021 Centreon
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

#include "com/centreon/broker/bam/kpi_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 */
kpi_status::kpi_status(uint32_t kpi_id)
    : io::data(kpi_status::static_type()),
      kpi_id(kpi_id),
      in_downtime(false),
      level_acknowledgement_hard(0.0),
      level_acknowledgement_soft(0.0),
      level_downtime_hard(0.0),
      level_downtime_soft(0.0),
      level_nominal_hard(100.0),
      level_nominal_soft(100.0),
      state_hard(0),
      state_soft(0),
      last_state_change(0),
      last_impact(0),
      valid(true) {}

// Mapping.
mapping::entry const kpi_status::entries[] = {
    mapping::entry(&bam::kpi_status::kpi_id,
                   "kpi_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::kpi_status::in_downtime, "in_downtime"),
    mapping::entry(&bam::kpi_status::level_acknowledgement_hard,
                   "level_acknowledgement_hard"),
    mapping::entry(&bam::kpi_status::level_acknowledgement_soft,
                   "level_acknowledgement_soft"),
    mapping::entry(&bam::kpi_status::level_downtime_hard,
                   "level_downtime_hard"),
    mapping::entry(&bam::kpi_status::level_downtime_soft,
                   "level_downtime_soft"),
    mapping::entry(&bam::kpi_status::level_nominal_hard, "level_nominal_hard"),
    mapping::entry(&bam::kpi_status::level_nominal_soft, "level_nominal_soft"),
    mapping::entry(&bam::kpi_status::state_hard, "state_hard"),
    mapping::entry(&bam::kpi_status::state_soft, "state_soft"),
    mapping::entry(&bam::kpi_status::last_state_change, "last_state_change"),
    mapping::entry(&bam::kpi_status::last_impact, "last_impact"),
    mapping::entry(&bam::kpi_status::valid, "valid"),
    mapping::entry()};

// Operations.
static io::data* new_kpi_status() {
  return new kpi_status(0);
}
io::event_info::event_operations const kpi_status::operations = {
    &new_kpi_status};
