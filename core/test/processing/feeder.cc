/*
 * Copyright 2020 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/processing/feeder.hh"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class TestFeeder : public ::testing::Test {
 public:
  void SetUp() override {
    multiplexing::engine::load();
    std::shared_ptr<io::stream> client;
    std::unordered_set<uint32_t> read_filters;
    std::unordered_set<uint32_t> write_filters;
    _feeder.reset(
        new feeder("test-feeder", client, read_filters, write_filters));
  }

  void TearDown() override {
    multiplexing::engine::unload();
  }

 protected:
  std::unique_ptr<feeder> _feeder;
};

TEST_F(TestFeeder, ImmediateExit) {
  ASSERT_NO_THROW(_feeder.reset());
}

TEST_F(TestFeeder, isFinished) {
  ASSERT_FALSE(_feeder->is_finished());
  json11::Json::object tree;
  _feeder->stats(tree);
  ASSERT_EQ(tree["state"].string_value(), "disconnected");
}
