/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/state.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;

/**
 *  Check that service_state is properly default-constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  correlation::state ss;

  // Check default construction.
  return ((ss.ack_time != -1) || (ss.current_state != 3) ||
          (ss.end_time != -1) || (ss.host_id != 0) ||
          (ss.in_downtime != false) || (ss.service_id != 0) ||
          (ss.start_time != 0) ||
          (ss.type() != io::events::data_type<io::events::correlation,
                                              correlation::de_state>::value));
}
