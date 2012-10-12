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

  // Parse perfdata string.
  p.parse_perfdata(
      "time=2.45698s;;nan;;inf d[metric]=239765B/s;5;;-inf;",
      list);

  // Check parsing.
  int retval(list.size() != 2);
  perfdata& pd1(list.front());
  perfdata& pd2(*(++list.begin()));
  retval |= (!isnan(pd1.critical())
             || !isinf(pd1.max())
             || !isnan(pd1.min())
             || (pd1.name() != "time")
             || (pd1.unit() != "s")
             || (fabs(pd1.value() - 2.45698) > 0.000001)
             || (pd1.value_type() != perfdata::gauge)
             || !isnan(pd1.warning())
             || !isnan(pd2.critical())
             || !isnan(pd2.max())
             || !isinf(pd2.min())
             || (pd2.name() != "metric")
             || (pd2.unit() != "B/s")
             || (fabs(pd2.value() - 239765) > 0.000001)
             || (pd2.value_type() != perfdata::derive)
             || (pd2.warning() != 5));
  return (retval);
}
