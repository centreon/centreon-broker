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

#include <cmath>
#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/flapping_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check flapping_status's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::flapping_status flappy;

  // Check.
  return (((flappy.source_id != 0)
           || (flappy.destination_id != 0)
           || (flappy.event_time != 0)
           || (flappy.event_type != 0)
           || (flappy.flapping_type != 0)
           || (fabs(flappy.high_threshold) > 0.000001)
           || (flappy.host_id != 0)
           || (fabs(flappy.low_threshold) > 0.000001)
           || (fabs(flappy.percent_state_change) > 0.000001)
           || (flappy.reason_type != 0)
           || (flappy.service_id != 0)
           || (flappy.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_flapping_status>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
