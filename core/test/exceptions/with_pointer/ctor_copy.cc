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
 *  Check that copy constructor works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // First exception.
  exceptions::msg base;
  base << "foobar" << 42 << 7894561236549877ull
       << false << "baz  qux  ";
  misc::shared_ptr<io::data> dat(new io::raw);
  exceptions::with_pointer e1(base, dat);

  // Second object.
  exceptions::with_pointer e2(e1);

  // Check.
  return ((strcmp(e1.what(), base.what())
           || (e1.ptr().data() != dat.data())
           || strcmp(e2.what(), base.what())
           || (e2.ptr().data() != dat.data()))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
