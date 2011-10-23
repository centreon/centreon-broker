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
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that buffer reset works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // Insert data.
  misc::stringifier s;
  s << 42 << "foo" << -789456;
  retval |= strcmp(s.data(), "42foo-789456");

  // Reset buffer.
  s.reset();
  retval |= strcmp(s.data(), "");

  // Insert new data.
  s << "baz qux" << true << -741236;
  retval |= strcmp(s.data(), "baz quxtrue-741236");

  // Return test result.
  return (retval);
}
