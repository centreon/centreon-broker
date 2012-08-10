/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/neb/instance_status.hh"

using namespace com::centreon::broker;

/**
 *  Check instance_status's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::instance_status is;

  // Check.
  return ((is.active_host_checks_enabled != false)
          || (is.active_service_checks_enabled != false)
          || (is.address != "")
          || (is.check_hosts_freshness != false)
          || (is.check_services_freshness != false)
          || (is.daemon_mode != false)
          || (is.description != "")
          || (is.event_handler_enabled != false)
          || (is.failure_prediction_enabled != false)
          || (is.flap_detection_enabled != false)
          || (is.global_host_event_handler != "")
          || (is.global_service_event_handler != "")
          || (is.id != 0)
          || (is.last_alive != 0)
          || (is.last_command_check != 0)
          || (is.last_log_rotation != 0)
          || (is.modified_host_attributes != 0)
          || (is.modified_service_attributes != 0)
          || (is.notifications_enabled != false)
          || (is.obsess_over_hosts != false)
          || (is.obsess_over_services != false)
          || (is.passive_host_checks_enabled != false)
          || (is.passive_service_checks_enabled != false)
          || (is.process_performance_data != false)
          || (is.type()
              != "com::centreon::broker::neb::instance_status"));
}
