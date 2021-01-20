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

#include "com/centreon/broker/influxdb/line_protocol_query.hh"
#include <gtest/gtest.h>
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(InfluxDBLineProtoQuery, EscapeKey) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_key("The test = valid, I hope"),
            "The\\ test\\ \\=\\ valid\\,\\ I\\ hope");
}

TEST(InfluxDBLineProtoQuery, EscapeMeasurement) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_measurement("The test = valid, I hope"),
            "The\\ test\\ =\\ valid\\,\\ I\\ hope");
}

TEST(InfluxDBLineProtoQuery, EscapeValue) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_value("The \"test\" = valid, I hope"),
            "\"The \\\"test\\\" = valid, I hope\"");
}

TEST(InfluxDBLineProtoQuery, GenerateMetricExcept) {
  influxdb::line_protocol_query lpq1;
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  influxdb::line_protocol_query lpq2(
      "test", columns, influxdb::line_protocol_query::status, cache);
  influxdb::line_protocol_query lpq3(
      "test", columns, influxdb::line_protocol_query::metric, cache);
  storage::metric m1;

  ASSERT_THROW(lpq1.generate_metric(m1), msg_fmt);
  ASSERT_THROW(lpq2.generate_metric(m1), msg_fmt);
  ASSERT_NO_THROW(lpq3.generate_metric(m1));
}

TEST(InfluxDBLineProtoQuery, GenerateMetric) {
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  storage::metric m1{1u, 1u, "host1", 2000llu, 60, true, 42u, 42, 42.0, 4};
  storage::metric m2{1u, 1u, "host2", 4000llu, 120, false, 43, 42, 42.0, 4};
  storage::metric m3{2u, 3u, "hotst3", 2000llu, 60, true, 42u, 43, 43.0, 5};

  columns.push_back(
      influxdb::column{"host1", "42.0", true, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::string});
  columns.push_back(
      influxdb::column{"host3", "43.0", true, influxdb::column::number});

  influxdb::line_protocol_query lpq(
      "test", columns, influxdb::line_protocol_query::metric, cache);

  ASSERT_EQ(lpq.generate_metric(m1),
            "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
  ASSERT_EQ(lpq.generate_metric(m2),
            "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 4000\n");
  ASSERT_EQ(lpq.generate_metric(m3),
            "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
}

TEST(InfluxDBLineProtoQuery, ComplexMetric) {
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  storage::metric m{1u, 1u, "host1", 2000llu, 60, true, 40u, 42, 42.0, 4};
  std::shared_ptr<neb::host> host{std::make_shared<neb::host>()};
  std::shared_ptr<neb::service> svc{std::make_shared<neb::service>()};
  std::shared_ptr<neb::instance> instance{std::make_shared<neb::instance>()};
  std::shared_ptr<storage::metric_mapping> metric_map{
      std::make_shared<storage::metric_mapping>()};
  std::shared_ptr<storage::index_mapping> index_map{
      std::make_shared<storage::index_mapping>()};

  columns.push_back(
      influxdb::column{"host1", "42.0", true, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::string});
  columns.push_back(
      influxdb::column{"host3", "43.0", true, influxdb::column::number});

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

  influxdb::line_protocol_query q{
      "test . $HOST$ $HOSTID$ $SERVICE$ $SERVICEID$ $INSTANCE$ $INSTANCEID$ "
      "$INDEXID$ $TEST$ TEST $$ $VALUE$",
      columns, influxdb::line_protocol_query::metric, cache};

  ASSERT_EQ(
      q.generate_metric(m),
      "test\\ .\\ host1\\ 1\\ svc.1\\ 1\\ poller\\ test\\ 3\\ 41\\ \\ TEST\\ $"
      "\\ 42,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
}

TEST(InfluxDBLineProtoQuery, ComplexStatus) {
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  storage::status s{2000llu, 3, 60, true, 9, 2};

  std::shared_ptr<neb::host> host{std::make_shared<neb::host>()};
  std::shared_ptr<neb::service> svc{std::make_shared<neb::service>()};
  std::shared_ptr<neb::instance> instance{std::make_shared<neb::instance>()};
  std::shared_ptr<storage::index_mapping> index_map{
      std::make_shared<storage::index_mapping>()};

  columns.push_back(
      influxdb::column{"host1", "42.0", true, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::number});
  columns.push_back(
      influxdb::column{"host2", "42.0", false, influxdb::column::string});
  columns.push_back(
      influxdb::column{"host3", "43.0", true, influxdb::column::number});

  influxdb::line_protocol_query q{
      "test . $HOST$ $HOSTID$ $SERVICE$ $SERVICEID$ $INSTANCE$ $INSTANCEID$ "
      "$INDEXID$ $TEST$ TEST $$ $VALUE$",
      columns, influxdb::line_protocol_query::status, cache};

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

  ASSERT_EQ(
      q.generate_status(s),
      "test\\ .\\ host1\\ 1\\ svc1\\ 1\\ poller\\ test\\ 3\\ 3\\ \\ "
      "TEST\\ $\\ 2,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
}

TEST(InfluxDBLineProtoQuery, Except) {
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  storage::status s;
  storage::metric m;

  influxdb::line_protocol_query q{"test .", columns,
                                  influxdb::line_protocol_query::metric, cache};
  influxdb::line_protocol_query q2{
      "test .", columns, influxdb::line_protocol_query::status, cache};

  try {
    influxdb::line_protocol_query q3{"test . $METRICID$", columns,
                                     influxdb::line_protocol_query::status,
                                     cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  try {
    influxdb::line_protocol_query q3{"test . $METRIC$", columns,
                                     influxdb::line_protocol_query::status,
                                     cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  try {
    influxdb::line_protocol_query q3{"test . $METRIC", columns,
                                     influxdb::line_protocol_query::status,
                                     cache};
    ASSERT_TRUE(false);
  } catch (msg_fmt const& ex) {
    ASSERT_TRUE(true);
  }

  m.metric_id = 3;
  m.name = "A";

  influxdb::line_protocol_query q4{"test . $METRICID$ $METRIC$", columns,
                                   influxdb::line_protocol_query::metric,
                                   cache};

  ASSERT_THROW(q.generate_status(s), msg_fmt);
  ASSERT_THROW(q2.generate_metric(m),msg_fmt);
  ASSERT_EQ(q4.generate_metric(m), "test\\ .\\ 3\\ A 0\n");

  influxdb::line_protocol_query q5{"test . $INSTANCE$", columns,
                                   influxdb::line_protocol_query::metric,
                                   cache};
  ASSERT_EQ(q5.generate_metric(m), "");

  influxdb::line_protocol_query q6{"test . $INSTANCE$", columns,
                                   influxdb::line_protocol_query::status,
                                   cache};
  ASSERT_EQ(q6.generate_status(s), "");
}
