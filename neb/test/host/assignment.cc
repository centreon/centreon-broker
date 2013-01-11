/*
** Copyright 2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
