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
      "time=2.45698s;;nan;;inf d[metric]=239765B/s;5;;-inf; "
      "infotraffic=18x;;;; a[foo]=1234;10;11: c[bar]=1234;~:10;20:30 "
      "baz=1234;@10:20;",
      list);

  // Expected result.
  QList<perfdata> expected;
  {
    perfdata pd;
    pd.name("time");
    pd.value_type(perfdata::gauge);
    pd.value(2.45698);
    pd.unit("s");
    pd.max(INFINITY);
    expected.push_back(pd);
  }
  {
    perfdata pd;
    pd.name("metric");
    pd.value_type(perfdata::derive);
    pd.value(239765);
    pd.unit("B/s");
    pd.warning(5.0);
    pd.warning_low(0.0);
    pd.min(-INFINITY);
    expected.push_back(pd);
  }
  {
    perfdata pd;
    pd.name("infotraffic");
    pd.value_type(perfdata::gauge);
    pd.value(18.0);
    pd.unit("x");
    expected.push_back(pd);
  }
  {
    perfdata pd;
    pd.name("foo");
    pd.value_type(perfdata::absolute);
    pd.value(1234.0);
    pd.warning(10.0);
    pd.warning_low(0.0);
    pd.critical(INFINITY);
    pd.critical_low(11.0);
    expected.push_back(pd);
  }
  {
    perfdata pd;
    pd.name("bar");
    pd.value_type(perfdata::counter);
    pd.value(1234.0);
    pd.warning(10.0);
    pd.warning_low(-INFINITY);
    pd.critical(30.0);
    pd.critical_low(20.0);
    expected.push_back(pd);
  }
  {
    perfdata pd;
    pd.name("baz");
    pd.value_type(perfdata::gauge);
    pd.value(1234.0);
    pd.warning(20.0);
    pd.warning_low(10.0);
    pd.warning_mode(true);
    expected.push_back(pd);
  }

  // Check parsing.
  int retval(0);
  while (!retval && !list.isEmpty() && !expected.isEmpty()) {
    retval = (list.front() != expected.front());
    list.pop_front();
    expected.pop_front();
  }
  retval |= (!list.isEmpty() || !expected.isEmpty());

  // Return value.
  return (retval ? EXIT_FAILURE : EXIT_SUCCESS);
}
