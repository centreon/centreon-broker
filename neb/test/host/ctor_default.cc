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
           || !h.enabled
           || !h.event_handler.isEmpty()
           || h.event_handler_enabled
           || (fabs(h.execution_time) > 0.0001)
           || h.flap_detection_enabled
           || h.flap_detection_on_down
           || h.flap_detection_on_unreachable
           || h.flap_detection_on_up
           || (fabs(h.freshness_threshold) > 0.0001)
           || h.has_been_checked
           || (fabs(h.high_flap_threshold) > 0.0001)
           || (h.host_id != 0)
           || !h.host_name.isEmpty()
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
           || h.obsess_over
           || !h.output.isEmpty()
           || (fabs(h.percent_state_change) > 0.0001)
           || !h.perf_data.isEmpty()
           || (fabs(h.retry_interval) > 0.0001)
           || h.should_be_scheduled
           || (h.state_type != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
