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
#include "com/centreon/broker/neb/host.hh"

using namespace com::centreon::broker;

/**
 *  Check host's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::host h;

  // Check.
  return (((h.source_id != 0)
           || (h.destination_id != 0)
           || !h.action_url.isEmpty()
           || h.active_checks_enabled
           || !h.address.isEmpty()
           || !h.alias.isEmpty()
           || !h.check_command.isEmpty()
           || (fabs(h.check_interval) > 0.0001)
           || h.check_freshness
           || !h.check_period.isEmpty()
           || (h.check_type != 0)
           || (h.current_check_attempt != 0)
           || (h.current_state != 4)
           || h.default_active_checks_enabled
           || h.default_event_handler_enabled
           || h.default_flap_detection_enabled
           || h.default_notifications_enabled
           || h.default_passive_checks_enabled
           || !h.enabled
           || !h.event_handler.isEmpty()
           || h.event_handler_enabled
           || (fabs(h.execution_time) > 0.0001)
           || (fabs(h.first_notification_delay) > 0.0001)
           || h.flap_detection_enabled
           || h.flap_detection_on_down
           || h.flap_detection_on_unreachable
           || h.flap_detection_on_up
           || (fabs(h.freshness_threshold) > 0.0001)
           || h.has_been_checked
           || (fabs(h.high_flap_threshold) > 0.0001)
           || (h.host_id != 0)
           || !h.host_name.isEmpty()
           || !h.icon_image.isEmpty()
           || !h.icon_image_alt.isEmpty()
           || h.is_flapping
           || (h.last_check != 0)
           || (h.last_hard_state != 4)
           || (h.last_hard_state_change != 0)
           || (h.last_state_change != 0)
           || (h.last_time_down != 0)
           || (h.last_time_unreachable != 0)
           || (h.last_time_up != 0)
           || (h.last_update != 0)
           || (fabs(h.latency) > 0.0001)
           || (fabs(h.low_flap_threshold) > 0.0001)
           || (h.max_check_attempts != 0)
           || (h.next_check != 0)
           || !h.notes.isEmpty()
           || !h.notes_url.isEmpty()
           || h.notifications_enabled
           || (fabs(h.notification_interval) > 0.0001)
           || !h.notification_period.isEmpty()
           || h.notify_on_downtime
           || h.notify_on_flapping
           || h.notify_on_recovery
           || h.obsess_over
           || !h.output.isEmpty()
           || (fabs(h.percent_state_change) > 0.0001)
           || !h.perf_data.isEmpty()
           || h.retain_nonstatus_information
           || h.retain_status_information
           || (fabs(h.retry_interval) > 0.0001)
           || h.should_be_scheduled
           || (h.state_type != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
