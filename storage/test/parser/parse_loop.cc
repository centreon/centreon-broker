/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
