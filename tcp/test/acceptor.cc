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
#include <json11.hpp>
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/tcp/connector.hh"

using namespace com::centreon::broker;

constexpr static char test_addr[] = "127.0.0.1";
constexpr static uint16_t test_port(4444);

static auto try_connect = [](tcp::connector& con,
                             uint32_t port =
                                 test_port) -> std::shared_ptr<io::stream> {
  con.connect_to(test_addr, port);
  while (true) {
    try {
      return con.open();
    } catch (...) {

    }
  }
};

TEST(TcpAcceptor, BadPort) {
  tcp::acceptor acc;

  if (getuid() != 0) {
    acc.listen_on(2);
    ASSERT_THROW(acc.open(), exceptions::msg);
  }
}

TEST(TcpAcceptor, NoConnector) {
  tcp::acceptor acc;

  acc.listen_on(test_port);
  ASSERT_EQ(acc.open(), std::shared_ptr<io::stream>());
}

TEST(TcpAcceptor, Wait2Connect) {
  tcp::acceptor acc;
  int i = 0;
  std::shared_ptr<io::stream> st;

  acc.listen_on(4141);
  std::thread t{[&] {
    std::this_thread::sleep_for(std::chrono::seconds {2});
    tcp::connector con;
    std::shared_ptr<io::stream> str{try_connect(con, 4141)};

    std::cout << "connected" << std::endl;
  }};

  st == std::shared_ptr<io::stream>();
  while (st == std::shared_ptr<io::stream>()) {
    i++;
    std::cout << "before accept" << std::endl;
    st = acc.open();
  }

  t.join();
  ASSERT_GT(i, 1);
}

TEST(TcpAcceptor, Simple) {
  tcp::acceptor acc;
  acc.listen_on(test_port);

  std::thread t{[&] {
    tcp::connector con;
    std::shared_ptr<io::stream> str{try_connect(con)};
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

TEST(TcpAcceptor, Multiple) {
  tcp::acceptor acc;
  acc.set_read_timeout(-1);
  acc.set_write_timeout(-1);

  acc.listen_on(test_port);
  {
    std::thread t{[] {
      tcp::connector con;
      std::shared_ptr<io::stream> str{try_connect(con)};
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
      std::shared_ptr<io::stream> str{try_connect(con)};
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

TEST(TcpAcceptor, BigSend) {
  tcp::acceptor acc;

  acc.listen_on(test_port);

  std::thread t{[] {
    tcp::connector con;
    std::shared_ptr<io::stream> str{try_connect(con)};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    for (int i = 0; i < 1024; i++) {
      data->append("0123456789");
    }
    data->append("01234");
    str->write(data);
    str->read(data_read, -1);
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

TEST(TcpAcceptor, CloseRead) {
  tcp::acceptor acc;

  acc.listen_on(test_port);

  std::thread t{[&] {
    {
      tcp::connector con;
      std::shared_ptr<io::stream> str{try_connect(con)};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append("0");
      str->write(data);
    }
  }};
  std::shared_ptr<io::stream> io{acc.open()};
  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data_read;

  t.join();
  while (true) {
    try {
      io->read(data_read, -1);
    } catch (exceptions::msg const& ex) {
      break;
    }
  }
}

TEST(TcpAcceptor, ChildsAndStats) {
  tcp::acceptor acc;

  acc.add_child("child1");
  acc.add_child("child2");
  acc.add_child("child3");
  acc.remove_child("child2");

  json11::Json::object obj;
  acc.stats(obj);
  json11::Json js{obj};
  ASSERT_EQ(js.dump(), "{\"peers\": \"2: child1, child3\"}");
}