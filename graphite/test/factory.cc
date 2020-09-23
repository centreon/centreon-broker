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

#include "com/centreon/broker/graphite/factory.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

TEST(graphiteFactory, HasEndpoint) {
  graphite::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);

  cfg.type = "tcp";
  ASSERT_FALSE(fact.has_endpoint(cfg, nullptr));
  cfg.type = "graphite";
  ASSERT_TRUE(fact.has_endpoint(cfg, nullptr));
  ASSERT_EQ(cfg.cache_enabled, true);
  ASSERT_EQ(cfg.params["cache"], "yes");
}

TEST(graphiteFactory, MissingParams) {
  graphite::factory fact;
  config::endpoint cfg(config::endpoint::io_type::output);
  std::shared_ptr<persistent_cache> cache;
  bool is_acceptor;

  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["db_host"] = "host";
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
  cfg.params["db_port"] = "toto";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), std::exception);
  cfg.params["db_port"] = "1234";
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
  cfg.params["queries_per_transaction"] = "toto";
  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), std::exception);
  cfg.params["queries_per_transaction"] = "1234";
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
  cfg.params["metric_naming"] = "centreon.metrics.$METRICID$";
  ASSERT_NO_THROW(delete fact.new_endpoint(cfg, is_acceptor, cache));
}
