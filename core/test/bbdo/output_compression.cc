/*
 * Copyright 2020 Centreon (https://www.centreon.com/)
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
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/persistent_file.hh"

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

class OutputCompressTest : public ::testing::Test {
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

TEST_F(OutputCompressTest, WriteReadService) {
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 12345;
  svc->service_id = 18;

  svc->output.reserve(100000);
  char c = 'a';
  for (auto it = svc->output.begin(); it != svc->output.end(); ++it) {
    *it = c++;
    if (c > 'z')
      c = 'a';
  }

  svc->perf_data.reserve(100000);
  c = '0';
  for (auto it = svc->perf_data.begin(); it != svc->perf_data.end(); ++it) {
    *it = c++;
    if (c > '9')
      c = '0';
  }
  svc->last_time_ok = timestamp(0x1122334455667788);  // 0x1cbe991a83

  std::shared_ptr<io::stream> stream;
  std::shared_ptr<into_memory> memory_stream(new into_memory());
  bbdo::stream stm;
  std::shared_ptr<compression::stream> compress =
      std::make_shared<compression::stream>();
  compress->set_substream(memory_stream);
  stm.set_substream(compress);
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
