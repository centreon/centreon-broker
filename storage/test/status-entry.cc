/*
 * * Copyright 2021 Centreon (https://www.centreon.com/)
 * *
 * * Licensed under the Apache License, Version 2.0 (the "License");
 * * you may not use this file except in compliance with the License.
 * * You may obtain a copy of the License at
 * *
 * * http://www.apache.org/licenses/LICENSE-2.0
 * *
 * * Unless required by applicable law or agreed to in writing, software
 * * distributed under the License is distributed on an "AS IS" BASIS,
 * * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * * See the License for the specific language governing permissions and
 * * limitations under the License.
 * *
 * * For more information : contact@centreon.com
 * *
 * */

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include <fstream>
#include <list>
#include <memory>

#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/storage/factory.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

class into_memory : public io::stream {
  std::vector<char> _memory;

 public:
  into_memory() : io::stream("into_memory"), _memory() {}
  ~into_memory() override {}
  bool read(std::shared_ptr<io::data>& d,
            time_t deadline = (time_t)-1) override {
    (void)deadline;
    if (_memory.empty())
      return false;
    std::shared_ptr<io::raw> raw(new io::raw);
    raw->get_buffer() = std::move(_memory);
    _memory.clear();
    d = raw;
    return true;
  }

  int32_t write(std::shared_ptr<io::data> const& d) override {
    _memory = std::static_pointer_cast<io::raw>(d)->get_buffer();
    return 1;
  }

  int32_t stop() override { return 0; }

  std::vector<char> const& get_memory() const { return _memory; }
  std::vector<char>& get_mutable_memory() { return _memory; }
};

class StatusEntryTest : public ::testing::Test {
 public:
  void SetUp() override {
    io::data::broker_id = 0;
    try {
      config::applier::init(0, "test_broker");
    } catch (std::exception const& e) {
      (void)e;
    }
    std::shared_ptr<persistent_cache> pcache(
        std::make_shared<persistent_cache>("/tmp/broker_test_cache"));
  }

  void TearDown() override {
    // The cache must be destroyed before the applier deinit() call.
    config::applier::deinit();
    ::remove("/tmp/broker_test_cache");
    ::remove(log_v2::instance().log_name().c_str());
  }
};

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.

TEST_F(StatusEntryTest, WriteStatus) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event status.
  e.register_event(io::events::storage, storage::de_status, "status",
                   &storage::status::operations, storage::status::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::status> st{std::make_shared<storage::status>(
      12345, 123456789123456789, 34567, false, 789789, 2)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm(true);
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(st);

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::status> new_st =
      std::static_pointer_cast<storage::status>(ev);
  ASSERT_EQ(st->ctime, new_st->ctime);
  ASSERT_EQ(st->index_id, new_st->index_id);
  ASSERT_EQ(st->state, new_st->state);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}
