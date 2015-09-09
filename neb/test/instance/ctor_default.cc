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
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check instance's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::instance i;

  // Check.
  return (((i.source_id != 0)
           || (i.destination_id != 0)
           || (i.engine != "")
           || (i.is_running != true)
           || (i.name != "")
           || (i.pid != 0)
           || (i.program_end != 0)
           || (i.program_start != 0)
           || (i.version != "")
           || (i.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_instance>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
