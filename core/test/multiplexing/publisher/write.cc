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

#include <gtest/gtest.h>

#include <cstring>
#include <thread>

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

const std::string MSG1("0123456789abcdef");
const std::string MSG2("foo bar baz qux");

class PublisherWrite : public testing::Test {
 public:
  void SetUp() override { config::applier::init(0, "test_broker"); }

  void TearDown() override { config::applier::deinit(); }
};

/**
 *  We should be able to read from publisher.
 */
TEST_F(PublisherWrite, Write) {
  int retval{0};
  {
    // Publisher.
    multiplexing::publisher p;

    // Subscriber.
    std::unordered_set<uint32_t> filters{io::raw::static_type()};
    multiplexing::subscriber s("core_multiplexing_publisher_write", "");
    s.get_muxer().set_read_filters(filters);
    s.get_muxer().set_write_filters(filters);

    // Publish event.
    {
      auto raw{std::make_shared<io::raw>()};
      raw->append(MSG1);
      p.write(raw);
    }

    // Launch multiplexing.
    multiplexing::engine::instance().start();

    // Publish another event.
    {
      auto raw{std::make_shared<io::raw>()};
      raw->append(MSG2);
      p.write(raw);
    }

    // Check data.
    std::array<std::string, 2> messages{MSG1, MSG2};
    for (auto& m : messages) {
      std::shared_ptr<io::data> data;
      bool ret;
      int count = 0;
      do {
        ret = s.get_muxer().read(data, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        count++;
      } while (!ret && count < 100);
      if (!data || data->type() != io::raw::static_type())
        retval |= 1;
      else {
        std::shared_ptr<io::raw> raw(std::static_pointer_cast<io::raw>(data));
        retval |= strncmp(raw->const_data(), m.c_str(), m.size());
      }
    }
  }
  // Return.
  ASSERT_EQ(retval, 0);
}
