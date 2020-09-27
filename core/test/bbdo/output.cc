/*
 * Copyright 2011 - 2020 Centreon (https://www.centreon.com/)
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

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include <fstream>
#include <list>
#include <memory>

#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/persistent_file.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

class into_memory : public io::stream {
 public:
  into_memory() : _memory() {}
  ~into_memory() override {}

  bool read(std::shared_ptr<io::data>& d,
            time_t deadline = (time_t)-1) override {
    (void)deadline;
    std::shared_ptr<io::raw> raw(new io::raw);
    raw->get_buffer() = _memory;
    d = raw;
    return true;
  }

  int write(std::shared_ptr<io::data> const& d) override {
    _memory = std::static_pointer_cast<io::raw>(d)->get_buffer();
    return 1;
  }

  std::vector<char> const& get_memory() const { return _memory; }

 private:
  std::vector<char> _memory;
};

class OutputTest : public ::testing::Test {
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
  }
};

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.
TEST_F(OutputTest, WriteService) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::shared_ptr<neb::service> svc(std::make_shared<neb::service>());
  svc->host_id = 12345;
  svc->service_id = 18;
  svc->output = "Bonjour";
  svc->last_time_ok = timestamp(0x1122334455667788);  // 0x1cbe991a83

  std::shared_ptr<io::stream> stream;
  std::shared_ptr<into_memory> memory_stream(new into_memory());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(svc);
  std::vector<char> const& mem1 = memory_stream->get_memory();

  ASSERT_EQ(mem1.size(), 276u);
  // The size is 276 - 16: 16 is the header size.
  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 146)),
            0x11223344u);
  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 150)),
            0x55667788u);
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 2)), 260u);
  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 91)), 12345u);
  ASSERT_EQ(std::string(&mem1[0] + 265), "Bonjour");

  ASSERT_EQ(std::string(&mem1[0] + 265), "Bonjour");
  svc->host_id = 78113;
  svc->service_id = 18;
  svc->output = "Conjour";
  stm.write(svc);
  std::vector<char> const& mem2 = memory_stream->get_memory();

  ASSERT_EQ(mem2.size(), 276u);
  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 91)), 78113u);
  // The size is 276 - 16: 16 is the header size.
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 2)), 260u);

  // Check checksum
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0])), 33491u);

  ASSERT_EQ(std::string(&mem1[0] + 265), "Conjour");
  l.unload();
}

TEST_F(OutputTest, WriteLongService) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  auto svc = std::make_shared<neb::service>();
  svc->host_id = 12;
  svc->service_id = 18;
  svc->output.reserve(70000);
  char c = 'A';
  for (uint32_t i = 0; i < svc->output.capacity(); i++) {
    svc->output.push_back(c++);
    if (c > 'Z')
      c = 'A';
  }

  std::shared_ptr<io::stream> stream;
  std::shared_ptr<into_memory> memory_stream(new into_memory());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(svc);
  std::vector<char> const& mem1 = memory_stream->get_memory();

  ASSERT_EQ(mem1.size(), 73956u);

  // The buffer is too big. So it is splitted into several -- here 2 -- buffers.
  // The are concatenated, keeped into the same block, but a new header is
  // inserted in the "middle" of the buffer: Something like this:
  //     HEADER | BUFFER1 | HEADER | BUFFER2
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 2)), 0xffffu);
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0])), 48152u);

  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 91)), 12u);

  // Second block
  // We have 73956 = HeaderSize + block1 + HeaderSize + block2
  //      So 73956 = 16         + 0xffff + 16         + 8389
  ASSERT_EQ(
      htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 16 + 65535 + 2)),
      8389u);

  // Check checksum
  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 16 + 65535)),
            63960u);
}

TEST_F(OutputTest, WriteReadService) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 12345;
  svc->service_id = 18;

  svc->output.reserve(1000000);
  char c = 'a';
  for (uint32_t i = 0; i < svc->output.capacity(); i++) {
    svc->output.push_back(c++);
    if (c > 'z')
      c = 'a';
    ASSERT_EQ(svc->output.size(), i + 1);
  }

  svc->perf_data.reserve(1000000);
  c = '0';
  for (uint32_t i = 0; i < svc->perf_data.capacity(); i++) {
    svc->perf_data.push_back(c++);
    if (c > '9')
      c = '0';
    ASSERT_EQ(svc->perf_data.size(), i + 1);
  }
  svc->last_time_ok = timestamp(0x1122334455667788);  // 0x1cbe991a83

  std::shared_ptr<io::stream> stream;
  std::shared_ptr<into_memory> memory_stream(new into_memory());
  bbdo::stream stm;
  stm.set_substream(memory_stream);
  stm.set_coarse(false);
  stm.set_negotiate(false);
  stm.negotiate(bbdo::stream::negotiate_first);
  stm.write(svc);

  std::shared_ptr<io::data> e;
  stm.read(e, time(nullptr) + 1000);
  std::shared_ptr<neb::service> new_svc =
      std::static_pointer_cast<neb::service>(e);
  ASSERT_EQ(svc->output, new_svc->output);
  ASSERT_EQ(svc->perf_data, new_svc->perf_data);
  l.unload();
}

TEST_F(OutputTest, ShortPersistentFile) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::shared_ptr<neb::service_status> svc(new neb::service_status);
  svc->host_id = 12345;
  svc->service_id = 18;
  svc->acknowledged = true;

  svc->output.reserve(1000);
  char c = 'a';
  for (uint32_t i = 0; i < svc->output.capacity(); i++) {
    svc->output.push_back(c++);
    if (c > 'z')
      c = 'a';
  }

  svc->perf_data.reserve(100);
  c = '0';
  for (uint32_t i = 0; i < svc->perf_data.capacity(); i++) {
    svc->perf_data.push_back(c++);
    if (c > '9')
      c = '0';
  }
  svc->last_time_ok = timestamp(0x1122334455667788);  // 0x1cbe991a83

  std::unique_ptr<io::stream> mf(new persistent_file("/tmp/test_output"));
  mf->write(svc);

  std::shared_ptr<io::data> e;
  mf.reset();


  mf.reset(new persistent_file("/tmp/test_output"));
  mf->read(e, 0);

  std::shared_ptr<neb::service_status> new_svc =
      std::static_pointer_cast<neb::service_status>(e);
  ASSERT_EQ(svc->acknowledged, new_svc->acknowledged);
  ASSERT_EQ(svc->output, new_svc->output);
  ASSERT_EQ(svc->perf_data, new_svc->perf_data);

  l.unload();
  std::remove("/tmp/test_output");
}

TEST_F(OutputTest, LongPersistentFile) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 12345;
  svc->service_id = 18;
  svc->acknowledged = true;

  svc->output.reserve(100000);
  char c = 'a';
  for (uint32_t i = 0; i < svc->output.capacity(); i++) {
    svc->output.push_back(c++);
    if (c > 'z')
      c = 'a';
  }

  svc->perf_data.reserve(100000);
  c = '0';
  for (uint32_t i = 0; i < svc->perf_data.capacity(); i++) {
    svc->perf_data.push_back(c++);
    if (c > '9')
      c = '0';
  }
  svc->last_time_ok = timestamp(0x1122334455667788);  // 0x1cbe991a83

  std::unique_ptr<io::stream> mf(new persistent_file("/tmp/long_output"));
  mf->write(svc);

  std::shared_ptr<io::data> e;
  mf.reset();


  mf.reset(new persistent_file("/tmp/long_output"));
  mf->read(e, 0);

  std::shared_ptr<neb::service> new_svc =
      std::static_pointer_cast<neb::service>(e);
  ASSERT_EQ(svc->acknowledged, new_svc->acknowledged);
  ASSERT_EQ(svc->output, new_svc->output);
  ASSERT_EQ(svc->perf_data, new_svc->perf_data);

  l.unload();
  std::remove("/tmp/long_output");
}
