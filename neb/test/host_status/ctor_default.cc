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
