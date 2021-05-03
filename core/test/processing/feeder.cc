/*
 * Copyright 2020-2021 Centreon (https://www.centreon.com/)
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
#include <thread>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/stats/center.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class TestStream : public io::stream {
 public:
  TestStream() : io::stream("TestStream") {}
  bool read(std::shared_ptr<io::data>&, time_t) override { return true; }

  int32_t write(std::shared_ptr<io::data> const&) override { return 1; }
  int32_t stop() override { return 0; }
};

class TestFeeder : public ::testing::Test {
 protected:
  std::unique_ptr<feeder> _feeder;

 public:
  void SetUp() override {
    pool::load(0);
    stats::center::load();
    config::applier::state::load();
    multiplexing::engine::load();
    io::protocols::load();
    io::events::load();

    std::unique_ptr<io::stream> client(new TestStream);
    std::unordered_set<uint32_t> read_filters;
    std::unordered_set<uint32_t> write_filters;
    _feeder.reset(
        new feeder("test-feeder", client, read_filters, write_filters));
  }

  void TearDown() override {
    _feeder.reset();
    multiplexing::engine::unload();
    config::applier::state::unload();
    io::events::unload();
    io::protocols::unload();
    stats::center::unload();
    pool::unload();
  }
};

TEST_F(TestFeeder, ImmediateStartExit) {
  ASSERT_NO_THROW(_feeder.reset());
}

TEST_F(TestFeeder, isFinished) {
  // It began
  ASSERT_FALSE(_feeder->is_finished());
  nlohmann::json tree;
  _feeder->stats(tree);
  ASSERT_EQ(tree["state"].get<std::string>(), "connected");
}
