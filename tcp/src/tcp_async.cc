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

async_buf tcp_async::get_packet(asio::ip::tcp::socket& socket) {
  async_buf buf;
  {
    std::lock_guard<misc::shared_mutex> lock(_rwlock);
    auto it = _read_data.find(socket.native_handle());
    if (it != _read_data.end()) {
      buf = std::move(it->second.second.front());
      it->second.second.pop();
    } else {
      logging::error(logging::medium)
          << "async_buf::get_packet called with unexisting socket...";
    }
  }
  return buf;
}

void tcp_async::_async_read_cb(asio::ip::tcp::socket& socket,
                               int fd,
                               std::error_code const& ec,
                               std::size_t bytes) {
  std::lock_guard<misc::shared_mutex> lock(_rwlock);
  std::unordered_map<int, std::pair<async_buf, async_queue>>::iterator it;
  {
    it = _read_data.find(fd);
    if (it == _read_data.end())
      return;
  }

  if (!ec) {
    if (bytes != 0) {
      logging::error(logging::low)
          << "async_buf::async_read_cb incoming packet size: " << bytes;
      it->second.first.resize(bytes);
      it->second.second.push(std::move(it->second.first));

      it->second.first.resize(async_buf_size);
    }
    socket.async_read_some(
        asio::buffer(it->second.first, async_buf_size),
        std::bind(&tcp_async::_async_read_cb, this, std::ref(socket), fd,
                  std::placeholders::_1, std::placeholders::_2));
  } else {
    logging::info(logging::high)
        << "connection lost for: "
        << socket.remote_endpoint().address().to_string() << ":"
        << socket.remote_endpoint().port();

    async_queue empty;
    it->second.second.swap(empty);
    _read_data.erase(fd);
  }
}

void tcp_async::register_socket(asio::ip::tcp::socket& socket) {
  int fd{socket.native_handle()};
  {
    std::lock_guard<misc::shared_mutex> lock(_rwlock);
    auto& data = _read_data[fd];

    data.first.resize(async_buf_size);

    socket.async_read_some(
        asio::buffer(data.first, async_buf_size),
        std::bind(&tcp_async::_async_read_cb, this, std::ref(socket),
                  socket.native_handle(), std::placeholders::_1,
                  std::placeholders::_2));
  }
}

void tcp_async::unregister_socket(
    std::shared_ptr<asio::ip::tcp::socket> socket) {
  ;
  async_queue empty;
  {
    std::lock_guard<misc::shared_mutex> lock(_rwlock);
    auto it = _read_data.find(socket->native_handle());
    if (it != _read_data.end()) {
      it->second.second.swap(empty);
      _read_data.erase(it);
    }
  }

  _strand.post(std::move([socket] {
    socket->shutdown(asio::ip::tcp::socket::shutdown_both);
    socket->close();
  }));
}

bool tcp_async::empty(asio::ip::tcp::socket& socket) {
  misc::read_lock lock(_rwlock);
  auto it = _read_data.find(socket.native_handle());

  if (it == _read_data.end())
    return true;

  return it->second.second.empty();
}

bool tcp_async::disconnected(asio::ip::tcp::socket& socket) {
  misc::read_lock lock(_rwlock);
  return (_read_data.find(socket.native_handle()) == _read_data.end());
}

asio::io_context& tcp_async::get_io_ctx() {
  return _io_context;
}

void tcp_async::_async_job() {
  //Work is needed because we run io_context in a separated thread
  //cf : https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/reference/io_service__work.html
  asio::io_service::work work(_io_context);

  while (!_closed) {
    try {
      _io_context.run_one();
    } catch (...) {
    }
  }
}

tcp_async::tcp_async() : _closed{false}, _strand{_io_context} {
  _async_thread = std::thread(&tcp_async::_async_job, this);
}

tcp_async::~tcp_async() {
  _closed = true;
  _io_context.stop();
  _async_thread.join();
}
