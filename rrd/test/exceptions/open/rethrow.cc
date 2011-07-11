/*
** Copyright 2011 Merethis
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

#include <string.h>
#include "com/centreon/broker/rrd/exceptions/open.hh"

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
      throw (rrd::exceptions::open()
               << "foobar" << 42 << -789654123ll);
      retval |= 1;
    }
    catch (exceptions::msg const& e) { // Proper catch.
      e.rethrow();
    }
  }
  catch (rrd::exceptions::open const& e) { // Catch rethrown exception.
    retval |= strcmp(e.what(), "foobar42-789654123");
  }
  catch (...) {
    retval |= 1;
  }

  // Return check result.
  return (retval);
}
