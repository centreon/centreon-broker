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

#include "com/centreon/broker/storage/metric.hh"
#include <gtest/gtest.h>
#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric assignment operator works properly.
 */
TEST(StorageMetric, SpecificConstructor) {
  // First object.
  storage::metric m1(1, 14, "foo", 123456789, 42, true, 24, 180, 4242.0, 1);

  // Check objects properties values.
  ASSERT_FALSE(m1.ctime != 123456789);
  ASSERT_FALSE(m1.interval != 42);
  ASSERT_FALSE(m1.is_for_rebuild != true);
  ASSERT_FALSE(m1.metric_id != 24);
  ASSERT_FALSE(m1.name != "foo");
  ASSERT_FALSE(m1.rrd_len != 180);
  ASSERT_FALSE(fabs(m1.value - 4242.0) > 0.00001);
  ASSERT_FALSE(m1.value_type != 1);
}

/**
 *  Check that the metric object properly default constructs.
 */
TEST(StorageMetric, DefaultCtor) {
  // Build object.
  storage::metric m;

  auto val(
      io::events::data_type<io::events::storage, storage::de_metric>::value);

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
