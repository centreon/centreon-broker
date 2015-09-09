/*
** Copyright 2013 Centreon
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

#include <cstdlib>
#include "com/centreon/broker/neb/host.hh"
#include "test/randomize.hh"

using namespace com::centreon::broker;

/**
 *  Check host's assignment operator.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Initialization.
  randomize_init();

  // Object #1.
  neb::host h1;
  std::vector<randval> randvals1;
  randomize(h1, &randvals1);

  // Object #2.
  neb::host h2;
  randomize(h2);

  // Assignment.
  h2 = h1;

  // Reset object #1.
  std::vector<randval> randvals2;
  randomize(h1, &randvals2);

  // Compare objects with expected results.
  bool error((h1 != randvals2) || (h2 != randvals1));

  // Cleanup.
  randomize_cleanup();

  // Exit.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
