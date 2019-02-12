/*
** Copyright 2011-2013 Centreon
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

#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::metric m;

  // Check properties values.
  return ((m.ctime != 0)
          || (m.interval != 0)
          || (m.is_for_rebuild != false)
          || (m.metric_id != 0)
          || !m.name.isEmpty()
          || (m.rrd_len != 0)
          || !std::isnan(m.value)
          || (m.value_type != storage::perfdata::gauge)
          || (m.type()
              != io::events::data_type<io::events::storage, storage::de_metric>::value));
}
