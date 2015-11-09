/*
** Copyright 2012-2013,2015 Centreon
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
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_group.hh"

using namespace com::centreon::broker;

/**
 *  Check service_group's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::service_group sgrp;

  // Check.
  return ((sgrp.enabled != true)
          || (sgrp.instance_id != 0)
          || (sgrp.name != "")
          || (sgrp.type()
              != io::events::data_type<io::events::neb, neb::de_service_group>::value));
}
