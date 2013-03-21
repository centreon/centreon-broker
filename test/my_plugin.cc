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
#include <iostream>

/**
 *  This is a fake plugin that return the requested return code and
 *  output. By default return 0 and print nothing.
 *
 *  @return The configured return value, 0 by default.
 */
int main(int argc, char* argv[]) {
  int retval(0);
  char const* output("");
  if (argc > 1)
    retval = strtol(argv[1], NULL, 0);
  if (argc > 2)
    output = argv[2];
  std::cout << output;
  return (retval);
}
