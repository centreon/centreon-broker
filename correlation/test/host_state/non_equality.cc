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

#include "com/centreon/broker/correlation/host_state.hh"

using namespace com::centreon::broker;

/**
 *  Check that host_state can be checked for non-equality.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::host_state hs1;
  hs1.ack_time = 6762;
  hs1.current_state = 2;
  hs1.end_time = 7456987;
  hs1.host_id = 21;
  hs1.in_downtime = true;
  hs1.service_id = 38475;
  hs1.start_time = 123456789;

  // Second object.
  correlation::host_state hs2(hs1);

  // Reset first object.
  correlation::host_state hs3;
  hs3.ack_time = 834957;
  hs3.current_state = 1;
  hs3.end_time = 5478963;
  hs3.host_id = 983;
  hs3.in_downtime = false;
  hs3.service_id = 471;
  hs3.start_time = 456887;

  // Check.
  return ((hs1 != hs2) || !(hs1 != hs3) || !(hs2 != hs3) || (hs1 != hs1) ||
          (hs2 != hs2) || (hs3 != hs3));
}
