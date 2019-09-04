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

#include "com/centreon/broker/processing/acceptor.hh"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/raw.hh"
#include "temporary_endpoint.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class ProcessingTest : public ::testing::Test {
 public:
  void SetUp() {
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }

    std::shared_ptr<io::endpoint> endpoint =
        std::make_shared<temporary_endpoint>();
    _acceptor.reset(new acceptor(endpoint, "temporary_endpoint"));
  }

  void TearDown() {
    _acceptor.reset();
    config::applier::deinit();
  }

 protected:
  std::unique_ptr<acceptor> _acceptor;
};

TEST_F(ProcessingTest, NotStarted) {
  ASSERT_NO_THROW(_acceptor->exit());
}

TEST_F(ProcessingTest, StartStop) {
  _acceptor->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_NO_THROW(_acceptor->exit());
}

TEST_F(ProcessingTest, StartWithFilterStop) {
  std::unordered_set<uint32_t> filters;
  filters.insert(io::raw::static_type());
  _acceptor->set_read_filters(filters);
  time_t now{time(nullptr)};
  _acceptor->set_retry_interval(2);
  _acceptor->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  ASSERT_NO_THROW(_acceptor->exit());
  time_t now1{time(nullptr)};
  ASSERT_TRUE(now1 >= now + 2);
}
