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
#include "com/centreon/broker/storage/factory.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(StorageFactory, Factory) {
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                        "centreon_storage", 5, true, 5);
  std::shared_ptr<persistent_cache> cache;
  config::endpoint cfg(config::endpoint::io_type::output);
  bool is_acceptor;

  storage::factory factory;

  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["length"] = "42";
  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache),
               exceptions::config);
  cfg.params["db_type"] = "mysql";
  cfg.params["db_name"] = "centreon";
  ASSERT_FALSE(factory.has_endpoint(cfg, nullptr));
  cfg.type = "storage";
  storage::connector* endp = static_cast<storage::connector*>(
      factory.new_endpoint(cfg, is_acceptor, cache));

  storage::connector con;
  con.connect_to(dbcfg, 42, 60, 300, true);

  ASSERT_TRUE(factory.has_endpoint(cfg, nullptr));
  ASSERT_EQ(cfg.read_timeout, -1);
  ASSERT_EQ(cfg.params["read_timeout"], "");

  delete endp;
}

TEST(StorageFactory, FactoryWithFullConf) {
  database_config dbcfg("MySQL", "127.0.0.1", 3306, "centreon", "centreon",
                        "centreon_storage", 5, true, 5);
  std::shared_ptr<persistent_cache> cache;
  config::endpoint cfg(config::endpoint::io_type::output);
  bool is_acceptor;

  storage::factory factory;

  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache), msg_fmt);
  cfg.params["length"] = "42";
  ASSERT_THROW(factory.new_endpoint(cfg, is_acceptor, cache),
               exceptions::config);
  cfg.params["db_type"] = "mysql";
  cfg.params["db_name"] = "centreon";
  cfg.params["interval"] = "43";
  cfg.params["rebuild_check_interval"] = "44";
  cfg.params["store_in_data_bin"] = "0";
  ASSERT_FALSE(factory.has_endpoint(cfg, nullptr));
  cfg.type = "storage";
  storage::connector* endp = static_cast<storage::connector*>(
      factory.new_endpoint(cfg, is_acceptor, cache));

  storage::connector con;
  con.connect_to(dbcfg, 42, 43, 44, false);

  ASSERT_TRUE(factory.has_endpoint(cfg, nullptr));
  ASSERT_EQ(cfg.read_timeout, -1);
  ASSERT_EQ(cfg.params["read_timeout"], "");

  delete endp;
}
