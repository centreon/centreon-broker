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

#include <cmath>
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the perfdata object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::perfdata p;

  // Check properties values.
  return (!isnan(p.critical())
          || !isnan(p.critical_low())
          || p.critical_mode()
          || !isnan(p.max())
          || !isnan(p.min())
          || !p.name().isEmpty()
          || !p.unit().isEmpty()
          || !isnan(p.value())
          || (p.value_type() != storage::perfdata::gauge)
          || !isnan(p.warning())
          || !isnan(p.warning_low())
          || p.warning_mode());
}
