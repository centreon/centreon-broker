/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that inequality operator works with logger configuration.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  config::logger l1;
  l1.config(true);
  l1.debug(true);
  l1.error(true);
  l1.info(true);
  l1.level(logging::high);
  l1.max_size(7685165485157ull);
  l1.name("FooBar");
  l1.type(config::logger::standard);

  // Second object.
  config::logger l2(l1);

  // Third object.
  config::logger l3(l1);
  l3.config(false);

  // Check
  return ((l1 != l2) || !(l2 != l3));
}
