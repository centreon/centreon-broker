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
 *  Check that exception is properly thrown.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Error flag.
  bool error(false);

  // First throw.
  try {
    try {
      exceptions::msg base;
      base << "foobar" << 42 << -789654ll;
      misc::shared_ptr<io::data> dat(new io::raw);
      throw (exceptions::with_pointer(base, dat));
      error = true;
    }
    catch (exceptions::with_pointer const& e) { // Properly caught.
      error = (error || strcmp(e.what(), "foobar42-789654"));
    }
  }
  catch (...) {
    error = true;
  }

  // Second throw.
  try {
    try {
      exceptions::msg base;
      base << "bazqux" << -74125896321445ll << 36;
      misc::shared_ptr<io::data> dat(new io::raw);
      throw (exceptions::with_pointer(base, dat));
      error = true;
    }
    catch (std::exception const& e) {
      error = (error || strcmp(e.what(), "bazqux-7412589632144536"));
    }
  }
  catch (...) {
    error = true;
  }

  // Return test result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
