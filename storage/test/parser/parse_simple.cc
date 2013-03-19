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

#include <cmath>
#include <QList>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**
 *  Check that the perfdata parser works as expected.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Objects.
  QList<perfdata> list;
  parser p;

  // Parse perfdata string.
  p.parse_perfdata(
    "time=2.45698s;2.000000;5.000000;0.000000;10.000000",
    list);

  // Expected perfdata.
  perfdata expected;
  expected.name("time");
  expected.value_type(perfdata::gauge);
  expected.value(2.45698);
  expected.unit("s");
  expected.warning(2.0);
  expected.warning_low(0.0);
  expected.critical(5.0);
  expected.critical_low(0.0);
  expected.min(0.0);
  expected.max(10.0);

  // Check parsing.
  bool error((list.size() != 1)
             || (list.front() != expected));

  // Cleanup.
  config::applier::deinit();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
