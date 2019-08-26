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

#include <QCoreApplication>
#include <QTimer>
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

class DummyThread : public thread {
  void run() override {
    time_t valid_time{time(nullptr) + 1};
    while (!should_exit() && time(nullptr) < valid_time) {
      QTimer::singleShot(1000, this, SLOT(quit()));
      std::cout << "Wait loop" << std::endl;
      exec();
    }
    std::cout << "Thread finished" << std::endl;
  }
  std::string _get_state() { return "test"; }
  unsigned int _get_queued_events() { return 0; }
  std::unordered_set<unsigned int> _get_write_filters() {
    std::unordered_set<unsigned int> retval;
    return retval;
  }
  std::unordered_set<unsigned int> _get_read_filters() {
    std::unordered_set<unsigned int> retval;
    return retval;
  }
 public:
  bool isRunning() {
    return QThread::isRunning();
  }
};

class TestThread : public ::testing::Test {
 public:
  void  SetUp() {
    _thread.reset(new DummyThread);
  }

 protected:
  std::unique_ptr<DummyThread> _thread;
};

TEST_F(TestThread, TimerBeforeExit) {
  int argc{0};
  char* argv[] = { nullptr };
  QCoreApplication app(argc, argv);
  _thread->start();
  time_t now{time(nullptr) + 2};
  do {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
  } while (time(nullptr) <= now);
  ASSERT_FALSE(_thread->isRunning());
}

TEST_F(TestThread, Wait) {
  int argc{0};
  char* argv[] = { nullptr };
  QCoreApplication app(argc, argv);
  ASSERT_TRUE(_thread->wait(100));
  _thread->start();
  ASSERT_FALSE(_thread->wait(100));
  time_t now{time(nullptr) + 2};
  do {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
  } while (time(nullptr) <= now);
  ASSERT_FALSE(_thread->isRunning());
  ASSERT_TRUE(_thread->wait(100));
}
