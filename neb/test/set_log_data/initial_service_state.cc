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

#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that an initial service state log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse an initial service state line.
  neb::set_log_data(le,
    "INITIAL SERVICE STATE: myserver;myservice;UNKNOWN;SOFT;1;ERROR when getting SNMP version");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 0) // SOFT
          || (le.msg_type != 8) // INITIAL SERVICE STATE
          || (le.output != "ERROR when getting SNMP version")
          || (le.retry != 1)
          || (le.service_description != "myservice")
          || (le.status != 3)); // UNKNOWN
}
