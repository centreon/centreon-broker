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
 *  Chech that copy construction works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  misc::stringifier s1;
  s1 << 42 << 36987410258ull << "foobar baz qux";

  // Second object.
  misc::stringifier s2(s1);

  // Change first object.
  s1.reset();
  s1 << "qux bazbar foo" << 65478847451578ull << -3612;

  // Check data.
  return (strcmp(s1.data(), "qux bazbar foo65478847451578-3612")
          || strcmp(s2.data(), "4236987410258foobar baz qux"));
}
