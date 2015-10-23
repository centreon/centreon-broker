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
#include "com/centreon/broker/neb/service.hh"

using namespace com::centreon::broker;

/**
 *  Check service's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::service s;

  // Check.
  return (((s.source_id != 0)
           || (s.destination_id != 0)
           || !s.action_url.isEmpty()
           || s.active_checks_enabled
           || !s.check_command.isEmpty()
           || s.check_freshness
           || (fabs(s.check_interval) > 0.001)
           || !s.check_period.isEmpty()
           || (s.check_type != 0)
           || (s.current_check_attempt != 0)
           || (s.current_state != 4)
           || s.default_active_checks_enabled
           || s.default_event_handler_enabled
           || s.default_flap_detection_enabled
           || s.default_notifications_enabled
           || s.default_passive_checks_enabled
           || !s.enabled
           || !s.event_handler.isEmpty()
           || s.event_handler_enabled
           || (fabs(s.execution_time) > 0.001)
           || (fabs(s.first_notification_delay) > 0.0001)
           || s.flap_detection_enabled
           || s.flap_detection_on_critical
           || s.flap_detection_on_ok
           || s.flap_detection_on_unknown
           || s.flap_detection_on_warning
           || (fabs(s.freshness_threshold) > 0.001)
           || s.has_been_checked
           || (fabs(s.high_flap_threshold) > 0.001)
           || (s.host_id != 0)
           || !s.host_name.isEmpty()
           || !s.icon_image.isEmpty()
           || !s.icon_image_alt.isEmpty()
           || s.is_flapping
           || s.is_volatile
           || (s.last_check != 0)
           || (s.last_hard_state != 4)
           || (s.last_hard_state_change != 0)
           || (s.last_state_change != 0)
           || (s.last_time_critical != 0)
           || (s.last_time_ok != 0)
           || (s.last_time_unknown != 0)
           || (s.last_time_warning != 0)
           || (s.last_update != 0)
           || (fabs(s.latency) > 0.001)
           || (fabs(s.low_flap_threshold) > 0.001)
           || (s.max_check_attempts != 0)
           || (s.next_check != 0)
           || !s.notes.isEmpty()
           || !s.notes_url.isEmpty()
           || s.notifications_enabled
           || (fabs(s.notification_interval) > 0.0001)
           || !s.notification_period.isEmpty()
           || s.notify_on_downtime
           || s.notify_on_flapping
           || s.notify_on_recovery
           || s.obsess_over
           || !s.output.isEmpty()
           || (fabs(s.percent_state_change) > 0.001)
           || !s.perf_data.isEmpty()
           || s.retain_nonstatus_information
           || s.retain_status_information
           || (fabs(s.retry_interval) > 0.001)
           || s.should_be_scheduled
           || (s.state_type != 0)
           || !s.service_description.isEmpty()
           || (s.service_id != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
