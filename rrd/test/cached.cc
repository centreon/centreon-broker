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

#include "com/centreon/broker/rrd/cached.hh"

#include <gtest/gtest.h>

#include <asio.hpp>
#include <fstream>

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(RRDCached, LibExisting) {
  rrd::cached<asio::local::stream_protocol::socket> cached{"/tmp", 42};
  std::remove("/tmp/test_rrd");
  ASSERT_THROW(cached.open("/tmp/test_rrd"), msg_fmt);
  std::ofstream ofs("/tmp/test_rrd");
  ofs.close();
  cached.clean();
  ASSERT_NO_THROW(cached.open("/tmp/test_rrd"));
  cached.remove("/tmp/test_rrd");
  cached.remove("/tmp/test_rrd");
}

TEST(RRDCached, LibNew) {
  rrd::cached<asio::ip::tcp::socket> cached{"/tmp", 42};

  std::remove("/tmp/test_rrd");
  cached.open("/tmp/test_rrd", 3600, time(nullptr), 1, 60);
  cached.remove("/tmp/test_rrd");
}

TEST(RRDCached, BatchLocal) {
  std::atomic_bool init_done{false};
  std::atomic_bool batch_done{false};
  testing::internal::CaptureStdout();
  ::unlink("/tmp/foobar");  // Remove previous binding.
  rrd::cached<asio::local::stream_protocol::socket> cached{"tmp", 42};

  ASSERT_THROW(cached.begin(), msg_fmt);

  std::thread t{[&] {
    asio::io_context io;
    asio::local::stream_protocol::endpoint ep("/tmp/foobar");
    asio::local::stream_protocol::acceptor acceptor(io, ep);
    asio::local::stream_protocol::socket socket(io);
    init_done = true;
    acceptor.accept(socket);
    std::cout << "connected" << std::endl;
    std::string buf;
    buf.resize(128);

    socket.read_some(asio::buffer(buf, 128));
    if (buf.find("BATCH\n") != std::string::npos)
      batch_done = true;

    socket.read_some(asio::buffer(buf, 128));
    if (buf.find(".\n") != std::string::npos)
      socket.write_some(asio::buffer("dassda\n", 7));

    buf.clear();
    buf.resize(128);
    while (buf.find(".\n") == std::string::npos)
      socket.read_some(asio::buffer(buf, 128));

    socket.write_some(asio::buffer("2\nTOTO\nTUTU\n", 12));
  }};

  while (!init_done)
    ;

  ASSERT_THROW(cached.connect_local("/tmp/toto"), msg_fmt);
  ASSERT_NO_THROW(cached.connect_local("/tmp/foobar"));
  cached.begin();
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  ASSERT_THROW(cached.commit(), msg_fmt);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  cached.begin();
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  cached.update(time(NULL), "4.23231");
  cached.update(time(NULL), "4.32");
  cached.update(time(NULL), "dfsasd");
  cached.update(time(NULL), "4.23231");
  cached.commit();

  t.join();
  ASSERT_TRUE(batch_done);
  ASSERT_EQ(testing::internal::GetCapturedStdout(), "connected\n");
}

TEST(RRDCached, BatchRemote) {
  std::atomic_bool init_done{false};
  std::atomic_bool batch_done{false};
  testing::internal::CaptureStdout();
  ::unlink("/tmp/foobar");  // Remove previous binding.
  rrd::cached<asio::ip::tcp::socket> cached{"tmp", 42};

  ASSERT_THROW(cached.begin(), msg_fmt);

  std::thread t{[&] {
    asio::io_context io;
    asio::ip::tcp::acceptor acceptor(
        io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 4242));
    asio::ip::tcp::socket socket(io);
    init_done = true;
    acceptor.accept(socket);
    std::cout << "connected" << std::endl;
    std::string buf;
    buf.resize(128);

    socket.read_some(asio::buffer(buf, 128));
    if (buf.find("BATCH\n") != std::string::npos)
      batch_done = true;

    socket.read_some(asio::buffer(buf, 128));
    if (buf.find(".\n") != std::string::npos)
      socket.write_some(asio::buffer("dassda\n", 7));

    buf.clear();
    buf.resize(128);
    while (buf.find(".\n") == std::string::npos)
      socket.read_some(asio::buffer(buf, 128));

    socket.write_some(asio::buffer("2\nTOTO\nTUTU\n", 12));
  }};

  while (!init_done)
    ;

  ASSERT_THROW(cached.connect_remote("badurl.centreon.org", 4242), msg_fmt);
  ASSERT_THROW(cached.connect_remote("localhost", 2), msg_fmt);
  ASSERT_NO_THROW(cached.connect_remote("localhost", 4242));
  cached.begin();
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  ASSERT_THROW(cached.commit(), msg_fmt);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  cached.begin();
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  cached.update(time(NULL), "4.23231");
  cached.update(time(NULL), "4.32");
  cached.update(time(NULL), "dfsasd");
  cached.update(time(NULL), "4.23231");
  cached.commit();

  t.join();
  ASSERT_TRUE(batch_done);
  ASSERT_EQ(testing::internal::GetCapturedStdout(), "connected\n");
}
