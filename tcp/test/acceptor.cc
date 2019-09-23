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

#include "com/centreon/broker/tcp/acceptor.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/tcp/connector.hh"

using namespace com::centreon::broker;

class TcpAcceptor : public testing::Test {
 public:
  void SetUp(void) override { logging::manager::load(); }

  void TearDown(void) override { logging::manager::unload(); }
};

TEST_F(TcpAcceptor, BadPort) {
  tcp::acceptor acc;

  acc.listen_on(2);
  ASSERT_THROW(acc.open(), exceptions::msg);
}

TEST_F(TcpAcceptor, Simple) {
  tcp::acceptor acc;

  acc.listen_on(4242);

  std::thread t{[] {
    tcp::connector con;
    con.connect_to("127.0.0.1", 4242);
    std::shared_ptr<io::stream> str{con.open()};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    data->append("TEST\n");
    str->write(data);
    str->read(data_read, -1);
  }};
  std::shared_ptr<io::stream> io{acc.open()};
  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data_read;
  io->read(data_read);

  std::vector<char> vec{
      std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
  std::string str{vec.begin(), vec.end()};
  ASSERT_TRUE(str == "TEST\n");

  data->append("TEST\n");
  io->write(data);

  t.join();
}

TEST_F(TcpAcceptor, Multiple) {
  tcp::acceptor acc;
  acc.set_read_timeout(-1);
  acc.set_write_timeout(-1);

  acc.listen_on(4242);
  {
    std::thread t{[] {
      tcp::connector con;
      con.connect_to("127.0.0.1", 4242);
      std::shared_ptr<io::stream> str{con.open()};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append("TEST\n");
      str->write(data);
      str->read(data_read, -1);
    }};
    std::shared_ptr<io::stream> io{acc.open()};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    io->read(data_read);

    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "TEST\n");

    data->append("TEST\n");
    io->write(data);

    t.join();
  }
  {
    std::thread t{[] {
      tcp::connector con;
      con.connect_to("127.0.0.1", 4242);
      std::shared_ptr<io::stream> str{con.open()};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append("TEST\n");
      str->write(data);
      str->read(data_read, -1);
    }};
    std::shared_ptr<io::stream> io{acc.open()};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    io->read(data_read);

    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "TEST\n");

    data->append("TEST\n");
    io->write(data);

    t.join();
  }
}

TEST_F(TcpAcceptor, BigSend) {
  tcp::acceptor acc;

  acc.listen_on(4242);

  std::thread t{[] {
    tcp::connector con;
    con.connect_to("127.0.0.1", 4242);
    std::shared_ptr<io::stream> str{con.open()};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    for (int i = 0; i < 1024; i++) {
      data->append("0123456789");
    }
    data->append("01234");
    str->write(data);
  }};
  std::shared_ptr<io::stream> io{acc.open()};
  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data_read;
  io->read(data_read, time(nullptr) + 5);

  std::vector<char> vec{
      std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
  std::string str{vec.begin(), vec.end()};

  data->append("TEST\n");
  io->write(data);

  ASSERT_TRUE(str.length() == 10245);

  t.join();
}