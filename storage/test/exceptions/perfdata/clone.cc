/*
** Copyright 2011-2012 Merethis
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

#include <memory>
#include <string.h>
#include "com/centreon/broker/storage/exceptions/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  storage::exceptions::perfdata e;
  e << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  std::auto_ptr<exceptions::msg> clone(e.clone());

  // Check that clone object was properly constructed.
  int retval(!clone.get()
             || strcmp("foo4277454654249841-1bar", clone->what()));

  // Check that this is really a perfdata exception object.
  if (!retval) {
    try {
      clone->rethrow();
    }
    catch (storage::exceptions::perfdata const& e) {
      (void)e; // We're good.
    }
    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  return (retval);
}
