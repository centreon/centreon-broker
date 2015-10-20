/*
** Copyright 2012-2013,2015 Centreon
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

#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/instance_status.hh"

using namespace com::centreon::broker;

/**
 *  Check instance_status's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::instance_status is;

  // Check.
  return (((is.source_id != 0)
           || (is.destination_id != 0)
           || (is.active_host_checks_enabled != false)
           || (is.active_service_checks_enabled != false)
           || (is.check_hosts_freshness != false)
           || (is.check_services_freshness != false)
           || (is.event_handler_enabled != false)
           || (is.failure_prediction_enabled != false)
           || (is.flap_detection_enabled != false)
           || (is.global_host_event_handler != "")
           || (is.global_service_event_handler != "")
           || (is.last_alive != (time_t)-1)
           || (is.last_command_check != (time_t)-1)
           || (is.notifications_enabled != false)
           || (is.obsess_over_hosts != false)
           || (is.obsess_over_services != false)
           || (is.passive_host_checks_enabled != false)
           || (is.passive_service_checks_enabled != false)
           || (is.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_instance_status>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
