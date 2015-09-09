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
#include <cstdlib>
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
      "baz=1234;@10:20; 'q u x'=9queries_per_second;@10:;@5:;0;100",
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
  {
    perfdata pd;
    pd.name("q u x");
    pd.value_type(perfdata::gauge);
    pd.value(9.0);
    pd.unit("queries_per_second");
    pd.warning(INFINITY);
    pd.warning_low(10.0);
    pd.warning_mode(true);
    pd.critical(INFINITY);
    pd.critical_low(5.0);
    pd.critical_mode(true);
    pd.min(0.0);
    pd.max(100.0);
    expected.push_back(pd);
  }

  // Check parsing.
  bool error(false);
  while (!error && !list.isEmpty() && !expected.isEmpty()) {
    error = (list.front() != expected.front());
    list.pop_front();
    expected.pop_front();
  }
  error = (error || !list.isEmpty() || !expected.isEmpty());

  // Cleanup.
  config::applier::deinit();

  // Return value.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
