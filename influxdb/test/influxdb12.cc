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

#include "com/centreon/broker/influxdb/influxdb12.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "../../core/test/test_server.hh"

using namespace com::centreon::broker;

class InfluxDB12 : public testing::Test {
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

TEST_F(InfluxDB12, BadConnection) {
  std::shared_ptr<persistent_cache> cache;
  influxdb::macro_cache mcache{cache};
  storage::metric m1, m2, m3;
  std::vector<influxdb::column> mcolumns;
  std::vector<influxdb::column> scolumns;

  ASSERT_THROW(influxdb::influxdb12 idb
                 ("centreon", "pass", "localhost", 4243, "centreon", "host_status", scolumns, "host_metrics", mcolumns, mcache),
               exceptions::msg);
}

TEST_F(InfluxDB12, Empty) {
  std::shared_ptr<persistent_cache> cache;
  influxdb::macro_cache mcache{cache};
  storage::metric m1, m2, m3;
  std::vector<influxdb::column> mcolumns;
  std::vector<influxdb::column> scolumns;

  influxdb::influxdb12 idb
    ("centreon", "pass", "localhost", 4242, "centreon", "host_status", scolumns, "host_metrics", mcolumns, mcache);
  idb.clear();
  ASSERT_NO_THROW(idb.commit());
}

TEST_F(InfluxDB12, Simple) {
  std::shared_ptr<persistent_cache> cache;
  influxdb::macro_cache mcache{cache};
  storage::metric m1, m2, m3;

  std::vector<influxdb::column> mcolumns;
  mcolumns.push_back(influxdb::column{"mhost1", "42.0", true, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"mhost2", "42.0", false, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"most2", "42.0", false, influxdb::column::string});
  mcolumns.push_back(influxdb::column{"most3", "43.0", true, influxdb::column::number});

  std::vector<influxdb::column> scolumns;
  mcolumns.push_back(influxdb::column{"shost1", "42.0", true, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"shost2", "42.0", false, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"shost2", "42.0", false, influxdb::column::string});
  mcolumns.push_back(influxdb::column{"shost3", "43.0", true, influxdb::column::number});

  influxdb::influxdb12 idb
    ("centreon", "pass", "localhost", 4242, "centreon", "host_status", scolumns, "host_metrics", mcolumns, mcache);
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

  idb.write(m1);
  idb.write(m2);
  idb.write(m3);

  ASSERT_NO_THROW(idb.commit());
}

TEST_F(InfluxDB12, BadServerResponse1) {
  std::shared_ptr<persistent_cache> cache;
  influxdb::macro_cache mcache{cache};
  storage::metric m1, m2, m3;
  std::vector<influxdb::column> mcolumns;
  std::vector<influxdb::column> scolumns;

  influxdb::influxdb12 idb
    ("centreon", "fail1", "localhost", 4242, "centreon", "host_status", scolumns, "host_metrics", mcolumns, mcache);

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

  idb.write(m1);
  idb.write(m2);
  idb.write(m3);

  ASSERT_THROW(idb.commit(), exceptions::msg);
}

TEST_F(InfluxDB12, BadServerResponse2) {
  std::shared_ptr<persistent_cache> cache;
  influxdb::macro_cache mcache{cache};
  storage::metric m1, m2, m3;

  std::vector<influxdb::column> mcolumns;
  mcolumns.push_back(influxdb::column{"mhost1", "42.0", true, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"mhost2", "42.0", false, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"most2", "42.0", false, influxdb::column::string});
  mcolumns.push_back(influxdb::column{"most3", "43.0", true, influxdb::column::number});

  std::vector<influxdb::column> scolumns;
  mcolumns.push_back(influxdb::column{"shost1", "42.0", true, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"shost2", "42.0", false, influxdb::column::number});
  mcolumns.push_back(influxdb::column{"shost2", "42.0", false, influxdb::column::string});
  mcolumns.push_back(influxdb::column{"shost3", "43.0", true, influxdb::column::number});

  influxdb::influxdb12 idb
    ("centreon", "fail2", "localhost", 4242, "centreon", "host_status", scolumns, "host_metrics", mcolumns, mcache);

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

  idb.write(m1);
  idb.write(m2);
  idb.write(m3);

  ASSERT_THROW(idb.commit(), exceptions::msg);
}

