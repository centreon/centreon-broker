/*
** Copyright 2013 Merethis
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
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;

/**
 *  Check service_status' default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::service_status ss;

  // Check.
  return (((ss.acknowledgement_type != 0)
           || ss.active_checks_enabled
           || !ss.check_command.isEmpty()
           || (fabs(ss.check_interval) > 0.001)
           || !ss.check_period.isEmpty()
           || (ss.check_type != 0)
           || (ss.current_check_attempt != 0)
           || (ss.current_notification_number != 0)
           || (ss.current_state != 4)
           || !ss.enabled
           || !ss.event_handler.isEmpty()
           || ss.event_handler_enabled
           || (fabs(ss.execution_time) > 0.001)
           || ss.failure_prediction_enabled
           || ss.flap_detection_enabled
           || ss.has_been_checked
           || (ss.host_id != 0)
           || !ss.host_name.isEmpty()
           || (ss.instance_id != 0)
           || ss.is_flapping
           || (ss.last_check != 0)
           || (ss.last_hard_state != 4)
           || (ss.last_hard_state_change != 0)
           || (ss.last_notification != 0)
           || (ss.last_state_change != 0)
           || (ss.last_time_critical != 0)
           || (ss.last_time_ok != 0)
           || (ss.last_time_unknown != 0)
           || (ss.last_time_warning != 0)
           || (ss.last_update != 0)
           || (fabs(ss.latency) > 0.001)
           || (ss.max_check_attempts != 0)
           || (ss.modified_attributes != 0)
           || (ss.next_check != 0)
           || (ss.next_notification != 0)
           || (ss.no_more_notifications != 0)
           || ss.notifications_enabled
           || ss.obsess_over
           || !ss.output.isEmpty()
           || ss.passive_checks_enabled
           || (fabs(ss.percent_state_change) > 0.001)
           || !ss.perf_data.isEmpty()
           || ss.problem_has_been_acknowledged
           || ss.process_performance_data
           || (fabs(ss.retry_interval) > 0.001)
           || (ss.scheduled_downtime_depth != 0)
           || ss.should_be_scheduled
           || (ss.state_type != 0)
           || !ss.service_description.isEmpty()
           || (ss.service_id != 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
