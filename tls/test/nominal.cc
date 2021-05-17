/*
 * Copyright 2011 - 2021 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/tcp/acceptor.hh"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

const static std::string test_addr("127.0.0.1");
constexpr static uint16_t test_port(4444);

class TcpAcceptor : public ::testing::Test {
 public:
  void SetUp() override { pool::load(0); }

  void TearDown() override {
    tcp::tcp_async::instance().stop_timer();
    pool::unload();
  }
};

TEST_F(TcpAcceptor, QuestionAnswer) {
  constexpr int rep = 100;

  std::thread cbd([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    /* Nominal case, cbd is acceptor and read on the socket */
    std::unique_ptr<io::stream> u_cbd;
    do {
      u_cbd = endp->open();
    } while (!u_cbd);

    std::shared_ptr<io::data> data_read;
    std::shared_ptr<io::raw> data_write;
    bool val;
    for (int i = 0; i < rep; i++) {
      val = false;
      std::string wanted(fmt::format("Question{}", i));
      while (!val || !data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() <
                 wanted.size()) {
        val = u_cbd->read(data_read, static_cast<time_t>(0));
      }

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());

      ASSERT_EQ(wanted, result);

      data_write = std::make_shared<io::raw>();
      std::string text(fmt::format("Answer{}", i));
      std::string cc("a");
      for (auto c : text) {
        cc[0] = c;
        data_write->append(cc);
      }
      u_cbd->write(data_write);
    }
    int retry = 10;
    while (retry-- && u_cbd->flush() == 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  std::thread centengine([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_centengine;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    std::shared_ptr<io::data> data_read;
    std::shared_ptr<io::raw> data_write;
    bool val;
    for (int i = 0; i < rep; i++) {
      data_write = std::make_shared<io::raw>();
      std::string text(fmt::format("Question{}", i));
      std::string cc("a");
      for (auto c : text) {
        cc[0] = c;
        data_write->append(cc);
      }
      s_centengine->write(data_write);

      val = false;
      std::string wanted(fmt::format("Answer{}", i));
      while (!val || !data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() <
                 wanted.size())
        val = s_centengine->read(data_read, static_cast<time_t>(0));

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());

      ASSERT_EQ(wanted, result);
    }
  });

  centengine.join();
  cbd.join();
}
