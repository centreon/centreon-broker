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

#include "com/centreon/broker/graphite/stream.hh"
#include <gtest/gtest.h>
#include <com/centreon/broker/graphite/connector.hh>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "../../core/test/test_server.hh"

using namespace com::centreon::broker;

class graphiteStream : public testing::Test {
 public:
  void SetUp() override {
    std::thread t{[&] {
      _server.init();
      _server.run();
    }};

    _thread = std::move(t);

    while (!_server.get_init_done());
  }
  void TearDown() override {
    if (_server.get_init_done())
      _server.stop();
    _thread.join();
  }

  test_server _server;
  std::thread _thread;
};

TEST_F(graphiteStream, BadPort) {
  std::shared_ptr<persistent_cache> cache;

  ASSERT_THROW(graphite::stream st("metric_name", "status_name", "a", "user", "pass", "localhost", 4243, 3, cache), exceptions::msg);
}

TEST_F(graphiteStream, Read) {
  std::shared_ptr<persistent_cache> cache;
  std::shared_ptr<io::data> data;

  graphite::stream st("metric_name", "status_name", "a", "user", "pass", "localhost", 4242, 3, cache);
  ASSERT_THROW(st.read(data, -1), exceptions::msg);
}

TEST_F(graphiteStream, Write) {
  std::shared_ptr<persistent_cache> cache;
  storage::metric *m1, *m2, *m3;
  std::shared_ptr<io::data> data;
  graphite::stream st("metric_name", "status_name", "a", "user", "pass", "localhost", 4242, 3, cache);

  m1 = new storage::metric;
  m2 = new storage::metric;
  m3 = new storage::metric;

  m1->ctime = 2000llu;
  m1->interval = 60;
  m1->is_for_rebuild = true;
  m1->metric_id = 42u;
  m1->name = "host1";
  m1->rrd_len = 42;
  m1->value = 42.0;
  m1->value_type = 4;
  m1->host_id = 1u;
  m1->service_id = 1u;

  m2->ctime = 2000llu;
  m2->interval = 60;
  m2->is_for_rebuild = true;
  m2->metric_id = 42u;
  m2->name = "host1";
  m2->rrd_len = 42;
  m2->value = 42.0;
  m2->value_type = 4;
  m2->host_id = 1u;
  m2->service_id = 1u;

  m3->ctime = 2000llu;
  m3->interval = 60;
  m3->is_for_rebuild = true;
  m3->metric_id = 42u;
  m3->name = "host1";
  m3->rrd_len = 42;
  m3->value = 42.0;
  m3->value_type = 4;
  m3->host_id = 1u;
  m3->service_id = 1u;
  std::shared_ptr<io::data> d1;
  std::shared_ptr<io::data> d2;
  std::shared_ptr<io::data> d3;

  d1.reset(m1);
  d2.reset(m2);
  d3.reset(m3);
  ASSERT_FALSE(st.write(d1));
  ASSERT_FALSE(st.write(d2));
  ASSERT_TRUE(st.write(d3));
}

TEST_F(graphiteStream, Flush) {
  std::shared_ptr<persistent_cache> cache;
  storage::metric *m1, *m2, *m3;
  std::shared_ptr<io::data> data;
  graphite::stream st("metric_name", "status_name", "a", "user", "pass", "localhost", 4242, 9, cache);

  m1 = new storage::metric;
  m2 = new storage::metric;
  m3 = new storage::metric;

  m1->ctime = 2000llu;
  m1->interval = 60;
  m1->is_for_rebuild = true;
  m1->metric_id = 42u;
  m1->name = "host1";
  m1->rrd_len = 42;
  m1->value = 42.0;
  m1->value_type = 4;
  m1->host_id = 1u;
  m1->service_id = 1u;

  m2->ctime = 2000llu;
  m2->interval = 60;
  m2->is_for_rebuild = true;
  m2->metric_id = 42u;
  m2->name = "host1";
  m2->rrd_len = 42;
  m2->value = 42.0;
  m2->value_type = 4;
  m2->host_id = 1u;
  m2->service_id = 1u;

  m3->ctime = 2000llu;
  m3->interval = 60;
  m3->is_for_rebuild = true;
  m3->metric_id = 42u;
  m3->name = "host1";
  m3->rrd_len = 42;
  m3->value = 42.0;
  m3->value_type = 4;
  m3->host_id = 1u;
  m3->service_id = 1u;

  std::shared_ptr<io::data> d1;
  std::shared_ptr<io::data> d2;
  std::shared_ptr<io::data> d3;

  d1.reset(m1);
  d2.reset(m2);
  d3.reset(m3);
  ASSERT_FALSE(st.write(d1));
  ASSERT_FALSE(st.write(d2));
  ASSERT_FALSE(st.write(d3));

  ASSERT_TRUE(st.flush());
}

TEST_F(graphiteStream, NullData) {
  std::shared_ptr<persistent_cache> cache;
  std::shared_ptr<io::data> data;
  graphite::stream st("metric_name",
                      "status_name",
                      "a",
                      "user",
                      "pass",
                      "localhost",
                      4242,
                      9,
                      cache);

  std::shared_ptr<io::data> d1{nullptr};
  ASSERT_FALSE(st.write(d1));
}

TEST_F(graphiteStream, FlushStatusOK) {
  std::shared_ptr<persistent_cache> cache;
  storage::status *s1, *s2, *s3;

  std::shared_ptr<io::data> data;
  graphite::stream st("metric_name", "status_name", "a", "user", "pass", "localhost", 4242, 9, cache);

  s1 = new storage::status;
  s2 = new storage::status;
  s3 = new storage::status;

  s1->source_id = 3;
  s1->destination_id = 4;
  s1->broker_id = 1;
  s1->ctime = 2000llu;
  s1->interval = 60;
  s1->index_id = 3;
  s1->is_for_rebuild = true;
  s1->rrd_len = 9;
  s1->state = 2;

  s2->source_id = 3;
  s2->destination_id = 4;
  s2->broker_id = 1;
  s2->ctime = 2000llu;
  s2->interval = 60;
  s2->index_id = 3;
  s2->is_for_rebuild = true;
  s2->rrd_len = 9;
  s2->state = 2;

  s3->source_id = 3;
  s3->destination_id = 4;
  s3->broker_id = 1;
  s3->ctime = 2000llu;
  s3->interval = 60;
  s3->index_id = 3;
  s3->is_for_rebuild = true;
  s3->rrd_len = 9;
  s3->state = 2;

  std::shared_ptr<io::data> d1;
  std::shared_ptr<io::data> d2;
  std::shared_ptr<io::data> d3;

  d1.reset(s1);
  d2.reset(s2);
  d3.reset(s3);
  ASSERT_FALSE(st.write(d1));
  ASSERT_FALSE(st.write(d2));
  ASSERT_FALSE(st.write(d3));

  ASSERT_TRUE(st.flush());
}

TEST_F(graphiteStream, StatsAndConnector) {
  std::shared_ptr<persistent_cache> cache;
  storage::metric m1, m2, m3;
  std::shared_ptr<io::data> data;
  graphite::connector con;
  con.connect_to("metric_name", "status_name", "a", "user", "pass", "localhost", 4242, 3, cache);

  json11::Json::object obj;
  con.open()->statistics(obj);
  ASSERT_TRUE(obj["state"].string_value().empty());
}
