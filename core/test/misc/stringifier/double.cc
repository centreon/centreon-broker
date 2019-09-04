/*
 * Copyright 2011- 2019 Centreon (https://www.centreon.com/)
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
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that double insertion works properly.
 *
 *  @return 0 on success.
 */
int main () {
  // Return value.
  int retval(0);

  // First insertion.
  misc::stringifier s1;
  s1 << -36.0;
  retval |= (fabs(strtod(s1.data(), NULL) + 36.0) > 0.1);

  // Second insertions.
  misc::stringifier s2;
  s2 << 75697.248;
  retval |= (fabs(strtod(s2.data(), NULL) - 75697.248) > 0.1);

  // Return test result.
  return (retval);
}
