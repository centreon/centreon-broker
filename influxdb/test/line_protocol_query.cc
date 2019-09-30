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
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

TEST(InfluxDBLineProtoQuery, EscapeKey) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_key("The test = valid, I hope"), "The\\ test\\ \\=\\ valid\\,\\ I\\ hope");
}

TEST(InfluxDBLineProtoQuery, EscapeMeasurement) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_measurement("The test = valid, I hope"), "The\\ test\\ =\\ valid\\,\\ I\\ hope");
}

TEST(InfluxDBLineProtoQuery, EscapeValue) {
  influxdb::line_protocol_query lpq;

  ASSERT_EQ(lpq.escape_value("The \"test\" = valid, I hope"), "\"The \\\"test\\\" = valid, I hope\"");
}

TEST(InfluxDBLineProtoQuery, GenerateMetricExcept) {
  influxdb::line_protocol_query lpq1;
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  influxdb::line_protocol_query lpq2("test", columns, influxdb::line_protocol_query::status, cache);
  influxdb::line_protocol_query lpq3("test", columns, influxdb::line_protocol_query::metric, cache);
  storage::metric m1;

  ASSERT_THROW(lpq1.generate_metric(m1), exceptions::msg);
  ASSERT_THROW(lpq2.generate_metric(m1), exceptions::msg);
  ASSERT_NO_THROW(lpq3.generate_metric(m1));
}

TEST(InfluxDBLineProtoQuery, GenerateMetric) {
  std::vector<influxdb::column> columns;
  std::shared_ptr<persistent_cache> pcache{nullptr};
  influxdb::macro_cache cache(pcache);
  storage::metric m1, m2, m3;


  columns.push_back(influxdb::column{"host1", "42.0", true, influxdb::column::number});
  columns.push_back(influxdb::column{"host2", "42.0", false, influxdb::column::number});
  columns.push_back(influxdb::column{"host2", "42.0", false, influxdb::column::string});
  columns.push_back(influxdb::column{"host3", "43.0", true, influxdb::column::number});

  influxdb::line_protocol_query lpq("test", columns, influxdb::line_protocol_query::metric, cache);


  m1.ctime = 2000llu;
  m1.interval = 60;
  m1.is_for_rebuild = true;
  m1.metric_id = 42u;
  m1.name = "host1";
  m1.rrd_len = 42;
  m1.value = 42.0;
  m1.value_type = 4;
  m1.host_id = 1u;
  m1.service_id = 1u;

  m2.ctime = 2000llu;
  m2.interval = 60;
  m2.is_for_rebuild = true;
  m2.metric_id = 42u;
  m2.name = "host1";
  m2.rrd_len = 42;
  m2.value = 42.0;
  m2.value_type = 4;
  m2.host_id = 1u;
  m2.service_id = 1u;

  m3.ctime = 2000llu;
  m3.interval = 60;
  m3.is_for_rebuild = true;
  m3.metric_id = 42u;
  m3.name = "host1";
  m3.rrd_len = 42;
  m3.value = 42.0;
  m3.value_type = 4;
  m3.host_id = 1u;
  m3.service_id = 1u;

  m2.ctime = 4000llu;
  m2.interval = 120;
  m2.is_for_rebuild = false;
  m2.metric_id = 43;
  m2.name = "host2";

  m3.name = "hotst3";
  m3.rrd_len = 43;
  m3.value = 43.0;
  m3.value_type = 5;
  m3.host_id = 2u;
  m3.service_id = 3u;

  ASSERT_EQ(lpq.generate_metric(m1), "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
  ASSERT_EQ(lpq.generate_metric(m2), "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 4000\n");
  ASSERT_EQ(lpq.generate_metric(m3), "test,host1=42.0,host3=43.0 host2=42.0,host2=\"42.0\" 2000\n");
}