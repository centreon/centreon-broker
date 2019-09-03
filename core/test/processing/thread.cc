/*
** Copyright 2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/processing/thread.hh"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class DummyThread : public bthread {
  void run() override {
    std::cout << "Wait loop" << std::endl;
    while (!should_exit()) {
      std::cout << "loop" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return;
    std::cout << "Thread finished" << std::endl;
  }

  std::string _get_state() { return "test"; }
  uint32_t _get_queued_events() { return 0; }
  std::unordered_set<uint32_t> const& _get_write_filters() const {
    return _filters;
  }
  std::unordered_set<uint32_t> const& _get_read_filters() const {
    return _filters;
  }

 public:
  bool isRunning() { return is_running(); }

 private:
  std::unordered_set<uint32_t> _filters;
};

class TestThread : public ::testing::Test {
 public:
  void SetUp() { _thread.reset(new DummyThread); }

 protected:
  std::unique_ptr<DummyThread> _thread;
};

TEST_F(TestThread, TimerBeforeExit) {
  ASSERT_FALSE(_thread->isRunning());
}

TEST_F(TestThread, Wait) {
  ASSERT_TRUE(_thread->wait(100));
  _thread->start();
  ASSERT_FALSE(_thread->wait(100));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_TRUE(_thread->isRunning());
  _thread->exit();
  std::cout << "COUCOU" << std::endl;
  ASSERT_TRUE(_thread->wait(1500));
}
