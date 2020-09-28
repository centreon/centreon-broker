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
#include <list>
#include <queue>
#include <thread>
#include <unordered_map>

#include "com/centreon/broker/tcp/tcp_connection.hh"

CCB_BEGIN()
namespace tcp {

class tcp_async {
  asio::io_context _io_context;
  asio::io_service::work _worker;
  std::vector<std::thread> _pool;
  std::mutex _m_read_data;
  std::mutex _closed_m;
  bool _closed;

  /* The acceptors open by this tcp_async */
  std::list<std::shared_ptr<asio::ip::tcp::acceptor>> _acceptor;

  /* Connections opened by acceptors not already got by streams */
  std::mutex _acceptor_con_m;
  std::condition_variable _acceptor_con_cv;
  std::unordered_multimap<asio::ip::tcp::acceptor*, tcp_connection::pointer>
      _acceptor_available_con;

  tcp_async();
  ~tcp_async();
  void _start();
  void _stop();

 public:
  std::shared_ptr<asio::ip::tcp::acceptor> create_acceptor(uint16_t port);
  void start_acceptor(std::shared_ptr<asio::ip::tcp::acceptor> acceptor);
  void stop_acceptor(std::shared_ptr<asio::ip::tcp::acceptor> acceptor);

  std::shared_ptr<tcp_connection> create_connection(std::string const& address,
                                                    uint16_t port);
  void remove_acceptor(std::shared_ptr<asio::ip::tcp::acceptor> acceptor);
  void handle_accept(std::shared_ptr<asio::ip::tcp::acceptor> acceptor,
                     tcp_connection::pointer new_connection,
                     const asio::error_code& error);
  tcp_connection::pointer get_connection(
      std::shared_ptr<asio::ip::tcp::acceptor> acceptor,
      uint32_t timeout_s);

  static tcp_async& instance();
};
}  // namespace tcp

CCB_END()
#endif  // CENTREON_BROKER_TCP_INC_COM_CENTREON_BROKER_TCP_TCP_ASYNC_HH_
