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
#include "com/centreon/broker/tcp/tcp_connection.hh"

#include <functional>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker::tcp;

/**
 * @brief tcp_connection constructor.
 *
 * @param io_context The io_context needed to use the socket.
 * @param host The peer host the socket is connected to. If empty, the
 *        connection is on an acceptor side and no connection has been
 *        established yet.
 * @param port The port on the peer side. If 0, the connection is on an acceptor
 *        side and no connection has been established yet.
 */
tcp_connection::tcp_connection(asio::io_context& io_context,
                               const std::string& host,
                               uint16_t port)
    : _socket(io_context),
      _strand(io_context),
      _write_queue_has_events(false),
      _writing(false),
      _acks(0),
      _reading(false),
      _closing(false),
      _closed(false),
      _peer(fmt::format("{}:{}", host, port)) {}

/**
 * @brief Destructor
 */
tcp_connection::~tcp_connection() noexcept {
  log_v2::tcp()->trace("Connection to {} destroyed.", _peer);
  close();
}

/**
 * @brief A tcp_connection manages its own shared_ptr. This method is common to
 * get a such one.
 *
 * @return A shared_ptr to this tcp_connection.
 */
tcp_connection::pointer tcp_connection::ptr() {
  return shared_from_this();
}

/**
 * @brief Accessor to the socket of this tcp_connection.
 *
 * @return An asio::ip::tcp::socket.
 */
asio::ip::tcp::socket& tcp_connection::socket() {
  return _socket;
}

/**
 * @brief This function should not be used. Its only interest if for tests. We
 * wait for the writing() internal function to be finished.
 *
 * @return 0.
 */
int32_t tcp_connection::flush() {
  int32_t retval = _acks;
  if (_acks) {
    /* Do not set it to zero directly, maybe it has already been incremented by
     * another operation */
    _acks -= retval;
    return retval;
  }
  {
    std::lock_guard<std::mutex> lck(_error_m);
    if (_current_error) {
      std::string msg{std::move(_current_error.message())};
      _current_error.clear();
      throw exceptions::msg() << msg;
    }
  }
  if (_write_queue_has_events && !_writing) {
    _writing = true;
    // The strand is useful because of the flush() method.
    _strand.context().post(std::bind(&tcp_connection::writing, ptr()));
  }
  return retval;
}

/**
 * @brief This function is an internal function just used to debug. It displays
 * the data array as hex 8 bits integers in the limit of 20 values. If the array
 * is longer, only the 10 first bytes and the 10 last bytes are displayed.
 *
 * @param data A const char* array.
 * @param size The size of the data array.
 *
 * @return A string containing the result.
 */
static std::string debug_buf(const char* data, int32_t size) {
  auto to_str = [](uint8_t d) -> uint8_t {
    uint8_t ret;
    if (d < 10)
      ret = '0' + d;
    else
      ret = 'a' + d - 10;
    return ret;
  };

  std::string retval;
  int l1;
  if (size <= 10)
    l1 = size;
  else
    l1 = 10;

  for (int i = 0; i < l1; i++) {
    uint8_t c = data[i];
    uint8_t d1 = c >> 4;
    uint8_t d2 = c & 0xf;
    retval.push_back(to_str(d1));
    retval.push_back(to_str(d2));
  }
  if (size > 10) {
    if (size > 20)
      retval += "...";
    for (int i = std::max(size - 10, l1); i < size; i++) {
      uint8_t c = data[i];
      uint8_t d1 = c >> 4;
      uint8_t d2 = c & 0xf;
      retval.push_back(to_str(d1));
      retval.push_back(to_str(d2));
    }
  }
  return retval;
}

/**
 * @brief Write data on the socket. This function returns immediatly and does
 * not wait the data to be written. Its real work is just to stack the given
 * vector on a queue and check the asio::async_write function is running. Then
 * while the queue is not empty, async_write will be called. Each time a data
 * is written and so removed from the queue, the _ack counter is incremented.
 * The return value is the current value of the _ack counter, which is also
 * updated.
 *
 * @param v A vector of char.
 *
 * @return The ack counter, the number of events to acknowledge on the broker
 * side.
 */
int32_t tcp_connection::write(const std::vector<char>& v) {
  {
    std::lock_guard<std::mutex> lck(_error_m);
    if (_current_error) {
      std::string msg{std::move(_current_error.message())};
      _current_error.clear();
      throw exceptions::msg() << msg;
    }
  }

  {
    std::lock_guard<std::mutex> lck(_exposed_write_queue_m);
    _exposed_write_queue.push(v);
  }

  // If the queue is not empty and the writing work is not started, we start
  // it.
  if (!_writing) {
    _writing = true;
    // The strand is useful because of the flush() method.
    _strand.context().post(std::bind(&tcp_connection::writing, ptr()));
  }

  int32_t retval = _acks;
  /* Do not set it to zero directly, maybe it has already been incremented by
   * another operation */
  _acks -= retval;

  return retval;
}

/**
 * @brief Execute the real writing on the socket. Infact, this function:
 *  * checks if the _write_queue is empty, and then exchanges its content with
 *    the _exposed_write_queue. No mutex is needed because if this function is
 *    executed from the internal function tcp_connection::write(), then we are
 *    not already writing. And otherwise, writing() is called from the
 *    tcp_connection::handle_write() function, cadenced by _strand.
 *  * Launches the async_write.
 */
void tcp_connection::writing() {
  if (!_write_queue_has_events) {
    std::lock_guard<std::mutex> lck(_exposed_write_queue_m);
    std::swap(_write_queue, _exposed_write_queue);
    _write_queue_has_events = !_write_queue.empty();
  }
  if (!_write_queue_has_events) {

    _writing = false;
    return;
  }

  asio::async_write(_socket, asio::buffer(_write_queue.front()),
                    _strand.wrap(std::bind(&tcp_connection::handle_write, ptr(),
                                           std::placeholders::_1)));
}

/**
 * @brief Here is the write handler of async_write(). While the queue contains
 * vectors to write, this handler continues to call async_write.
 *
 * @param ec
 */
void tcp_connection::handle_write(const asio::error_code& ec) {
  if (ec) {
    log_v2::tcp()->error("Error while writing on tcp socket: {}", ec.message());
    std::lock_guard<std::mutex> lck(_error_m);
    _current_error = ec;
    _writing = false;
  } else {
    ++_acks;
    _write_queue.pop();
    _write_queue_has_events = !_write_queue.empty();
    if (_write_queue_has_events) {
      // The strand is useful because of the flush() method.
      asio::async_write(_socket, asio::buffer(_write_queue.front()),
                        _strand.wrap(std::bind(&tcp_connection::handle_write,
                                               ptr(), std::placeholders::_1)));
    } else
      writing();
  }
}

/**
 * @brief Start to continuously read on the socket. It fills a queue, block by
 * block. Then the read function has just to check if this queue is non empty
 * and get packets.
 */
void tcp_connection::start_reading() {
  if (_closing || _closed)
    return;

  if (!_reading) {
    _reading = true;
  }
  _socket.async_read_some(
      asio::buffer(_read_buffer),
      _strand.wrap(std::bind(&tcp_connection::handle_read, ptr(),
                             std::placeholders::_1, std::placeholders::_2)));
}

void tcp_connection::handle_read(const asio::error_code& ec,
                                 size_t read_bytes) {
  log_v2::tcp()->trace("Incoming data: {} bytes: {}", read_bytes,
                       debug_buf(&_read_buffer[0], read_bytes));
  if (read_bytes > 0) {
    std::lock_guard<std::mutex> lock(_read_queue_m);
    _read_queue.emplace(_read_buffer.begin(),
                        _read_buffer.begin() + read_bytes);
    _read_queue_cv.notify_one();
  }
  if (ec) {
    log_v2::tcp()->error("Error while reading on socket: {}", ec.message());
    std::lock_guard<std::mutex> lck(_read_queue_m);
    _closing = true;
    _read_queue_cv.notify_one();
  } else
    start_reading();
}

/**
 * @brief Shutdown the socket.
 */
void tcp_connection::close() {
  if (!_closed) {
    _closed = true;
    std::error_code ec;
    _socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    log_v2::tcp()->trace("socket shutdown with message: {}", ec.message());
    _socket.close(ec);
    log_v2::tcp()->trace("socket closed with message: {}", ec.message());
  }
}

/**
 * @brief Reads data from the connection given as parameter. The time limit
 * set as timeout is the second parameter
 *
 * @param con The connection to read on to get data.
 * @param timeout_time The time set as timeout. If -1 is given, no timeout is
 * set.
 * @param timeout This boolean is set to true if the read function finished
 * with a timeout.
 */
std::vector<char> tcp_connection::read(time_t timeout_time, bool* timeout) {
  {
    std::lock_guard<std::mutex> lck(_error_m);
    if (_current_error) {
      std::string msg{std::move(_current_error.message())};
      _current_error.clear();
      throw exceptions::msg() << msg;
    }
  }

  if (!_reading)
    _strand.post(std::bind(&tcp_connection::start_reading, this));

  std::vector<char> retval;

  if (!_socket.is_open()) {
    if (_exposed_read_queue.empty()) {
      std::lock_guard<std::mutex> lck(_read_queue_m);
      std::swap(_exposed_read_queue, _read_queue);
    }

    log_v2::tcp()->warn(
        "Socket is closed. Trying to read the end of its buffer");
    if (!_exposed_read_queue.empty()) {
      retval = std::move(_exposed_read_queue.front());
      _exposed_read_queue.pop();
    }
  } else {
    if (_exposed_read_queue.empty()) {
      std::unique_lock<std::mutex> lck(_read_queue_m);
      std::swap(_exposed_read_queue, _read_queue);
      if (_exposed_read_queue.empty()) {
        /* No timeout on wait */
        if (timeout_time == static_cast<time_t>(-1)) {
          _read_queue_cv.wait(
              lck, [this] { return !_read_queue.empty() || _closing; });
          if (_read_queue.empty())
            throw exceptions::msg() << "Attempt to read data from peer "
                                    << _peer << " on a closing socket";
          /* Timeout on wait */
        } else {
          time_t now;
          time(&now);
          int delay = timeout_time - now;
          if (delay < 0)
            delay = 0;
          if (_read_queue_cv.wait_for(lck, std::chrono::seconds(delay), [this] {
                return !_read_queue.empty() || _closing;
              })) {
            if (_read_queue.empty())
              throw exceptions::msg() << "Attempt to read data from peer "
                                      << _peer << " on a closing socket";
          } else {
            log_v2::tcp()->trace("Timeout during read ; timeout time = {}",
                                 timeout_time);
            *timeout = true;
            return retval;
          }
        }
        std::swap(_exposed_read_queue, _read_queue);
      }
    }
    retval = std::move(_exposed_read_queue.front());
    _exposed_read_queue.pop();
  }
  *timeout = retval.empty();
  return retval;
}

/**
 * @brief Is this socket is closed?
 *
 * @return a boolean.
 */
bool tcp_connection::is_closed() const {
  return _closed;
}

const std::string& tcp_connection::peer() const {
  return _peer;
}

void tcp_connection::update_peer() {
  if (_socket.is_open()) {
    _peer =
        fmt::format("{}:{}", _socket.remote_endpoint().address().to_string(),
                    _socket.remote_endpoint().port());
  }
}
