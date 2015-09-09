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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check downtime's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::downtime dwntm;

  // Check.
  return ((dwntm.actual_end_time != 0)
          || (dwntm.actual_start_time != 0)
          || (dwntm.author != "")
          || (dwntm.comment != "")
          || (dwntm.deletion_time != 0)
          || (dwntm.downtime_type != 0)
          || (dwntm.duration != 0)
          || (dwntm.end_time != 0)
          || (dwntm.entry_time != 0)
          || (dwntm.fixed != true)
          || (dwntm.host_id != 0)
          || (dwntm.instance_id != 0)
          || (dwntm.internal_id != 0)
          || (dwntm.service_id != 0)
          || (dwntm.start_time != 0)
          || (dwntm.triggered_by != 0)
          || (dwntm.was_cancelled != false)
          || (dwntm.was_started != false)
          || (dwntm.type()
              != io::events::data_type<io::events::neb, neb::de_downtime>::value));
}
