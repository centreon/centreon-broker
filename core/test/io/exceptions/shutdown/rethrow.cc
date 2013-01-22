/*
** Copyright 2011 Merethis
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

#include <cstring>
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker;

/**
 *  Check that the rethrow() interface works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  try {
    try {
      // Initial throw.
      throw (io::exceptions::shutdown(false, true)
               << "foobar" << 42 << -789654123ll);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& s) { // Proper catch.
      s.rethrow();
    }
  }
  catch (io::exceptions::shutdown const& s) { // Catch rethrow.
    retval = (strcmp(s.what(), "foobar42-789654123")
              || s.is_in_shutdown()
              || !s.is_out_shutdown());
  }
  catch (...) {
    retval |= 1;
  }

  // Return check result.
  return (retval);
}
