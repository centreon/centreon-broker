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
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_dependency.hh"

using namespace com::centreon::broker;

/**
 *  Check service_dependency's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::service_dependency sdep;

  // Check.
  return ((sdep.dependency_period != "")
          || (sdep.dependent_host_id != 0)
          || (sdep.enabled != true)
          || (sdep.execution_failure_options != "")
          || (sdep.host_id != 0)
          || (sdep.inherits_parent != false)
          || (sdep.instance_id != 0)
          || (sdep.notification_failure_options != "")
          || (sdep.service_id != 0)
          || (sdep.type()
              != io::events::data_type<io::events::neb, neb::de_service_dependency>::value));
}
