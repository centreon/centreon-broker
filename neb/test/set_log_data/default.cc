/*
** Copyright 2015 Centreon
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
