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

#include <cstdlib>
#include <iostream>

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

const std::string MSG1("0123456789abcdef");
const std::string MSG2("foo bar baz");
const std::string MSG3("last message with qux");
const std::string MSG4("no this is the last message");

class StartStop : public testing::Test {
 public:
  void SetUp() override { config::applier::init(); }

  void TearDown() override { config::applier::deinit(); }
};

/**
 *  Check that multiplexing engine works properly.
 *
 *  @return 0 on success.
 */
TEST_F(StartStop, MultiplexingWorks) {
  // Initialization.
  bool error{true};

  try {
    // Subscriber.
    std::unordered_set<uint32_t> filters;
    filters.insert(io::raw::static_type());
    multiplexing::subscriber s("core_multiplexing_engine_start_stop", false);
    s.get_muxer().set_read_filters(filters);
    s.get_muxer().set_write_filters(filters);

    // Send events through engine.
    std::array<std::string, 2> messages{MSG1, MSG2};
    for (auto& m : messages) {
      std::shared_ptr<io::raw> data(new io::raw);
      data->append(m);
      multiplexing::engine::instance().publish(
          std::static_pointer_cast<io::data>(data));
    }

    // Should read no events from subscriber.
    {
      std::shared_ptr<io::data> data;
      s.get_muxer().read(data, 0);
      if (data)
        throw exceptions::msg() << "error at step #1";
    }

    // Start multiplexing engine.
    multiplexing::engine::instance().start();

    // Read retained events.
    for (auto& m : messages) {
      std::shared_ptr<io::data> data;
      s.get_muxer().read(data, 0);
      if (!data || data->type() != io::raw::static_type())
        throw exceptions::msg() << "error at step #2";
      else {
        std::shared_ptr<io::raw> raw(std::static_pointer_cast<io::raw>(data));
        if (strncmp(raw->const_data(), m.c_str(), m.size()))
          throw exceptions::msg() << "error at step #3";
      }
    }

    // Publish a new event.
    {
      std::shared_ptr<io::raw> data(new io::raw);
      data->append(MSG3);
      multiplexing::engine::instance().publish(
          std::static_pointer_cast<io::data>(data));
    }

    // Read event.
    {
      std::shared_ptr<io::data> data;
      s.get_muxer().read(data, 0);
      if (!data || data->type() != io::raw::static_type())
        throw exceptions::msg() << "error at step #4";
      else {
        std::shared_ptr<io::raw> raw(std::static_pointer_cast<io::raw>(data));
        if (strncmp(raw->const_data(), MSG3.c_str(), MSG3.size()))
          throw exceptions::msg() << "error at step #5";
      }
    }

    // Stop multiplexing engine.
    multiplexing::engine::instance().stop();

    // Publish a new event.
    {
      std::shared_ptr<io::raw> data(new io::raw);
      data->append(MSG4);
      multiplexing::engine::instance().publish(
          std::static_pointer_cast<io::data>(data));
    }

    // Read no event.
    {
      std::shared_ptr<io::data> data;
      s.get_muxer().read(data, 0);
      if (data)
        throw exceptions::msg() << "error at step #6";
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  } catch (...) {
    std::cerr << "unknown exception\n";
  }

  // Return.
  ASSERT_FALSE(error);
}
