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
 *  Check that assignment operator works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // First object.
  exceptions::msg base1;
  base1 << 4189545612ul << "foo   " << " bar" << -123456789ll;
  misc::shared_ptr<io::data> data1(new io::raw);
  exceptions::with_pointer e1(base1, data1);

  // Second object.
  exceptions::msg base2;
  base2 << "baz" << 42u << 123456 << -7410;
  misc::shared_ptr<io::data> data2(new io::raw);
  exceptions::with_pointer e2(base2, data2);

  // Third object.
  exceptions::msg base3;
  base3 << -3612.32 << " qux ";
  misc::shared_ptr<io::data> data3(new io::raw);
  exceptions::with_pointer e3(base3, data3);

  // Assignments.
  e2 = e1;
  e1 = e3;

  // Check.
  return ((strcmp(e1.what(), base3.what())
           || (e1.ptr().data() != data3.data())
           || strcmp(e2.what(), base1.what())
           || (e2.ptr().data() != data1.data())
           || strcmp(e3.what(), base3.what())
           || (e3.ptr().data() != data3.data()))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
