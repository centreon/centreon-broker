/*
** Copyright 2011-2013,2015,2017 Centreon
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

#ifndef CCB_TCP_STREAM_HH
#define CCB_TCP_STREAM_HH

#include <asio.hpp>
#include <memory>
#include <string>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/tcp/tcp_connection.hh"

CCB_BEGIN()

namespace tcp {
// Forward declaration.
class acceptor;

/**
 *  @class stream stream.hh "com/centreon/broker/tcp/stream.hh"
 *  @brief TCP stream.
 *
 *  TCP stream.
 */
class stream : public io::stream {
  static size_t _total_tcp_count;

  const std::string _host;
  const uint16_t _port;
  const int32_t _read_timeout;
  tcp_connection::pointer _connection;
  acceptor* _parent;

 public:
  stream(std::string const& host, uint16_t port, int32_t read_timeout);
  stream(tcp_connection::pointer conn, int32_t read_timeout);
  ~stream() noexcept;
  stream& operator=(const stream&) = delete;
  stream(const stream&) = delete;
  std::string peer() const override final;
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  void set_parent(acceptor* parent);
  int32_t flush() override;
  int32_t stop() override;
  int32_t write(std::shared_ptr<io::data> const& d) override;
};
}  // namespace tcp

CCB_END()

#endif  // !CCB_TCP_STREAM_HH
