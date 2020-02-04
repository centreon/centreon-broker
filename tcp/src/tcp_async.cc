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

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

constexpr std::size_t async_buf_size = 16384;

tcp_async& tcp_async::instance() {
  static tcp_async instance;
  return instance;
}

async_buf tcp_async::wait_for_packet(asio::ip::tcp::socket& socket,
                                     time_t deadline,
                                     bool& disconnected,
                                     bool& timeout) {
  timeout = false;
  disconnected = false;

  std::unique_lock<std::mutex> lock(_m_read_data);
  async_buf buf;
  {
    auto it = _read_data.find(socket.native_handle());
    if (it != _read_data.end()) {
      disconnected = it->second._closing;
      // No data present we need to wait for deadline...
      if (it->second._buffer_queue.empty() && !disconnected) {
        // deadline passed
        time_t t{time(nullptr)};
        if (deadline != -1 && t > deadline) {
          timeout = true;
          return buf;
        } else if (deadline != -1) {
          it->second._timer.reset(new asio::steady_timer{
              _io_context, std::chrono::seconds(deadline - t)});
          it->second._timer->async_wait(std::bind(&tcp_async::_async_timeout_cb,
                                                  this, socket.native_handle(),
                                                  std::placeholders::_1));
        }

        it->second._wait_socket_event.wait(lock, [&]() -> bool {
          if (it->second._closing) {
            disconnected = true;
          }

          if (it->second._timeout)
            timeout = true;

          if (!it->second._buffer_queue.empty() || disconnected || timeout) {
            if (it->second._timer) {
              it->second._timer->cancel();
              it->second._timer.reset(nullptr);
            }
            return true;
          }

          return false;
        });
      }

      if (!it->second._buffer_queue.empty()) {
        buf = std::move(it->second._buffer_queue.front());
        it->second._buffer_queue.pop();
      }
    } else {
      disconnected = true;
    }
  }
  return buf;
}

void tcp_async::_async_accept_cb(std::error_code const& ec,
                                 std::shared_ptr<tcp_accept> acc_data) {
  std::unique_lock<std::mutex> lock(acc_data->_acc_lock);
  acc_data->_timer->cancel();
  if (!ec)
    acc_data->_accept_ok = true;
  else
    acc_data->_ec = std::system_error(ec);
  acc_data->_wait_bind_event.notify_all();
}

void tcp_async::_async_acc_timeout_cb(std::error_code const& ec,
                                      std::shared_ptr<tcp_accept> acc_data,
                                      asio::ip::tcp::acceptor& acc) {
  std::unique_lock<std::mutex> lock(acc_data->_acc_lock);

  if (!ec) {
    acc.close();
    acc_data->_timeout = true;
  }
  acc_data->_wait_bind_event.notify_all();
}

bool tcp_async::wait_for_accept(asio::ip::tcp::socket& socket,
                                asio::ip::tcp::acceptor& acc,
                                std::chrono::seconds timeout) {
  std::shared_ptr<tcp_accept> acc_data{std::make_shared<tcp_accept>()};

  acc_data->_timer.reset(new asio::steady_timer{_io_context, timeout});
  acc_data->_timer->async_wait(std::bind(&tcp_async::_async_acc_timeout_cb,
                                         this, std::placeholders::_1,
                                         acc_data, std::ref(acc)));

  acc.async_accept(socket,
                   std::bind(&tcp_async::_async_accept_cb, this,
                             std::placeholders::_1, acc_data));

  std::unique_lock<std::mutex> m(acc_data->_acc_lock);
  acc_data->_wait_bind_event.wait(m, [&acc_data]() -> bool {
    if (acc_data->_accept_ok)
      return true;
    if (acc_data->_ec.code())
      return true;
    if (acc_data->_timeout)
      return true;
    return false;
  });

  if (acc_data->_accept_ok || socket.is_open())
    return true;

  if (acc_data->_ec.code()) {
    if (acc_data->_ec.code().value() == ECANCELED)
      return false;
    throw acc_data->_ec;
  }

  return false;
}

void tcp_async::_async_timeout_cb(int fd, std::error_code const& ec) {
  if (!ec) {
    std::unique_lock<std::mutex> lock(_m_read_data);
    std::unordered_map<int, tcp_con>::iterator it;
    it = _read_data.find(fd);
    if (it == _read_data.end())
      return;

    it->second._timeout = true;
    it->second._wait_socket_event.notify_all();
  }
}

void tcp_async::_async_read_cb(asio::ip::tcp::socket& socket,
                               int fd,
                               std::error_code const& ec,
                               std::size_t bytes) {
  std::unique_lock<std::mutex> lock(_m_read_data);
  std::unordered_map<int, tcp_con>::iterator it;
  {
    it = _read_data.find(fd);
    if (it == _read_data.end())
      return;
  }

  if (!ec) {
    if (bytes != 0) {
      logging::error(logging::low)
          << "async_buf::async_read_cb incoming packet size: " << bytes;
      it->second._work_buffer.resize(bytes);
      it->second._buffer_queue.push(std::move(it->second._work_buffer));
      it->second._wait_socket_event.notify_all();

      // refit buffer for next read
      it->second._work_buffer.resize(async_buf_size);
    }
    socket.async_read_some(
        asio::buffer(it->second._work_buffer, async_buf_size),
        std::bind(&tcp_async::_async_read_cb, this, std::ref(socket), fd,
                  std::placeholders::_1, std::placeholders::_2));
  } else {
    if (bytes != 0) {
      it->second._work_buffer.resize(bytes);
      it->second._buffer_queue.push(std::move(it->second._work_buffer));

      it->second._work_buffer.resize(0);
    }
    logging::info(logging::high)
        << "connection lost for: "
        << socket.remote_endpoint().address().to_string() << ":"
        << socket.remote_endpoint().port();

    it->second._closing = true;
    it->second._wait_socket_event.notify_all();
  }
}

void tcp_async::register_socket(asio::ip::tcp::socket& socket) {
  int fd{socket.native_handle()};
  {
    std::unique_lock<std::mutex> lock(_m_read_data);
    auto& data = _read_data[fd];

    data._work_buffer.resize(async_buf_size);

    socket.async_read_some(
        asio::buffer(data._work_buffer, async_buf_size),
        std::bind(&tcp_async::_async_read_cb, this, std::ref(socket),
                  socket.native_handle(), std::placeholders::_1,
                  std::placeholders::_2));
  }
}
void tcp_async::unregister_socket(asio::ip::tcp::socket& socket) {
  bool done{false};
  int fd{socket.native_handle()};
  std::condition_variable cond;
  std::mutex mut;

  _strand.post([&] {
    socket.shutdown(asio::ip::tcp::socket::shutdown_both);
    socket.close();
    std::unique_lock<std::mutex> m(mut);
    done = true;
    cond.notify_all();
    m.unlock();
  });

  std::unique_lock<std::mutex> m(mut);
  cond.wait(m, [&done]() -> bool { return done; });

  std::unique_lock<std::mutex> lock(_m_read_data);
  auto it = _read_data.find(fd);
  if (it != _read_data.end()) {
    _read_data.erase(it);
  }
}

asio::io_context& tcp_async::get_io_ctx() {
  return _io_context;
}

void tcp_async::_async_job() {
  // Work is needed because we run io_context in a separated thread
  // cf :
  // https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/reference/io_service__work.html
  asio::io_service::work work(_io_context);

  while (!_closed) {
    try {
      _io_context.run_one();
    } catch (...) {
    }
  }
}

tcp_async::tcp_async() : _strand{_io_context}, _closed{false} {
  _async_thread = std::thread(&tcp_async::_async_job, this);
}

tcp_async::~tcp_async() {
  _closed = true;
  _io_context.stop();
  _async_thread.join();
}
