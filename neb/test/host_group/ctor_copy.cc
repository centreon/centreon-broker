/*
** Copyright 2012 Centreon
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

#include "com/centreon/broker/neb/host_group.hh"
#include "test/randomize.hh"

using namespace com::centreon::broker;

/**
 *  Check host_group's copy constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  randomize_init();

  // Object #1.
  neb::host_group hgrp1;
  std::vector<randval> randvals1;
  randomize(hgrp1, &randvals1);

  // Object #2.
  neb::host_group hgrp2(hgrp1);

  // Reset object #1.
  std::vector <randval> randvals2;
  randomize(hgrp1, &randvals2);

  // Compare objects with expected results.
  int retval((hgrp1 != randvals2) || (hgrp2 != randvals1));

  // Cleanup.
  randomize_cleanup();

  return (retval);
}
