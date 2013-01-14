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
#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

/**
 *  Check that the rethrow() interface works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(false);

  try {
    try {
      // Initial throw.
      exceptions::msg base;
      base << "foobar" << 42 << -789654123ll;
      misc::shared_ptr<io::data> dat(new io::raw);
      throw (exceptions::with_pointer(base, dat));
      error = true;
    }
    catch (exceptions::with_pointer const& e) { // Proper catch.
      e.rethrow();
    }
  }
  catch (exceptions::msg const& e) { // The base exception is rethrow.
    error = (error || strcmp(e.what(), "foobar42-789654123"));
  }
  catch (...) {
    error = true;
  }

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
