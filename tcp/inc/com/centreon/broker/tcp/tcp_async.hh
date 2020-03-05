/*
** Copyright 2020 Centreon
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
#ifndef CENTREON_BROKER_TCP_INC_COM_CENTREON_BROKER_TCP_TCP_ASYNC_HH_
#define CENTREON_BROKER_TCP_INC_COM_CENTREON_BROKER_TCP_TCP_ASYNC_HH_

#include <asio.hpp>
#include <queue>
#include <thread>
#include <unordered_map>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()
namespace tcp {

typedef std::vector<char> async_buf;
typedef std::queue<async_buf> async_queue;

struct tcp_con {
  async_buf _work_buffer;
  async_queue _buffer_queue;
  std::unique_ptr<asio::steady_timer> _timer;

  bool _closing;
  bool _timeout;

  // waiting for data
  std::condition_variable _wait_socket_event;

  tcp_con() : _timer{nullptr}, _closing{false}, _timeout{false} {};
};

struct tcp_accept {
  // waiting for data
  std::condition_variable _wait_bind_event;
  std::unique_ptr<asio::steady_timer> _timer;
  std::system_error _ec;

  std::mutex _acc_lock;

  bool _timeout;
  bool _accept_ok;

  tcp_accept() : _timer{nullptr}, _timeout{false}, _accept_ok{false} {}
};

class tcp_async {
  asio::io_context _io_context;
  asio::io_context::strand _strand;
  std::thread _async_thread;
  std::mutex _m_read_data;
  std::atomic_bool _closed;

  std::unordered_map<int, tcp_con> _read_data;

  tcp_async();
  ~tcp_async();

  void _async_job();
  void _async_read_cb(asio::ip::tcp::socket& socket,
                      int fd,
                      std::error_code const& ec,
                      std::size_t bytes);
  void _async_timeout_cb(int fd, std::error_code const& ec);
  void _async_accept_cb(std::error_code const& err, std::shared_ptr<tcp_accept> acc_data);
  void _async_acc_timeout_cb(std::error_code const& ec, std::shared_ptr<tcp_accept> acc_data, asio::ip::tcp::acceptor &acc);

 public:
  void register_socket(asio::ip::tcp::socket& socket);
  void unregister_socket(asio::ip::tcp::socket& socket, bool sync);

  async_buf wait_for_packet(asio::ip::tcp::socket& socket,
                            time_t deadline,
                            bool& disconnected,
                            bool& timeout);

  bool wait_for_accept(asio::ip::tcp::socket& socket, asio::ip::tcp::acceptor& acc,
                       std::chrono::seconds timeout);

  asio::io_context& get_io_ctx();

  static tcp_async& instance();
};
}  // namespace tcp

CCB_END()
#endif  // CENTREON_BROKER_TCP_INC_COM_CENTREON_BROKER_TCP_TCP_ASYNC_HH_
