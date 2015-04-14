/*
** Copyright 2015 Merethis
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
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that default logs (external commands, internal messages, ...)
 *  are properly parsed.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse a service alert line.
  neb::set_log_data(le,
    "EXTERNAL COMMAND: SCHEDULE_FORCED_SVC_CHECK;MyHost;MyService;1428930446");

  // Check that it was properly parsed.
  return (((le.host_name != "")
           || (le.log_type != 0) // Default.
           || (le.msg_type != 5) // Default.
           || (le.output
               != "EXTERNAL COMMAND: SCHEDULE_FORCED_SVC_CHECK;MyHost;MyService;1428930446")
           || (le.retry != 0)
           || (le.service_description != "")
           || (le.status != 5))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
