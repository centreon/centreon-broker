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
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that C-string insertion works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // First insertion.
  misc::stringifier s;
  s << "foo";
  retval |= strcmp(s.data(), "foo");

  // Second insertions.
  s << " bar" << " baz" << " qux";
  retval |= strcmp(s.data(), "foo bar baz qux");

  // Third insertion.
  s << static_cast<char const*>(NULL);
  retval |= strcmp(s.data(), "foo bar baz qux(null)");

  // Return test result.
  return (retval);
}
