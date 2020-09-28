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

#include "com/centreon/broker/processing/thread.hh"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class DummyThread : public bthread {
  std::string _filters;

 public:
  DummyThread() : bthread(), _filters("fake_filter") { set_state("test"); }

  ~DummyThread() { exit(); }
  void run() override {
    while (!should_exit()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  std::string const& _get_write_filters() const override {
    return _filters;
  }
  std::string const& _get_read_filters() const override {
    return _filters;
  }

  bool isRunning() { return is_running(); }
  uint32_t _get_queued_events() const override { return 0; }
};

class TestThread : public ::testing::Test {
 public:
  void SetUp() override {

    _thread.reset(new DummyThread); }

 protected:
  std::unique_ptr<DummyThread> _thread;
};

TEST_F(TestThread, TimerBeforeExit) {
  ASSERT_FALSE(_thread->isRunning());
}

TEST_F(TestThread, NoExit) {
  ASSERT_NO_THROW(_thread->start());
}
