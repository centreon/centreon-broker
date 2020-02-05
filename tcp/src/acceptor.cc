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

constexpr std::size_t max_pending_connection = 30;

/**
 *  Default constructor.
 */
acceptor::acceptor()
    : io::endpoint(true),
      _port(0),
      _binding{false},
      _read_timeout(-1),
      _write_timeout(-1) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  Add a child to this acceptor.
 *
 *  @param[in] child  Child name.
 */
void acceptor::add_child(std::string const& child) {
  std::lock_guard<std::mutex> lock(_childrenm);
  _children.push_back(child);
}

/**
 *  Set the port on which the acceptor will listen.
 *
 *  @param[in] port Port on which the acceptor will listen.
 */
void acceptor::listen_on(unsigned short port) {
  _port = port;
  _binding = false;

  _ep = asio::ip::tcp::endpoint(asio::ip::address_v4::any(), _port);
  // Step 3. Instantiating and opening an acceptor socket.
  _acceptor.reset(new asio::ip::tcp::acceptor(
      tcp_async::instance().get_io_ctx(), _ep.protocol()));
  asio::socket_base::reuse_address option(true);
  _acceptor->set_option(option);
}

/**
 *  Start connection acception.
 *
 */
std::shared_ptr<io::stream> acceptor::open() {
  // Listen on port.
  std::lock_guard<std::mutex> lock(_mutex);

  if (!_acceptor)
    listen_on(_port);

  logging::info(logging::high) << "try to bind";
  std::shared_ptr<asio::ip::tcp::socket> socket{
      new asio::ip::tcp::socket{tcp_async::instance().get_io_ctx()}};
  try {
    if (!_binding) {
      _acceptor->bind(_ep);
      _acceptor->listen(max_pending_connection);
      _binding = true;
    }
    if (!tcp_async::instance().wait_for_accept(*socket, *_acceptor,
                                               std::chrono::seconds{1})) {
      return std::shared_ptr<stream>();
    }
  } catch (std::system_error const& se) {
    throw exceptions::msg()
        << "TCP: error while waiting client on port: " << _port << " "
        << se.what();
  }
  tcp_async::instance().register_socket(*socket);

  // Accept client.
  std::shared_ptr<stream> incoming{new stream{socket, ""}};

  logging::info(logging::high) << "TCP: new client connected";
  incoming->set_parent(this);
  incoming->set_read_timeout(_read_timeout);
  incoming->set_write_timeout(_write_timeout);
  return incoming;
}

/**
 *  Remove child of this socket.
 *
 *  @param[in] child  Child to remove.
 */
void acceptor::remove_child(std::string const& child) {
  std::lock_guard<std::mutex> lock(_childrenm);
  for (std::list<std::string>::iterator it(_children.begin()),
       end(_children.end());
       it != end; ++it)
    if (*it == child) {
      _children.erase(it);
      break;
    }
}

/**
 *  @brief Set read timeout.
 *
 *  If child stream does not provide data frequently enough, it will
 *  time out after some configured seconds.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void acceptor::set_read_timeout(int secs) {
  _read_timeout = secs;
}

/**
 *  Set write timeout on data.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void acceptor::set_write_timeout(int secs) {
  _write_timeout = secs;
}

/**
 *  Get statistics about this TCP acceptor.
 *
 *  @param[out] tree Buffer in which statistics will be written.
 */
void acceptor::stats(json11::Json::object& tree) {
  std::lock_guard<std::mutex> children_lock(_childrenm);
  std::ostringstream oss;
  oss << _children.size() << ": ";
  for (std::list<std::string>::const_iterator it(_children.begin()),
       end(_children.end());
       it != end; ++it)
    if (it == _children.begin())
      oss << *it;
    else
      oss << ", " << *it;
  tree["peers"] = oss.str();
}
