/*
** Copyright 2011-2012 Centreon
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
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric copy constructor works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::metric m1;
  m1.ctime = 123456789;
  m1.interval = 42;
  m1.is_for_rebuild = true;
  m1.metric_id = 24;
  m1.name = "foobar";
  m1.rrd_len = 180;
  m1.value = 4242.0;
  m1.value_type = 3;

  // Second object.
  storage::metric m2(m1);

  // Change first object.
  m1.ctime = 741258;
  m1.interval = 36;
  m1.is_for_rebuild = false;
  m1.metric_id = 12;
  m1.name = "bazqux";
  m1.rrd_len = 900;
  m1.value = 1234.0;
  m1.value_type = 2;

  // Check objects properties values.
  return ((m1.ctime != 741258)
          || (m1.interval != 36)
          || (m1.is_for_rebuild != false)
          || (m1.metric_id != 12)
          || (m1.name != "bazqux")
          || (m1.rrd_len != 900)
          || (fabs(m1.value - 1234.0) > 0.00001)
          || (m1.value_type != 2)
          || (m2.ctime != 123456789)
          || (m2.interval != 42)
          || (m2.is_for_rebuild != true)
          || (m2.metric_id != 24)
          || (m2.name != "foobar")
          || (m2.rrd_len != 180)
          || (fabs(m2.value - 4242.0) > 0.00001)
          || (m2.value_type != 3));
}
