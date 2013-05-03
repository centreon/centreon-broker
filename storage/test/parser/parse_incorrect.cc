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

#include <cmath>
#include <cstdlib>
#include <QList>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**
 *  Check that incorrect perfdata generate an error in the parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Objects.
  QList<perfdata> list;
  parser p;

  bool success;
  try {
    // Parse perfdata string.
    p.parse_perfdata(
        "metric1= 10 metric2=42",
        list);

    // No success.
    success = false;
  }
  catch (exceptions::msg const& e) {
    (void)e;
    success = true;
  }

  // Cleanup.
  config::applier::deinit();

  return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
