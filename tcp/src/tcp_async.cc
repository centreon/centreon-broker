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
#include "com/centreon/broker/tcp/tcp_async.hh"

#include <functional>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**
 * @brief Return the tcp_async singleton.
 *
 * @return A tcp_async singleton.
 */
tcp_async& tcp_async::instance() {
  static tcp_async instance;
  return instance;
}

/**
 * @brief If the acceptor given in parameter has established a connection.
 * This method returns it. Otherwise, it returns an empty connection.
 *
 * @param acceptor The acceptor we want a connection to.
 *
 * @return A shared_ptr to a connection or nothing.
 */
tcp_connection::pointer tcp_async::get_connection(
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor,
    uint32_t timeout_s) {
  std::unique_lock<std::mutex> lck(_acceptor_con_m);
  if (_acceptor_con_cv.wait_for(lck, std::chrono::seconds(timeout_s),
                                [this, a = acceptor.get()] {
                                  return _acceptor_available_con.find(a) !=
                                             _acceptor_available_con.end() ||
                                         !a->is_open();
                                })) {
    auto found = _acceptor_available_con.find(acceptor.get());
    if (found != _acceptor_available_con.end()) {
      tcp_connection::pointer retval = found->second;
      _acceptor_available_con.erase(found);
      return retval;
    }
  }
  return nullptr;
}

bool tcp_async::contains_available_acceptor_connections(
    asio::ip::tcp::acceptor* acceptor) const {
  std::lock_guard<std::mutex> lck(_acceptor_con_m);
  return _acceptor_available_con.find(acceptor) !=
         _acceptor_available_con.end();
}
/**
 * @brief Create an ASIO acceptor listening on the given port. Once it is
 * operational, it begins to accept connections.
 *
 * @param port The port to listen on.
 *
 * @return The created acceptor as a shared_ptr.
 */
std::shared_ptr<asio::ip::tcp::acceptor> tcp_async::create_acceptor(
    uint16_t port) {
  auto retval(std::make_shared<asio::ip::tcp::acceptor>(
      pool::io_context(), asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)));

  asio::ip::tcp::acceptor::reuse_address option(true);
  retval->set_option(option);
  return retval;
}

/**
 * @brief Starts the acceptor given in parameter. To accept the acceptor needs
 * the IO Context to be running.
 *
 * @param acceptor The acceptor that you want it to accept.
 */
void tcp_async::start_acceptor(
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor) {
  tcp_connection::pointer new_connection =
      std::make_shared<tcp_connection>(pool::io_context());

  acceptor->async_accept(new_connection->socket(),
                         std::bind(&tcp_async::handle_accept, this, acceptor,
                                   new_connection, std::placeholders::_1));
}

/**
 * @brief Stop the acceptor.
 *
 * @param acceptor The acceptor to stop.
 */
void tcp_async::stop_acceptor(
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor) {
  std::lock_guard<std::mutex> lck(_acceptor_con_m);

  std::error_code ec;
  acceptor->cancel(ec);
  if (ec)
    log_v2::tcp()->warn("Error while cancelling acceptor: {}", ec.message());
  acceptor->close(ec);
  if (ec)
    log_v2::tcp()->warn("Error while closing acceptor: {}", ec.message());
  _acceptor_con_cv.notify_all();
}

/**
 * @brief The handler called after an async_accept.
 *
 * @param acceptor The acceptor accepting a connection.
 * @param new_connection The established connection.
 * @param ec An error code if any.
 */
void tcp_async::handle_accept(std::shared_ptr<asio::ip::tcp::acceptor> acceptor,
                              tcp_connection::pointer new_connection,
                              const asio::error_code& ec) {
  /* If we got a connection, we store it */
  if (!ec) {
    new_connection->update_peer();
    std::lock_guard<std::mutex> lck(_acceptor_con_m);
    _acceptor_available_con.insert(
        std::make_pair(acceptor.get(), new_connection));
    _acceptor_con_cv.notify_one();
    start_acceptor(acceptor);
  } else
    log_v2::tcp()->error("acceptor error: {}", ec.message());
}

/**
 * @brief Creates a connection to the given host on the given port.
 *
 * @param host The host to connect to.
 * @param port The port to use for the connection.
 *
 * @return A shared_ptr to the connection or an empty shared_ptr.
 */
tcp_connection::pointer tcp_async::create_connection(std::string const& host,
                                                     uint16_t port) {
  log_v2::tcp()->trace("create connection to host {}:{}", host, port);
  tcp_connection::pointer conn =
      std::make_shared<tcp_connection>(pool::io_context(), host, port);
  asio::ip::tcp::socket& sock = conn->socket();

  asio::ip::tcp::resolver resolver(pool::io_context());
  asio::ip::tcp::resolver::query query(host, std::to_string(port));
  asio::ip::tcp::resolver::iterator it = resolver.resolve(query), end;

  std::error_code err{std::make_error_code(std::errc::host_unreachable)};

  // it can resolve multiple addresses like ipv4 or ipv6
  // We need to try all to find the first available socket
  for (; err && it != end; ++it) {
    sock.connect(*it, err);

    if (err)
      sock.close();
  }

  /* Connection refused */
  if (err.value() == 111) {
    log_v2::tcp()->error("TCP: Connection refused to {}:{}", host, port);
    throw std::system_error(err);
  } else if (err) {
    log_v2::tcp()->error("TCP: could not connect to {}:{}", host, port);
    throw msg_fmt(err.message());
  } else {
    asio::socket_base::keep_alive option{true};
    sock.set_option(option);
    return conn;
  }
}
