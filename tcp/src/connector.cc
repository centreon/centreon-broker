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

#include "com/centreon/broker/tcp/connector.hh"
#include <memory>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/stream.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
connector::connector()
    : io::endpoint(false), _port(0), _read_timeout(-1), _write_timeout(-1) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Set connection parameters.
 *
 *  @param[in] host  Host to connect to.
 *  @param[in] port  Port to connect to.
 */
void connector::connect_to(std::string const& host, unsigned short port) {
  _host = host;
  _port = port;
}

/**
 *  Connect to the remote host.
 */
std::shared_ptr<io::stream> connector::open() {
  // Launch connection process.
  logging::info(logging::medium)
      << "TCP: connecting to " << _host << ":" << _port;
  std::string connection_name{_host + ":" + std::to_string(_port)};

  std::shared_ptr<asio::ip::tcp::socket> sock;
  try {
    sock.reset(new asio::ip::tcp::socket(tcp_async::instance().get_io_ctx()));
    asio::ip::tcp::resolver resolver{tcp_async::instance().get_io_ctx()};
    asio::ip::tcp::resolver::query query{_host, std::to_string(_port)};
    asio::ip::tcp::resolver::iterator it{resolver.resolve(query)};
    asio::ip::tcp::resolver::iterator end;

    std::error_code err{std::make_error_code(std::errc::host_unreachable)};

    // it can resolve to multiple addresses like ipv4 and ipv6
    // we need to try all to find the first available socket
    while (err && it != end) {
      sock->connect(*it, err);

      if (err)
        sock->close();

      ++it;
    }

    if (err) {
      broker::exceptions::msg e;
      e << "TCP: could not connect to remote server '" << _host << ":" << _port
        << "': " << err.message();
      throw e;
    }

    asio::socket_base::keep_alive option{true};
    sock->set_option(option);
  } catch (std::system_error const& se) {
    broker::exceptions::msg e;
    e << "TCP: could not resolve remote server '" << _host << ":" << _port
      << "': " << se.what();
    throw(e);
  }
  tcp_async::instance().register_socket(*sock);

  logging::info(logging::medium)
      << "TCP: successfully connected to " << connection_name;

  // Return stream.
  std::shared_ptr<stream> s(new stream(sock, connection_name));
  s->set_read_timeout(_read_timeout);
  s->set_write_timeout(_write_timeout);
  return (s);
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_read_timeout(int secs) {
  _read_timeout = secs;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_write_timeout(int secs) {
  _write_timeout = secs;
}