/*
** Copyright 2013,2015 Centreon
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
#include <cstdlib>
#include "com/centreon/broker/neb/host_status.hh"

using namespace com::centreon::broker;

/**
 *  Check host_status' default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::host_status hs;

  // Check.
  return (((hs.source_id != 0)
           || (hs.destination_id != 0)
           || hs.active_checks_enabled
           || !hs.check_command.isEmpty()
           || (fabs(hs.check_interval) > 0.0001)
           || !hs.check_period.isEmpty()
           || (hs.check_type != 0)
           || (hs.current_check_attempt != 0)
           || (hs.current_state != 4)
           || !hs.enabled
           || !hs.event_handler.isEmpty()
           || hs.event_handler_enabled
           || (fabs(hs.execution_time) > 0.0001)
           || hs.failure_prediction_enabled
           || hs.flap_detection_enabled
           || hs.has_been_checked
           || (hs.host_id != 0)
           || hs.is_flapping
           || (hs.last_check != 0)
           || (hs.last_hard_state != 4)
           || (hs.last_hard_state_change != 0)
           || (hs.last_state_change != 0)
           || (hs.last_time_down != 0)
           || (hs.last_time_unreachable != 0)
           || (hs.last_time_up != 0)
           || (hs.last_update != 0)
           || (fabs(hs.latency) > 0.0001)
           || (hs.max_check_attempts != 0)
           || (hs.next_check != 0)
           || hs.notifications_enabled
           || hs.obsess_over
           || !hs.output.isEmpty()
           || (fabs(hs.percent_state_change) > 0.0001)
           || !hs.perf_data.isEmpty()
           || (fabs(hs.retry_interval) > 0.0001)
           || hs.should_be_scheduled
           || (hs.state_type != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
