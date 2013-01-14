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
 *  Check that the exception cloning works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Base exception.
  exceptions::msg base;
  base << "foo" << 42 << 77454654249841ull << -1 << "bar";
  misc::shared_ptr<io::data> dat(new io::raw);
  exceptions::with_pointer e(base, dat);

  // Clone object.
  std::auto_ptr<exceptions::msg> clone(e.clone());

  // Check that clone was properly constructed.
  bool error(!clone.get() || strcmp(e.what(), base.what()));

  // The base exception must be thrown.
  if (!error) {
    try {
      clone->rethrow();
    }
    catch (exceptions::msg const& e) {
      // Proper type catch, check content.
      error = (error || strcmp(e.what(), base.what()));
    }
    catch (...) {
      error = true;
    }
  }

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
