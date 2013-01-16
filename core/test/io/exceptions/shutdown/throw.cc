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
 *  Check that exception is properly thrown.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // First throw.
  try {
    try {
      throw (io::exceptions::shutdown(true, false)
               << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& s) { // Properly caught.
      retval |= (strcmp(s.what(), "foobar42-789654")
                 || !s.is_in_shutdown()
                 || s.is_out_shutdown());
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (io::exceptions::shutdown(false, false)
               << -36 << "fooqux" << true);
      retval |= 1;
    }
    catch (exceptions::msg const& m) {
      retval |= strcmp(m.what(), "-36fooquxtrue");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw (io::exceptions::shutdown(true, true)
               << "bazqux" << -74125896321445ll << 36);
      retval |= 1;
    }
    catch (std::exception const& e) {
      retval |= strcmp(e.what(), "bazqux-7412589632144536");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Return test result.
  return (retval);
}
