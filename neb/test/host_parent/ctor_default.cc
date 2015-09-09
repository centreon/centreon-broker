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
#include "com/centreon/broker/neb/host_parent.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check host_parent's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::host_parent hprnt;

  // Check.
  return ((hprnt.enabled != true)
          || (hprnt.host_id != 0)
          || (hprnt.instance_id != 0)
          || (hprnt.parent_id != 0)
          || (hprnt.type()
              != io::events::data_type<io::events::neb, neb::de_host_parent>::value));
}
