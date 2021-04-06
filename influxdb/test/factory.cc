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

#include "com/centreon/broker/influxdb/factory.hh"
#include <gtest/gtest.h>
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(InfluxDBFactory, HasEndpoint) {
  influxdb::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);

  cfg.type = "tcp";
  ASSERT_FALSE(fact.has_endpoint(cfg, nullptr));
  cfg.type = "influxdb";
  ASSERT_TRUE(fact.has_endpoint(cfg, nullptr));
  ASSERT_EQ(cfg.cache_enabled, true);
  ASSERT_EQ(cfg.params["cache"], "yes");
}

TEST(InfluxDBFactory, MissingParams) {
  influxdb::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);
  std::shared_ptr<persistent_cache> cache;
  bool is_acceptor;

  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_user"] = "admin";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_password"] = "pass";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_host"] = "host";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_name"] = "centreon";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_port"] = "centreon";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["db_port"] = "4242";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["queries_per_transaction"] = "centreon";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["queries_per_transaction"] = "100";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["status_timeseries"] = "host_status";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
}

TEST(InfluxDBFactory, StatusException) {
  influxdb::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);
  std::shared_ptr<persistent_cache> cache;
  bool is_acceptor;

  cfg.type = "influxdb";
  cfg.params["db_user"] = "admin";
  cfg.params["db_password"] = "pass";
  cfg.params["db_host"] = "host";
  cfg.params["db_name"] = "centreon";
  cfg.params["db_port"] = "4242";
  cfg.params["queries_per_transaction"] = "100";
  cfg.params["metrics_timeseries"] = "host_metric";
  cfg.params["status_timeseries"] = "host_status";

  json11::Json::object conf;
  conf["status_column"] = nullptr;
  cfg.cfg = json11::Json{conf};
  std::unique_ptr<io::endpoint> ep;
  ASSERT_NO_THROW(ep.reset(fact.new_endpoint(cfg, is_acceptor, cache)));

  json11::Json js1{json11::Json::object{
      {"name", json11::Json{nullptr}},
      {"value", json11::Json{nullptr}},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["status_column"] = js1;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js2{json11::Json::object{
      {"name", "host"},
      {"value", json11::Json{nullptr}},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["status_column"] = js2;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js3{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["status_column"] = js3;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js4{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", json11::Json{nullptr}},
  }};
  conf["status_column"] = js4;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js5{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", "bad"},
  }};
  conf["status_column"] = js5;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js6{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", "number"},
  }};
  conf["status_column"] = js6;
  cfg.cfg = json11::Json{conf};
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));

  json11::Json js7{json11::Json::object{
      {"name", ""},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", "number"},
  }};
  conf["status_column"] = js7;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json::array array;
  array.push_back(js6);
  array.push_back(js6);
  array.push_back(js6);
  array.push_back(js6);
  conf["status_column"] = array;
  cfg.cfg = json11::Json{conf};
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
}

TEST(InfluxDBFactory, MetricException) {
  influxdb::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);
  std::shared_ptr<persistent_cache> cache;
  bool is_acceptor;

  cfg.type = "influxdb";
  cfg.params["db_user"] = "admin";
  cfg.params["db_password"] = "pass";
  cfg.params["db_host"] = "host";
  cfg.params["db_name"] = "centreon";
  cfg.params["db_port"] = "4242";
  cfg.params["queries_per_transaction"] = "100";
  cfg.params["metrics_timeseries"] = "host_metric";
  cfg.params["status_timeseries"] = "host_status";

  json11::Json::object conf;
  conf["metric_column"] = nullptr;
  cfg.cfg = json11::Json{conf};
  std::unique_ptr<io::endpoint> ep;
  ASSERT_NO_THROW(ep.reset(fact.new_endpoint(cfg, is_acceptor, cache)));

  json11::Json js1{json11::Json::object{
      {"name", json11::Json{nullptr}},
      {"value", json11::Json{nullptr}},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["metrics_column"] = js1;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js2{json11::Json::object{
      {"name", "host"},
      {"value", json11::Json{nullptr}},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["metrics_column"] = js2;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js3{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", json11::Json{nullptr}},
      {"type", json11::Json{nullptr}},
  }};
  conf["metrics_column"] = js3;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js4{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", json11::Json{nullptr}},
  }};
  conf["metrics_column"] = js4;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js5{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", "bad"},
  }};
  conf["metrics_column"] = js5;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json js6{json11::Json::object{
      {"name", "host"},
      {"value", "val"},
      {"is_tag", "true"},
      {"type", "number"},
  }};
  conf["metrics_column"] = js6;
  cfg.cfg = json11::Json{conf};
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));

  json11::Json js7{json11::Json::object{
      {"name", ""},
      {"value", "val"},
      {"is_tag", "false"},
      {"type", "number"},
  }};
  conf["metrics_column"] = js7;
  cfg.cfg = json11::Json{conf};
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), msg_fmt);

  json11::Json::array array;
  array.push_back(js6);
  array.push_back(js6);
  array.push_back(js6);
  array.push_back(js6);
  conf["metrics_column"] = array;
  cfg.cfg = json11::Json{conf};
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
}
