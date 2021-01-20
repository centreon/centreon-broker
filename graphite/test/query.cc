/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/graphite/query.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(graphiteQuery, ComplexMetric) {
  std::shared_ptr<persistent_cache> pcache{nullptr};
  graphite::macro_cache cache(pcache);
  storage::metric m{1u, 1u, "host1", 2000llu, 60, true, 40u, 42, 42.0, 4};
  std::shared_ptr<neb::host> host{std::make_shared<neb::host>()};
  std::shared_ptr<neb::service> svc{std::make_shared<neb::service>()};
  std::shared_ptr<neb::instance> instance{std::make_shared<neb::instance>()};
  std::shared_ptr<storage::metric_mapping> metric_map{
      std::make_shared<storage::metric_mapping>()};
  std::shared_ptr<storage::index_mapping> index_map{
      std::make_shared<storage::index_mapping>()};

  m.source_id = 3;

  svc->service_description = "svc.1";
  svc->service_id = 1;
  svc->host_id = 1;

  host->host_name = "host1";
  host->host_id = 1;

  instance->poller_id = 3;
  instance->name = "poller test";

  metric_map->metric_id = 40;
  metric_map->index_id = 41;

  index_map->index_id = 41;

  cache.write(host);
  cache.write(svc);
  cache.write(instance);
  cache.write(metric_map);
  cache.write(index_map);

  graphite::query q{
      "test . $HOST$ $HOSTID$ $SERVICE$ $SERVICEID$ $INSTANCE$ $INSTANCEID$ "
      "$INDEXID$ $TEST$ TEST $$",
      "a", graphite::query::metric, cache};

  ASSERT_EQ(q.generate_metric(m),
            "test_._host1_1_svca1_1_poller_test_3_41__TEST_$ 42 2000\n");
}

TEST(graphiteQuery, ComplexStatus) {
  std::shared_ptr<persistent_cache> pcache{nullptr};
  graphite::macro_cache cache(pcache);
  storage::status s{2000llu, 3, 60, true, 9, 2};

  std::shared_ptr<neb::host> host{std::make_shared<neb::host>()};
  std::shared_ptr<neb::service> svc{std::make_shared<neb::service>()};
  std::shared_ptr<neb::instance> instance{std::make_shared<neb::instance>()};
  std::shared_ptr<storage::index_mapping> index_map{
      std::make_shared<storage::index_mapping>()};

  graphite::query q{
      "test . $HOST$ $HOSTID$ $SERVICE$ $SERVICEID$ $INSTANCE$ $INSTANCEID$ "
      "$INDEXID$ $TEST$ TEST $$",
      "a", graphite::query::status, cache};

  svc->service_description = "svc1";
  svc->service_id = 1;
  svc->host_id = 1;

  host->host_name = "host1";
  host->host_id = 1;

  instance->poller_id = 3;
  instance->name = "poller test";

  index_map->index_id = 3;
  index_map->host_id = 1;
  index_map->service_id = 1;

  s.source_id = 3;
  s.destination_id = 4;
  s.broker_id = 1;

  cache.write(host);
  cache.write(svc);
  cache.write(instance);
  cache.write(index_map);

  ASSERT_EQ(q.generate_status(s),
            "test_._host1_1_svc1_1_poller_test_3_3__TEST_$ 2 2000\n");
}

TEST(graphiteQuery, Except) {
  std::shared_ptr<persistent_cache> pcache{nullptr};
  graphite::macro_cache cache(pcache);
  storage::status s;
  storage::metric m;

  graphite::query q{"test .", "a", graphite::query::metric, cache};
  graphite::query q2{"test .", "a", graphite::query::status, cache};

  try {
    graphite::query q3{"test . $METRICID$", "a", graphite::query::status,
                       cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  try {
    graphite::query q3{"test . $METRIC$", "a", graphite::query::status, cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  try {
    graphite::query q3{"test . $METRIC", "a", graphite::query::status, cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  m.metric_id = 3;
  m.name = "A";

  graphite::query q4{"test . $METRICID$ $METRIC$", "a", graphite::query::metric,
                     cache};

  ASSERT_THROW(q.generate_status(s), msg_fmt);
  ASSERT_THROW(q2.generate_metric(m), msg_fmt);
  ASSERT_EQ(q4.generate_metric(m), "test_._3_A nan 0\n");

  graphite::query q5{"test . $INSTANCE$", "a", graphite::query::metric, cache};
  ASSERT_EQ(q5.generate_metric(m), "");

  graphite::query q6{"test . $INSTANCE$", "a", graphite::query::status, cache};
  ASSERT_EQ(q6.generate_status(s), "");
}
