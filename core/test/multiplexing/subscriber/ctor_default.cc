/*
** Copyright 2011-2013 Centreon
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

#include <gtest/gtest.h>
#include <cstring>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

#define MSG "0123456789abcdef"

class SubscriberTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialization.
    config::applier::init();
    multiplexing::engine::instance().start();
  }

  void TearDown() override {
    // Cleanup.
    config::applier::deinit();
  }
};

TEST_F(SubscriberTest, DefaultConstructor) {
  // Subscriber.
  std::unordered_set<uint32_t> filters;
  filters.insert(io::raw::static_type());
  multiplexing::subscriber s("core_multiplexing_subscriber_ctor_default", "");
  s.get_muxer().set_read_filters(filters);
  s.get_muxer().set_write_filters(filters);

  // Check that subscriber is empty.
  std::shared_ptr<io::data> event;
  s.get_muxer().read(event, 0);
  ASSERT_FALSE(event);

  // Write data to subscriber.
  std::shared_ptr<io::raw> data = std::make_shared<io::raw>();
  data->append(MSG);
  s.get_muxer().write(std::static_pointer_cast<io::data>(data));

  // Fetch event.
  s.get_muxer().read(event, 0);
  ASSERT_TRUE(event);
  ASSERT_EQ(event->type(), io::raw::static_type());
  ASSERT_TRUE(strncmp(std::static_pointer_cast<io::raw>(event)->const_data(),
                      MSG, sizeof(MSG) - 1) == 0);

  // Try reading again.
  s.get_muxer().read(event, 0);
  ASSERT_FALSE(event);
}
