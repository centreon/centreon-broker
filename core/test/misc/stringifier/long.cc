/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include <cstring>
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that long integer insertion works properly.
 *
 *  @return 0 on success.
 */
int main () {
  // Return value.
  int retval(0);

  // First insertion.
  misc::stringifier s;
  s << 42l;
  retval |= strcmp(s.data(), "42");

  // Second insertions.
  s << -7895456l << 236987l << -46l;
  retval |= strcmp(s.data(), "42-7895456236987-46");

  // Return test result.
  return (retval);
}
