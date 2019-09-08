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

#ifndef CCB_BAM_INTERNAL_HH
#define CCB_BAM_INTERNAL_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
// Data elements.
enum data_element {
  de_ba_status = 1,
  de_kpi_status,
  de_meta_service_status,
  de_ba_event,
  de_kpi_event,
  de_ba_duration_event,
  de_dimension_ba_event,
  de_dimension_kpi_event,
  de_dimension_ba_bv_relation_event,
  de_dimension_bv_event,
  de_dimension_truncate_table_signal,
  de_rebuild,
  de_dimension_timeperiod,
  de_dimension_ba_timeperiod_relation,
  de_dimension_timeperiod_exception,
  de_dimension_timeperiod_exclusion,
  de_inherited_downtime
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_INTERNAL_HH
