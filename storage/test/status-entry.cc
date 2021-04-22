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
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/storage/factory.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/index_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

class into_memory : public io::stream {
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

  int write(std::shared_ptr<io::data> const& d) override {
    _memory = std::static_pointer_cast<io::raw>(d)->get_buffer();
    return 1;
  }

  std::vector<char> const& get_memory() const { return _memory; }
  std::vector<char>& get_mutable_memory() { return _memory; }

 private:
  std::vector<char> _memory;
};

class StatusEntryTest : public ::testing::Test {
  public:
    void SetUp() override {
      io::data::broker_id = 0;
      try {
        config::applier::init();
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

TEST_F(StatusEntryTest, OldStatus) {
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
        12345, 54321, 34567, false, 789789, 2)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);

  /* An old status message with index_id in int32. */
  std::vector<char> v{
      (char)0x11, (char)0x73, (char)0x00, (char)0x1b, (char)0x00, (char)0x03,
      (char)0x00, (char)0x04, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x30, (char)0x39,
      (char)0x00, (char)0x00, (char)0xd4, (char)0x31, (char)0x00, (char)0x00,
      (char)0x87, (char)0x07, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0x00, (char)0x0c, (char)0x0d, (char)0x1d, (char)0x00, (char)0x02,
      (char)0x00};

  memory_stream->get_mutable_memory() = std::move(v);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

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

TEST_F(StatusEntryTest, OldRebuild) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event rebuild.
  e.register_event(io::events::storage, storage::de_rebuild, "rebuild",
                   &storage::rebuild::operations, storage::rebuild::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::rebuild> st{std::make_shared<storage::rebuild>(
        true, 56789, false)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  //stm.write(st);

  /* An old rebuild message with index_id in int32. */
  std::vector<char> v{
      (char)0xd1, (char)0xcd, (char)0x00, (char)0x06, (char)0x00, (char)0x03,
      (char)0x00, (char)0x02, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x01, (char)0x00,
      (char)0x00, (char)0xdd, (char)0xd5, (char)0x00};
  memory_stream->get_mutable_memory() = std::move(v);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::rebuild> new_st =
        std::static_pointer_cast<storage::rebuild>(ev);
  ASSERT_EQ(st->end, new_st->end);
  ASSERT_EQ(st->id, new_st->id);
  ASSERT_EQ(st->is_index, new_st->is_index);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}

TEST_F(StatusEntryTest, OldIndexMapping) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event index_mapping.
  e.register_event(io::events::storage, storage::de_index_mapping, "index_mapping",
                   &storage::index_mapping::operations, storage::index_mapping::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::index_mapping> st{std::make_shared<storage::index_mapping>(
        true, 56789, false)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  //stm.write(st);

  /* An old index_mapping message with index_id in int32. */
  std::vector<char> v{
    (char)0x1e, (char)0x1d, (char)0x00, (char)0x0c, (char)0x00, (char)0x03, (char)0x00, (char)0x05, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x01, (char)0x00, (char)0x00, (char)0xdd, (char)0xd5, (char)0x00, (char)0x00, (char)0x00, (char)0x00};
  memory_stream->get_mutable_memory() = std::move(v);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::index_mapping> new_st =
        std::static_pointer_cast<storage::index_mapping>(ev);
  ASSERT_EQ(st->index_id, new_st->index_id);
  ASSERT_EQ(st->host_id, new_st->host_id);
  ASSERT_EQ(st->service_id, new_st->service_id);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}

TEST_F(StatusEntryTest, OldRemoveGraph) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event remove_graph.
  e.register_event(io::events::storage, storage::de_remove_graph, "remove_graph",
                   &storage::remove_graph::operations, storage::remove_graph::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::remove_graph> st{std::make_shared<storage::remove_graph>(
        54321, true)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  //stm.write(st);

  /* An old remove_graph message with index_id in int32. */
  std::vector<char> v{
      (char)0x92, (char)0xc7, (char)0x00, (char)0x05, (char)0x00, (char)0x03,
      (char)0x00, (char)0x03, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
      (char)0xd4, (char)0x31, (char)0x01};
  memory_stream->get_mutable_memory() = std::move(v);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::remove_graph> new_st =
        std::static_pointer_cast<storage::remove_graph>(ev);
  ASSERT_EQ(st->id, new_st->id);
  ASSERT_EQ(st->is_index, new_st->is_index);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}

TEST_F(StatusEntryTest, NewStatus) {
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
        12345, 54321, 34567, false, 789789, 2)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(st);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

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

TEST_F(StatusEntryTest, NewRebuild) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event rebuild.
  e.register_event(io::events::storage, storage::de_rebuild, "rebuild",
                   &storage::rebuild::operations, storage::rebuild::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::rebuild> st{std::make_shared<storage::rebuild>(
        true, 56789, false)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(st);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::rebuild> new_st =
        std::static_pointer_cast<storage::rebuild>(ev);
  ASSERT_EQ(st->end, new_st->end);
  ASSERT_EQ(st->id, new_st->id);
  ASSERT_EQ(st->is_index, new_st->is_index);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}

TEST_F(StatusEntryTest, NewIndexMapping) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event index_mapping.
  e.register_event(io::events::storage, storage::de_index_mapping, "index_mapping",
                   &storage::index_mapping::operations, storage::index_mapping::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::index_mapping> st{std::make_shared<storage::index_mapping>(
        true, 56789, false)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(st);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::index_mapping> new_st =
        std::static_pointer_cast<storage::index_mapping>(ev);
  ASSERT_EQ(st->index_id, new_st->index_id);
  ASSERT_EQ(st->host_id, new_st->host_id);
  ASSERT_EQ(st->service_id, new_st->service_id);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}

TEST_F(StatusEntryTest, NewRemoveGraph) {
  io::events& e(io::events::instance());

  // Register category.
  int storage_category(e.register_category("storage", io::events::storage));
  ASSERT_TRUE(storage_category == io::events::storage);

  // Register event remove_graph.
  e.register_event(io::events::storage, storage::de_remove_graph, "remove_graph",
                   &storage::remove_graph::operations, storage::remove_graph::entries);

  // Register storage layer.
  io::protocols::instance().reg("storage", std::make_shared<storage::factory>(),
                                1, 7);

  std::shared_ptr<storage::remove_graph> st{std::make_shared<storage::remove_graph>(
        54321, true)};

  std::shared_ptr<into_memory> memory_stream(std::make_shared<into_memory>());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(st);

  for (auto& c : memory_stream->get_memory())
    printf(" %02x", c);
  std::cout << std::endl;

  std::shared_ptr<io::data> ev;
  stm.read(ev, time(nullptr) + 1000);
  std::shared_ptr<storage::remove_graph> new_st =
        std::static_pointer_cast<storage::remove_graph>(ev);
  ASSERT_EQ(st->id, new_st->id);
  ASSERT_EQ(st->is_index, new_st->is_index);

  io::events::instance().unregister_category(io::events::storage);
  io::protocols::instance().unreg("storage");
}
