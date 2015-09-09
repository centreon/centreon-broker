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
#include "com/centreon/broker/neb/host_check.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check host_check's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::host_check hchk;

  // Check.
  return (((hchk.source_id != 0)
           || (hchk.destination_id != 0)
           || hchk.active_checks_enabled
           || (hchk.command_line != "")
           || (hchk.host_id != 0)
           || (hchk.next_check != 0)
           || (hchk.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_host_check>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
