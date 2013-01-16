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

#include <cstring>
#include "com/centreon/broker/rrd/exceptions/open.hh"

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
      throw (rrd::exceptions::open()
               << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (rrd::exceptions::open const& e) {
      retval |= strcmp(e.what(), "foobar42-789654");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (rrd::exceptions::open()
               << "baz" << 42 << "qux" << -789410l);
      retval |= 1;
    }
    catch (exceptions::msg const& e) {
      retval |= strcmp(e.what(), "baz42qux-789410");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw (rrd::exceptions::open()
               << "foobarbazqux" << -74125896321445ll << 36);
      retval |= 1;
    }
    catch (std::exception const& e) {
      retval |= strcmp(e.what(), "foobarbazqux-7412589632144536");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Return test result.
  return (retval);
}
