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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/pool.hh"

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
 * @brief Default constructor. Don't use it, it is private. Instead, call the
 * instance() static method to get it.
 */
tcp_async::tcp_async()
    : _strand{pool::instance().io_context()},
      _timer(pool::instance().io_context()),
      _clear_available_con_running(false) {}

/**
 * @brief The destructor of tcp_async. It is called once the instance is
 * released.
 */
tcp_async::~tcp_async() noexcept {
  if (_clear_available_con_running) {
    std::promise<bool> p;
    std::future<bool> f(p.get_future());
    _clear_available_con_running = false;
    asio::post(_timer.get_executor(), [this, &p] {
      _timer.cancel();
      p.set_value(true);
    });
    f.get();
  }

  /* Before destroying the strand, we have to wait it is really empty. We post
   * a last action and wait it is over. */
  std::promise<bool> p;
  std::future<bool> f{p.get_future()};
  _strand.post([&p] { p.set_value(true); });
  f.get();
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
  auto end = std::chrono::system_clock::now() + std::chrono::seconds{timeout_s};
  do {
    std::promise<tcp_connection::pointer> p;
    std::future<tcp_connection::pointer> f{p.get_future()};
    _strand.post([&p, a = acceptor.get(), this] {
      auto found = _acceptor_available_con.find(a);
      if (found != _acceptor_available_con.end()) {
        tcp_connection::pointer retval = std::move(found->second.first);
        _acceptor_available_con.erase(found);
        p.set_value(retval);
      } else
        p.set_value(nullptr);
    });
    auto retval = f.get();
    if (retval)
      return retval;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  } while (std::chrono::system_clock::now() < end);
  return nullptr;
}

bool tcp_async::contains_available_acceptor_connections(
    asio::ip::tcp::acceptor* acceptor) const {
  std::promise<bool> p;
  std::future<bool> f{p.get_future()};
  _strand.post([&p, &acceptor, this] {
    p.set_value(_acceptor_available_con.find(acceptor) !=
                _acceptor_available_con.end());
  });

  return f.get();
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
 * @brief Cleaning function called when the timer expires to close unused
 * connections.
 *
 * @param ec The error code returned by ASIO if any.
 */
void tcp_async::_clear_available_con(asio::error_code ec) {
  if (ec)
    log_v2::core()->info("Available connections cleaning: {}", ec.message());
  else {
    log_v2::core()->debug("Available connections cleaning");
    std::time_t now = std::time(nullptr);

    _strand.post([now, this] {
      for (auto it = _acceptor_available_con.begin();
           it != _acceptor_available_con.end();) {
        if (now >= it->second.second + 10) {
          log_v2::tcp()->debug("Destroying connection to '{}'",
                               it->second.first->peer());
          it = _acceptor_available_con.erase(it);
        } else
          ++it;
      }
      if (!_acceptor_available_con.empty()) {
        _timer.expires_after(std::chrono::seconds(10));
        _timer.async_wait(std::bind(&tcp_async::_clear_available_con, this,
                                    std::placeholders::_1));
      } else
        _clear_available_con_running = false;
    });
  }
}

/**
 * @brief Starts the acceptor given in parameter. To accept the acceptor needs
 * the IO Context to be running. A timer is started/restarted so that in 10s
 * not used connections will be erased.
 *
 * @param acceptor The acceptor that you want it to accept.
 */
void tcp_async::start_acceptor(
    std::shared_ptr<asio::ip::tcp::acceptor> acceptor) {
  log_v2::tcp()->trace("Start acceptor");

  if (!_clear_available_con_running)
    _clear_available_con_running = true;
  log_v2::tcp()->info("Reschedule available connections cleaning in 10s");
  _timer.expires_after(std::chrono::seconds(10));
  _timer.async_wait(
      std::bind(&tcp_async::_clear_available_con, this, std::placeholders::_1));

  tcp_connection::pointer new_connection =
      std::make_shared<tcp_connection>(pool::io_context());

  log_v2::tcp()->debug("Waiting for a connection");
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
  std::error_code ec;
  acceptor->cancel(ec);
  if (ec)
    log_v2::tcp()->warn("Error while cancelling acceptor: {}", ec.message());
  acceptor->close(ec);
  if (ec)
    log_v2::tcp()->warn("Error while closing acceptor: {}", ec.message());
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
    asio::error_code ecc;
    new_connection->update_peer(ecc);
    if (ecc)
      log_v2::tcp()->error(
          "tcp acceptor handling connection: unable to get peer endpoint: {}",
          ecc.message());
    else {
      std::time_t now = std::time(nullptr);
      asio::ip::tcp::socket& sock = new_connection->socket();
      asio::socket_base::keep_alive option{true};
      sock.set_option(option);
      _strand.post([new_connection, now, acceptor, this] {
        _acceptor_available_con.insert(std::make_pair(
            acceptor.get(), std::make_pair(new_connection, now)));
      });
      start_acceptor(acceptor);
    }
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
    throw exceptions::msg() << err.message();
  } else {
    asio::socket_base::keep_alive option{true};
    sock.set_option(option);
    return conn;
  }
}
