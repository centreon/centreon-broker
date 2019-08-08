/*
** Copyright 2018 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <asio.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <list>

struct test_server_connection {
  asio::ip::tcp::socket socket;
  asio::streambuf buf;

  test_server_connection(asio::io_context& ctx) : socket{ctx}, buf{} {}

  test_server_connection(asio::io_context& ctx, size_t max_buf)
    : socket{ctx}, buf{max_buf} {}
};

class test_server {
 public:
  test_server();

  void run();

 private:
  void start_accept();

  void handle_accept(std::list<test_server_connection>::iterator con_handle,
                     std::error_code const& err);

  void start_read(std::list<test_server_connection>::iterator& con);

  void handle_read(std::list<test_server_connection>::iterator con_handle,
                   std::error_code const& err, size_t bytes_transfered);

  asio::io_context _ctx;
  asio::ip::tcp::acceptor _acceptor;
  std::list<test_server_connection> _connections;
};

test_server::test_server() : _ctx(), _acceptor{_ctx}, _connections{} {}

void test_server::run() {
  asio::ip::tcp::endpoint ep =
    asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 4242);
  _acceptor.open(ep.protocol());
  _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  try {
    _acceptor.bind(ep);
  } catch (std::system_error const& se) {
    std::cout << "bind error" << se.what();
    return;
  }
  _acceptor.listen();
  start_accept();

  _ctx.run();
}

void test_server::start_accept() {
  std::list<test_server_connection>::iterator con_handle =
    _connections.emplace(_connections.begin(), _ctx);
  auto handler = std::bind(&test_server::handle_accept, this, con_handle,
                           std::placeholders::_1);
  _acceptor.async_accept(con_handle->socket, handler);
}

void test_server::handle_accept(
  std::list<test_server_connection>::iterator con_handle,
  std::error_code const& err) {
  if (!err) {
    std::cout << "Connection from: "
              << con_handle->socket.remote_endpoint().address().to_string()
              << "\n";
    start_read(con_handle);
  } else {
    std::cerr << "We had an error: " << err.message() << std::endl;
    _connections.erase(con_handle);
  }
  start_accept();
}

void test_server::start_read(std::list<test_server_connection>::iterator& con) {
  auto handler = std::bind(&test_server::handle_read, this, con,
                           std::placeholders::_1, std::placeholders::_2);
  asio::async_read_until(con->socket, con->buf, "\n",
                         handler);
}

void test_server::handle_read(std::list<test_server_connection>::iterator con_handle,
                              std::error_code const& err, size_t bytes_transfered) {
  if (bytes_transfered > 0) {
    std::error_code err;

    std::istream is(&con_handle->buf);
    std::string line;
    std::getline(is, line);
    if (line.find("PING") != std::string::npos)
      asio::write(con_handle->socket, asio::buffer(std::string("PONG")), asio::transfer_all(), err);
    std::cout << "Message Received: " << line << std::endl;
  }

  if (!err) {
    start_read(con_handle);
  } else {
    std::cerr << "We had an error: " << err.message() << std::endl;
    _connections.erase(con_handle);
  }
}

void server_launch() {
  test_server test;
  test.run();
}
