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

#include <string.h>
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker;

/**
 *  Check that copy constructor works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  io::exceptions::shutdown s1(true, false);
  s1 << "foobar" << 42 << 7894561236549877ull << false << "baz  qux  ";

  // Second object.
  io::exceptions::shutdown s2(s1);

  // Update first object.
  s1 << "another string";

  // Check.
  return (strcmp(
      s1.what(),
      "foobar427894561236549877falsebaz  qux  another string")
          || strcmp(
      s2.what(),
      "foobar427894561236549877falsebaz  qux  ")
          || !s1.is_in_shutdown()
          || s1.is_out_shutdown()
          || !s2.is_in_shutdown()
          || s2.is_out_shutdown());
}
