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

#include <QScopedPointer>
#include <string.h>
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  io::exceptions::shutdown s(true, false);
  s << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  QScopedPointer<exceptions::msg> clone(s.clone());

  // Check that clone was properly constructed.
  int retval(clone.isNull());

  // Check that this is really a shutdown exception object.
  if (!retval) {
    try {
      clone->rethrow();
    }
    catch (io::exceptions::shutdown const& s) {
      retval |= (
        strcmp("foo4277454654249841-1bar", s.what())
        || !s.is_in_shutdown()
        || s.is_out_shutdown());
    }
    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  return (retval);
}
