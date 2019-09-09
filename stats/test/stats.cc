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

#include <json11.hpp>
#include <gtest/gtest.h>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/stats/builder.hh"

using namespace com::centreon::broker;

class StatsTest : public ::testing::Test {
 public:
  void SetUp() override {
    multiplexing::engine::load();
    config::applier::modules::load();
    config::applier::endpoint::load();
  }

  void TearDown() override {
    config::applier::endpoint::unload();
    config::applier::modules::unload();
    multiplexing::engine::unload();
  }
};

TEST_F(StatsTest, Builder) {
  stats::builder build;

  build.build();

  std::string err;
  json11::Json const& result{json11::Json::parse(build.data(), err)};

  ASSERT_TRUE(err.empty());
  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"],misc::string::get(CENTREON_BROKER_VERSION));
  ASSERT_EQ(result["pid"], misc::string::get(getpid()));
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());
  std::cout << result.dump() << std::endl;
}