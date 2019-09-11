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

#include "com/centreon/broker/storage/connector.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/config.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/storage/factory.hh"

using namespace com::centreon::broker;

TEST(StorageConnector, Connector) {
  storage::connector con;
  storage::connector con2;
  database_config cfg;

  con.connect_to(cfg, 5, 6, 7);
  con2 = con;

  storage::connector con3{con2};

  ASSERT_TRUE(con == con3);
  ASSERT_TRUE(con2 == con3);
  ASSERT_TRUE(con == con);
}

TEST(StorageFactory, Factory) {
  std::shared_ptr<persistent_cache> cache;
  config::endpoint cfg;
  bool is_acceptor;

  storage::factory factory;

  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["length"] = "42";
  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), exceptions::config);
  cfg.params["db_type"] = "mysql";
  cfg.params["db_name"] = "centreon";
  ASSERT_FALSE(factory.has_endpoint(cfg));
  cfg.type = "storage";
  storage::connector *endp = static_cast<storage::connector *>(factory.new_endpoint(cfg, is_acceptor, cache));

  database_config db_cfg(cfg);
  storage::connector con;
  con.connect_to(cfg, 42, 60, 300, true, false);

  ASSERT_TRUE(*endp == con);

  ASSERT_TRUE(factory.has_endpoint(cfg));
  ASSERT_TRUE(cfg.read_timeout == 1);
  ASSERT_TRUE(cfg.params["read_timeout"] == "1");

  delete endp;
}

TEST(StorageFactory, FactoryWithFullConf) {
  std::shared_ptr<persistent_cache> cache;
  config::endpoint cfg;
  bool is_acceptor;

  storage::factory factory;

  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), exceptions::msg);
  cfg.params["length"] = "42";
  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), exceptions::config);
  cfg.params["db_type"] = "mysql";
  cfg.params["db_name"] = "centreon";
  cfg.params["interval"] = "43";
  cfg.params["rebuild_check_interval"] = "44";
  cfg.params["store_in_data_bin"] = "0";
  cfg.params["insert_in_index_data"] = "1";
  ASSERT_FALSE(factory.has_endpoint(cfg));
  cfg.type = "storage";
  storage::connector *endp = static_cast<storage::connector *>(factory.new_endpoint(cfg, is_acceptor, cache));

  database_config db_cfg(cfg);
  storage::connector con;
  con.connect_to(cfg, 42, 43, 44, false, true);

  ASSERT_TRUE(*endp == con);

  ASSERT_TRUE(factory.has_endpoint(cfg));
  ASSERT_TRUE(cfg.read_timeout == 1);
  ASSERT_TRUE(cfg.params["read_timeout"] == "1");

  delete endp;
}