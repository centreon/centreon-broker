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
 *  Chat that assignment operator works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  io::exceptions::shutdown s1(false, true);
  s1 << 4189545612ul << "foo   " << " bar" << -123456789ll;

  // Second object.
  io::exceptions::shutdown s2(false, false);
  s2 << "baz" << 42u << 123456 << -7410;

  // Assign.
  s2 = s1;

  // Update first object.
  s1 << "qux";

  // Check.
  return (strcmp(s1.what(), "4189545612foo    bar-123456789qux")
	  || (strcmp(s2.what(), "4189545612foo    bar-123456789"))
	  || s1.is_in_shutdown()
	  || !s1.is_out_shutdown()
	  || s2.is_in_shutdown()
	  || !s2.is_out_shutdown());
}
