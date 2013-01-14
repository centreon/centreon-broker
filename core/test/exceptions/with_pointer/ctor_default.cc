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
 *  Check that default construction works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Base exception.
  exceptions::msg base_exception;
  base_exception << "SOME MESSAGE";

  // Pointer.
  misc::shared_ptr<io::data> data(new io::raw);

  // Construct with_pointer.
  exceptions::with_pointer e(base_exception, data);

  // Check.
  return ((strcmp(base_exception.what(), e.what())
           || (data.data() != e.ptr().data()))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
