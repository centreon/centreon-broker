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

#include "com/centreon/broker/rrd/factory.hh"
#include <gtest/gtest.h>
#include <com/centreon/broker/rrd/connector.hh>
#include <com/centreon/broker/rrd/output.hh>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

TEST(RRDFactory, HasEndpoint) {
  rrd::factory fact;
  config::endpoint cfg;

  cfg.type = "ip";
  ASSERT_FALSE(fact.has_endpoint(cfg));
  cfg.type = "rrd";
  ASSERT_TRUE(fact.has_endpoint(cfg));
}

TEST(RRDFactory, Exception) {
  rrd::factory fact;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
}

TEST(RRDFactory, Simple) {
  rrd::factory fact2;
  rrd::factory* fact = &fact2;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["path"] = "/tmp/";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["port"] = "/tmp/test";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["port"] = "4242";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["cache_size"] = "dsasd";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["cache_size"] = "50";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["metrics_path"] = "toto";
  ASSERT_THROW(fact->new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["status_path"] = "toto";
  ASSERT_NO_THROW(delete fact->new_endpoint(cfg, is_acceptor, cache));
  cfg.params["write_metrics"] = "false";
  cfg.params["write_status"] = "false";
  cfg.params["ignore_update_errors"] = "false";
  cfg.params["path"] = "";
  ASSERT_NO_THROW(delete fact->new_endpoint(cfg, is_acceptor, cache));
}

TEST(RRDFactory, Output) {
  rrd::factory fact;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  cfg.params["path"] = "/tmp/";
  cfg.params["cache_size"] = "dsasd";
  cfg.params["cache_size"] = "50";
  cfg.params["metrics_path"] = "toto";
  cfg.params["status_path"] = "toto";
  cfg.params["write_metrics"] = "false";
  cfg.params["write_status"] = "false";
  cfg.params["ignore_update_errors"] = "false";
  cfg.params["path"] = "";
  rrd::connector* con
    {static_cast<rrd::connector*>(fact.new_endpoint(cfg, is_acceptor, cache))};

  std::shared_ptr<rrd::output>
    out{std::static_pointer_cast<rrd::output>(con->open())};

  delete con;
}
