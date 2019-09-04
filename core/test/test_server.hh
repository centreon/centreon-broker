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
#ifndef CENTREON_BROKER_CORE_TEST_TEST_SERVER_HH_
#define CENTREON_BROKER_CORE_TEST_TEST_SERVER_HH_

#include <asio.hpp>
#include <atomic>
#include <list>
#include <memory>
#include <unordered_map>

struct test_server_connection {
  asio::ip::tcp::socket socket;
  char *buf;

  test_server_connection(asio::io_context& ctx, size_t max_buf) :
    socket{ctx} {
      buf = new char[max_buf];
  }

  ~test_server_connection() {
    delete[] buf;
  }
};

class test_server {
 public:
  test_server();

  void init();
  void run();
  void stop() { _ctx->stop(); };
  std::atomic_size_t const& get_num_connections() { return _num_connections; };
  std::atomic_bool const& get_init_done() { return _init_done; };
  std::atomic_bool const& get_bind_ok() { return _bind_ok; };

  bool add_client(asio::ip::tcp::socket & sock, asio::io_context& io);

 private:
  void start_accept();

  void handle_accept(std::list<test_server_connection>::iterator con_handle,
                     std::error_code const& err);

  void start_read(std::list<test_server_connection>::iterator& con);

  void handle_read(std::list<test_server_connection>::iterator con_handle,
                   std::error_code const& err, size_t bytes_transfered);

  std::unique_ptr<asio::io_context>_ctx;
  std::unique_ptr<asio::ip::tcp::acceptor> _acceptor;
  std::list<test_server_connection> _connections;
  std::atomic_size_t _num_connections;
  std::atomic_bool _init_done;
  std::atomic_bool _bind_ok;
  std::unordered_map<std::string, std::string> _answer_reply;
};

#endif  // CENTREON_BROKER_CORE_TEST_TEST_SERVER_HH_
