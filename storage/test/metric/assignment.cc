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
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric assignment operator works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::metric m1;
  m1.ctime = 123456789;
  m1.interval = 42;
  m1.metric_id = 24;
  m1.name = "foo";
  m1.rrd_len = 180;
  m1.value = 4242.0;
  m1.value_type = 1;

  // Second object.
  storage::metric m2;
  m2.ctime = 654123;
  m2.interval = 78;
  m2.metric_id = 33;
  m2.name = "bar";
  m2.rrd_len = 666;
  m2.value = 987.0;
  m2.value_type = 3;

  // Assignment.
  m2 = m1;

  // Change first object.
  m1.ctime = 741258;
  m1.interval = 36;
  m1.metric_id = 12;
  m1.name = "baz";
  m1.rrd_len = 900;
  m1.value = 1234.0;
  m1.value_type = 2;

  // Check objects properties values.
  return ((m1.ctime != 741258)
          || (m1.interval != 36)
          || (m1.metric_id != 12)
          || (m1.name != "baz")
          || (m1.rrd_len != 900)
          || (fabs(m1.value - 1234.0) > 0.00001)
          || (m1.value_type != 2)
          || (m2.ctime != 123456789)
          || (m2.interval != 42)
          || (m2.metric_id != 24)
          || (m2.name != "foo")
          || (m2.rrd_len != 180)
          || (fabs(m2.value - 4242.0) > 0.00001)
          || (m2.value_type != 1));
}
