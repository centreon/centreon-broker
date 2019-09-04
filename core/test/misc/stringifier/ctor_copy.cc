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
 *  Chech that copy construction works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  misc::stringifier s1;
  s1 << 42 << 36987410258ull << "foobar baz qux";

  // Second object.
  misc::stringifier s2(s1);

  // Change first object.
  s1.reset();
  s1 << "qux bazbar foo" << 65478847451578ull << -3612;

  // Check data.
  return (strcmp(s1.data(), "qux bazbar foo65478847451578-3612")
          || strcmp(s2.data(), "4236987410258foobar baz qux"));
}
