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

#include "com/centreon/broker/tcp/factory.hh"
#include <gtest/gtest.h>
#include <com/centreon/broker/exceptions/msg.hh>
#include <com/centreon/broker/tcp/acceptor.hh>

using namespace com::centreon::broker;

TEST(TcpFactory, HasEndpoint) {
  tcp::factory fact;
  config::endpoint cfg;

  cfg.type = "ip";
  ASSERT_TRUE(fact.has_endpoint(cfg));
  cfg.type = "tcp";
  ASSERT_TRUE(fact.has_endpoint(cfg));
  cfg.type = "ipv4";
  ASSERT_TRUE(fact.has_endpoint(cfg));
  cfg.type = "ipv6";
  ASSERT_TRUE(fact.has_endpoint(cfg));
  cfg.type = "udp";
  ASSERT_FALSE(fact.has_endpoint(cfg));
}

TEST(TcpFactory, Exception) {
  tcp::factory fact;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  ASSERT_THROW(fact.new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
}

TEST(TcpFactory, Acceptor) {
  tcp::factory fact;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  cfg.params["port"] = "4343";
  cfg.params["socket_write_timeout"] = "10";
  cfg.params["socket_read_timeout"] = "10";
  io::endpoint* endp = fact.new_endpoint(cfg, is_acceptor, cache);

  ASSERT_TRUE(is_acceptor);
  ASSERT_TRUE(endp->is_acceptor());

  delete endp;
}

TEST(TcpFactory, Connector) {
  tcp::factory fact;
  config::endpoint cfg;
  bool is_acceptor;
  std::shared_ptr<persistent_cache> cache;

  cfg.type = "tcp";
  cfg.params["port"] = "4444";
  cfg.params["host"] = "127.0.0.1";
  std::unique_ptr<io::factory> f{new tcp::factory};
  ASSERT_TRUE(f->has_endpoint(cfg));
  std::unique_ptr<io::endpoint> endp{fact.new_endpoint(cfg, is_acceptor, cache)};

  ASSERT_FALSE(is_acceptor);
  ASSERT_TRUE(endp->is_connector());
}
