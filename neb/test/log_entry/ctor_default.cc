/*
** Copyright 2012-2013 Centreon
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
#include "com/centreon/broker/neb/log_entry.hh"

using namespace com::centreon::broker;

/**
 *  Check log_entry's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::log_entry le;

  // Check.
  return (((le.source_id != 0)
           || (le.destination_id != 0)
           || (le.c_time != 0)
           || (le.host_id != 0)
           || (le.host_name != "")
           || (le.issue_start_time != 0)
           || (le.log_type != 0)
           || (le.msg_type != 0)
           || (le.output != "")
           || (le.poller_name != "")
           || (le.retry != 0)
           || (le.service_description != "")
           || (le.service_id != 0)
           || (le.status != 5)
           || (le.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_log_entry>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
