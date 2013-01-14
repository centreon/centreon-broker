/*
** Copyright 2011-2013 Merethis
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
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**
 *  Chat that assignment operator works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  exceptions::msg e1;
  e1 << 4189545612ul << "foo   " << " bar" << -123456789ll;

  // Second object.
  exceptions::msg e2;
  e2 << "baz" << 42u << 123456 << -7410;

  // Assign.
  e2 = e1;

  // Update first object.
  e1 << "qux";

  // Check.
  return (strcmp(e1.what(), "4189545612foo    bar-123456789qux")
	  || (strcmp(e2.what(), "4189545612foo    bar-123456789")));
}
