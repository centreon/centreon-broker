/*
** Copyright 2012-2013,2015 Merethis
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
           || (is.address != "")
           || (is.check_hosts_freshness != false)
           || (is.check_services_freshness != false)
           || (is.description != "")
           || (is.event_handler_enabled != false)
           || (is.flap_detection_enabled != false)
           || (is.global_host_event_handler != "")
           || (is.global_service_event_handler != "")
           || (is.last_alive != 0)
           || (is.last_command_check != 0)
           || (is.obsess_over_hosts != false)
           || (is.obsess_over_services != false)
           || (is.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_instance_status>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
