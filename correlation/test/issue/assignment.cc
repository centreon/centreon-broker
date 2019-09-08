/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/correlation/issue.hh"

using namespace com::centreon::broker;

/**
 *  Check that issue can be properly assigned.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::issue i1;
  i1.ack_time = 248578;
  i1.end_time = 7456987;
  i1.host_id = 21;
  i1.service_id = 42;
  i1.start_time = 123456789;

  // Second object.
  correlation::issue i2;
  i2 = i1;

  // Reset first object.
  i1.ack_time = 1684474;
  i1.end_time = 5478963;
  i1.host_id = 983;
  i1.service_id = 211;
  i1.start_time = 456887;

  // Check.
  return ((i1.ack_time != 1684474) || (i1.end_time != 5478963) ||
          (i1.host_id != 983) || (i1.service_id != 211) ||
          (i1.start_time != 456887) || (i2.ack_time != 248578) ||
          (i2.end_time != 7456987) || (i2.host_id != 21) ||
          (i2.service_id != 42) || (i2.start_time != 123456789));
}
