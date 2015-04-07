/*
** Copyright 2013,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
  return ((s.active_checks_enabled
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
           || !s.enabled
           || !s.event_handler.isEmpty()
           || s.event_handler_enabled
           || (fabs(s.execution_time) > 0.001)
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
           || (s.instance_id != 0)
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
           || s.obsess_over
           || !s.output.isEmpty()
           || (fabs(s.percent_state_change) > 0.001)
           || !s.perf_data.isEmpty()
           || (fabs(s.retry_interval) > 0.001)
           || s.should_be_scheduled
           || (s.state_type != 0)
           || !s.service_description.isEmpty()
           || (s.service_id != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
