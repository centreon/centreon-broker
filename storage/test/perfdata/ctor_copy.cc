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
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the perfdata copy constructor works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::perfdata p1;
  p1.critical(42.0);
  p1.critical_low(-456.032);
  p1.critical_mode(false);
  p1.max(76.3);
  p1.min(567.2);
  p1.name("foo");
  p1.unit("bar");
  p1.value(52189.912);
  p1.value_type(storage::perfdata::counter);
  p1.warning(4548.0);
  p1.warning_low(42.42);
  p1.warning_mode(true);

  // Second object.
  storage::perfdata p2(p1);

  // Change first object.
  p1.critical(9432.5);
  p1.critical_low(1000.0001);
  p1.critical_mode(true);
  p1.max(123.0);
  p1.min(843.876);
  p1.name("baz");
  p1.unit("qux");
  p1.value(3485.9);
  p1.value_type(storage::perfdata::derive);
  p1.warning(3612.0);
  p1.warning_low(-987579.0);
  p1.warning_mode(false);

  // Check objects properties values.
  return ((fabs(p1.critical() - 9432.5) > 0.00001)
          || (fabs(p1.critical_low() - 1000.0001) > 0.00001)
          || !p1.critical_mode()
          || (fabs(p1.max() - 123.0) > 0.00001)
          || (fabs(p1.min() - 843.876) > 0.00001)
          || (p1.name() != "baz")
          || (p1.unit() != "qux")
          || (fabs(p1.value() - 3485.9) > 0.00001)
          || (p1.value_type() != storage::perfdata::derive)
          || (fabs(p1.warning() - 3612.0) > 0.00001)
          || (fabs(p1.warning_low() + 987579.0) > 0.01)
          || p1.warning_mode()
          || (fabs(p2.critical() - 42.0) > 0.00001)
          || (fabs(p2.critical_low() + 456.032) > 0.00001)
          || p2.critical_mode()
          || (fabs(p2.max() - 76.3) > 0.00001)
          || (fabs(p2.min() - 567.2) > 0.00001)
          || (p2.name() != "foo")
          || (p2.unit() != "bar")
          || (fabs(p2.value() - 52189.912) > 0.00001)
          || (p2.value_type() != storage::perfdata::counter)
          || (fabs(p2.warning() - 4548.0) > 0.00001)
          || (fabs(p2.warning_low() - 42.42) > 0.00001)
          || !p2.warning_mode());
}
