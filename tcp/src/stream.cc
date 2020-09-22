/*
 * Copyright 2011 - 2020 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/tcp/stream.hh"

#include <sys/socket.h>
#include <sys/time.h>

#include <algorithm>
#include <atomic>
#include <sstream>
#include <system_error>
#include <thread>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**
 * @brief Stream constructor used by a connector. The stream establishes a
 * connection to the host server listening on the given port.
 * read_timeout is a duration in seconds used to read on the socket.
 *
 * @param host The host to connect to.
 * @param port The port used by the host to listen.
 * @param read_timeout The read_timeout in seconds or -1 if no timeout.
 */
stream::stream(std::string const& host, uint16_t port, int32_t read_timeout)
    : io::stream("TCP"),
      _host(host),
      _port(port),
      _read_timeout(read_timeout),
      _connection(tcp_async::instance().create_connection(host, port)),
      _parent(nullptr) {}

/**
 * @brief Stream constructor for a server. The connection is already running.
 * We just specify the read_timeout which is a duration in seconds.
 *
 * @param conn The connection to use by this stream.
 * @param read_timeout A duration in seconds or -1 if no timeout.
 */
stream::stream(tcp_connection::pointer conn, int32_t read_timeout)
    : io::stream("TCP"),
      _host(conn->socket().remote_endpoint().address().to_string()),
      _port(conn->socket().remote_endpoint().port()),
      _read_timeout(read_timeout),
      _connection(conn),
      _parent(nullptr) {}

/**
 *  Destructor.
 */
stream::~stream() noexcept {
  log_v2::tcp()->trace("stream closed");
  if (_connection->socket().is_open())
    _connection->close();

  if (_parent)
    _parent->remove_child(peer());
}

/**
 *  Get peer name.
 *
 *  @return Peer name.
 */
std::string stream::peer() const {
  return fmt::format(
      "tcp://{}:{}",
      _connection->socket().remote_endpoint().address().to_string(),
      _connection->socket().remote_endpoint().port());
}

/**
 *  Read data with timeout.
 *
 *  @param[out] d         Received event if any.
 *  @param[in]  deadline  Timeout in seconds.
 *
 *  @return Respects io::stream::read()'s return value.
 */

bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  static uint32_t thread_id = 0;
  std::hash<std::thread::id> hasher;
  uint32_t current_thread_id = hasher(std::this_thread::get_id());
  if (thread_id == 0)
    thread_id = current_thread_id;
  if (thread_id != current_thread_id) {
    log_v2::tcp()->trace("thread {} != thread {} and stream = {}", thread_id,
                         current_thread_id, reinterpret_cast<uint64_t>(this));
  }

  log_v2::tcp()->trace("read on stream");

  // Set deadline.
  {
    time_t now = ::time(nullptr);
    if (_read_timeout != -1 &&
        (deadline == static_cast<time_t>(-1) || now + _read_timeout < deadline))
      deadline = now + _read_timeout / 1000;
  }

  if (_connection->is_closed()) {
    d.reset(new io::raw);
    throw exceptions::msg() << "Connection lost";
  }

  bool timeout = false;
  d.reset(new io::raw(_connection->read(deadline, &timeout)));
  std::shared_ptr<io::raw> data{std::static_pointer_cast<io::raw>(d)};
  log_v2::tcp()->debug("TCP Read done : {} bytes", data->get_buffer().size());
  return !timeout;
}

/**
 *  Set parent socket.
 *
 *  @param[in,out] parent  Parent socket.
 */
void stream::set_parent(acceptor* parent) {
  _parent = parent;
}

int32_t stream::flush() {
  return _connection->flush();
}

/**
 *  Write data to the socket.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::write(std::shared_ptr<io::data> const& d) {
  log_v2::tcp()->trace("write on stream");
  // Check that data exists and should be processed.
  assert(d);

  if (_connection->is_closed())
    throw exceptions::msg() << "Connection lost";

  if (d->type() == io::raw::static_type()) {
    std::shared_ptr<io::raw> r(std::static_pointer_cast<io::raw>(d));
    log_v2::tcp()->debug("TCP: write request of {} bytes to peer '{}:{}'",
                         r->size(), _host, _port);
    log_v2::tcp()->trace("write {} bytes", r->size());
    std::error_code err;
    try {
      return _connection->write(r->get_buffer());
    } catch (std::exception const& e) {
      log_v2::tcp()->error("Socket gone");
      throw;
    }
  }
  return 1;
}
