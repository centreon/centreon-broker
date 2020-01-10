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
#include "com/centreon/broker/misc/shared_mutex.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()
namespace tcp {

typedef std::vector<char> async_buf;
typedef std::queue<async_buf> async_queue;

class tcp_async {
  asio::io_context _io_context;
  asio::io_context::strand _strand;
  std::thread _async_thread;
  mutable misc::shared_mutex _rwlock;
  std::atomic_bool _closed;

  std::unordered_map<int, std::pair<async_buf, async_queue>> _read_data;



  tcp_async();
  ~tcp_async();
  void _async_job();
  void _async_read_cb(asio::ip::tcp::socket &socket,
                      int fd,
                      std::error_code const& ec,
                      std::size_t bytes);

 public:
  void register_socket(asio::ip::tcp::socket &socket);

  //shared_ptr is needed to keep a ref on the socket if it was destroy
  //by the stream, because we need to async strand it.
  void unregister_socket(std::shared_ptr<asio::ip::tcp::socket> socket);

  bool disconnected(asio::ip::tcp::socket &socket);
  bool empty(asio::ip::tcp::socket &socket);
  async_buf get_packet(asio::ip::tcp::socket &socket);

  asio::io_context& get_io_ctx();

  static tcp_async& instance();
};
}

CCB_END()
#endif  // CENTREON_BROKER_TCP_INC_COM_CENTREON_BROKER_TCP_TCP_ASYNC_HH_
