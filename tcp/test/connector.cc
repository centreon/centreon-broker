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

#include "com/centreon/broker/tcp/connector.hh"
#include <gtest/gtest.h>
#include <com/centreon/broker/tcp/stream.hh>
#include "../../core/test/test_server.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

class TcpConnector : public testing::Test {
 public:
  void SetUp() override {
    logging::manager::load();
    std::thread t{[&] {
      _server.init();
      _server.run();
    }};

    _thread = std::move(t);

    while (!_server.get_init_done())
      ;
  }
  void TearDown() override {
    if (_server.get_init_done())
      _server.stop();
    _thread.join();

    logging::manager::unload();
  }

  test_server _server;
  std::thread _thread;
};

TEST_F(TcpConnector, InvalidHost) {
  tcp::connector connector;

  connector.connect_to("htrekf';kfdsa'", 4242);
  ASSERT_THROW(connector.open(), exceptions::msg);
}

TEST_F(TcpConnector, NoConnection) {
  tcp::connector connector;

  connector.connect_to("127.0.0.1", 2);
  ASSERT_THROW(connector.open(), exceptions::msg);
}

TEST_F(TcpConnector, Timeout) {
  tcp::connector connector;

  connector.connect_to("127.0.0.1", 4242);
  connector.set_read_timeout(1);
  std::shared_ptr<io::stream> io{connector.open()};

  std::shared_ptr<io::data> data{new io::raw()};

  ASSERT_FALSE(io->read(data, time(NULL) + 10));
}

TEST_F(TcpConnector, Simple) {
  tcp::connector connector;

  connector.connect_to("127.0.0.1", 4242);
  connector.set_read_timeout(-1);
  std::shared_ptr<io::stream> io{connector.open()};

  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data2{new io::raw()};

  data->append("PING\n");
  io->write(data);

  ASSERT_TRUE(io->peer() == "tcp://127.0.0.1:4242");

  ASSERT_TRUE(io->read(data2, ::time(nullptr) + 5));

  std::vector<char> vec{std::static_pointer_cast<io::raw>(data2)->get_buffer()};
  std::string str{vec.begin(), vec.end()};
  ASSERT_TRUE(str == "PONG\n");
}

TEST_F(TcpConnector, ReadAfterTimeout) {
  tcp::connector connector;

  connector.connect_to("127.0.0.1", 4242);
  connector.set_read_timeout(-1);
  std::shared_ptr<io::stream> io{connector.open()};

  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data2{new io::raw()};

  data->append("SERV_DELAY\n");
  io->write(data);

  ASSERT_FALSE(io->read(data2, ::time(nullptr) + 1));
  while(!io->read(data2, ::time(nullptr) + 1));

  std::vector<char> vec{std::static_pointer_cast<io::raw>(data2)->get_buffer()};
  std::string str{vec.begin(), vec.end()};
  ASSERT_TRUE(str == "PONG\n");
}

TEST_F(TcpConnector, MultipleSimple) {
  tcp::connector connector;

  connector.connect_to("127.0.0.1", 4242);
  connector.set_read_timeout(-1);
  std::shared_ptr<io::stream> io{connector.open()};

  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data2{new io::raw()};

  data->append("PING\n");
  io->write(data);

  ASSERT_TRUE(io->read(data2, ::time(nullptr) + 5));
  {
    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data2)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "PONG\n");
  }

  io->write(data);
  ASSERT_TRUE(io->read(data2, ::time(nullptr) + 5));
  {
    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data2)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "PONG\n");
  }

  io->write(data);
  ASSERT_TRUE(io->read(data2, ::time(nullptr) + 5));
  {
    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data2)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "PONG\n");
  }
}