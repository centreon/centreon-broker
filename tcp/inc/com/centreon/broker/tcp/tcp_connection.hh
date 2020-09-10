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
#ifndef CENTREON_BROKER_TCP_CONNECTION_HH
#define CENTREON_BROKER_TCP_CONNECTION_HH
#include <asio.hpp>
#include <atomic>
#include <memory>
#include <queue>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tcp {

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
  constexpr static std::size_t async_buf_size = 16384;
  asio::ip::tcp::socket _socket;
  asio::io_context::strand _strand;

  std::mutex _data_m;
  asio::error_code _current_error;
  std::queue<std::vector<char>> _write_queue;
  std::atomic_bool _writing;

  std::atomic<int32_t> _acks;
  std::atomic_bool _reading;
  std::atomic_bool _closing;
  std::array<char, async_buf_size> _read_buffer;
  std::mutex _read_queue_m;
  std::condition_variable _read_queue_cv;
  std::queue<std::vector<char>> _read_queue;

  std::atomic_bool _closed;

  std::condition_variable _is_writing_cv;

 public:
  typedef std::shared_ptr<tcp_connection> pointer;
  tcp_connection(asio::io_context& io_context);
  ~tcp_connection() noexcept;

  pointer ptr();
  asio::ip::tcp::socket& socket();
  asio::io_context::strand& get_strand() { return _strand; }

  int32_t flush();

  void handle_write(const asio::error_code& ec);
  int32_t write(const std::vector<char>& v);

  void start_reading();
  void handle_read(const asio::error_code& ec, size_t read_bytes);
  std::vector<char> read(time_t timeout_time, bool* timeout);

  void close();

  bool is_closed() const;
};

}  // namespace tcp

CCB_END()

#endif /* !CENTREON_BROKER_TCP_CONNECTION_HH */
