/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <gtest/gtest.h>
#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric assignment operator works properly.
 */
TEST(StorageMetric, Assign) {
  // First object.
  storage::metric m1;
  m1.ctime = 123456789;
  m1.interval = 42;
  m1.is_for_rebuild = true;
  m1.metric_id = 24;
  m1.name = "foo";
  m1.rrd_len = 180;
  m1.value = 4242.0;
  m1.value_type = 1;

  // Second object.
  storage::metric m2;
  m2.ctime = 654123;
  m2.interval = 78;
  m2.is_for_rebuild = false;
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
  m1.is_for_rebuild = false;
  m1.metric_id = 12;
  m1.name = "baz";
  m1.rrd_len = 900;
  m1.value = 1234.0;
  m1.value_type = 2;

  // Check objects properties values.
  ASSERT_FALSE(m1.ctime != 741258);
  ASSERT_FALSE(m1.interval != 36);
  ASSERT_FALSE(m1.is_for_rebuild != false);
  ASSERT_FALSE(m1.metric_id != 12);
  ASSERT_FALSE(m1.name != "baz");
  ASSERT_FALSE(m1.rrd_len != 900);
  ASSERT_FALSE(fabs(m1.value - 1234.0) > 0.00001);
  ASSERT_FALSE(m1.value_type != 2);
  ASSERT_FALSE(m2.ctime != 123456789);
  ASSERT_FALSE(m2.interval != 42);
  ASSERT_FALSE(m2.is_for_rebuild != true);
  ASSERT_FALSE(m2.metric_id != 24);
  ASSERT_FALSE(m2.name != "foo");
  ASSERT_FALSE(m2.rrd_len != 180);
  ASSERT_FALSE(fabs(m2.value - 4242.0) > 0.00001);
  ASSERT_FALSE(m2.value_type != 1);
}

/**
 *  Check that the metric copy constructor works properly.
 */
TEST(StorageMetric, CopyCtor) {
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
  ASSERT_FALSE(m1.ctime != 741258);
  ASSERT_FALSE(m1.interval != 36);
  ASSERT_FALSE(m1.is_for_rebuild != false);
  ASSERT_FALSE(m1.metric_id != 12);
  ASSERT_FALSE(m1.name != "bazqux");
  ASSERT_FALSE(m1.rrd_len != 900);
  ASSERT_FALSE(fabs(m1.value - 1234.0) > 0.00001);
  ASSERT_FALSE(m1.value_type != 2);
  ASSERT_FALSE(m2.ctime != 123456789);
  ASSERT_FALSE(m2.interval != 42);
  ASSERT_FALSE(m2.is_for_rebuild != true);
  ASSERT_FALSE(m2.metric_id != 24);
  ASSERT_FALSE(m2.name != "foobar");
  ASSERT_FALSE(m2.rrd_len != 180);
  ASSERT_FALSE(fabs(m2.value - 4242.0) > 0.00001);
  ASSERT_FALSE(m2.value_type != 3);
}

/**
 *  Check that the metric object properly default constructs.
 */
TEST(StorageMetric, DefaultCtor) {
  // Build object.
  storage::metric m;

  auto val(io::events::data_type<io::events::storage, storage::de_metric>::value);

  // Check properties values.
  ASSERT_FALSE(m.ctime != 0);
  ASSERT_FALSE(m.interval != 0);
  ASSERT_FALSE(m.is_for_rebuild != false);
  ASSERT_FALSE(m.metric_id != 0);
  ASSERT_FALSE(!m.name.empty());
  ASSERT_FALSE(m.rrd_len != 0);
  ASSERT_FALSE(!std::isnan(m.value));
  ASSERT_FALSE(m.value_type != storage::perfdata::gauge);
  ASSERT_FALSE(m.type() != val);
}
