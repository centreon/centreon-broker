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

  // Multiple parsing.
  int retval(0);
  for (unsigned int i = 0; i < 10000; ++i) {
    // Parse perfdata string.
    list.clear();
    p.parse_perfdata(
      "c[time]=2.45698s;2.000000;5.000000;0.000000;10.000000",
      list);

    // Check parsing.
    retval |= (list.size() != 1);
    perfdata& pd(list.front());
    retval |= ((fabs(pd.critical() - 5.0) > 0.000001)
      || (fabs(pd.max() - 10.0) > 0.000001)
      || (fabs(pd.min()) > 0.000001)
      || (pd.name() != "time")
      || (pd.unit() != "s")
      || (fabs(pd.value() - 2.45698) > 0.000001)
      || (pd.value_type() != perfdata::counter)
      || (fabs(pd.warning() - 2.0) > 0.000001));
  }
  return (retval);
}
