/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_TCP_ACCEPTOR_HH
#  define CCB_TCP_ACCEPTOR_HH

#  include <asio.hpp>
#  include <list>
#  include <memory>
#  include <mutex>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

#if ASIO_VERSION < 101200
namespace asio {
  typedef io_service io_context;
}
#endif

CCB_BEGIN()

namespace tcp {
/**
 *  @class acceptor acceptor.hh "com/centreon/broker/tcp/acceptor.hh"
 *  @brief TCP acceptor.
 *
 *  Accept TCP connections.
 */
class acceptor : public io::endpoint {
 public:
  acceptor();
  ~acceptor();
  void add_child(std::string const& child);
  void listen_on(unsigned short port);
  std::shared_ptr<io::stream>
  open();
  void remove_child(std::string const& child);
  void set_read_timeout(int secs);
  void set_write_timeout(int secs);
  void stats(io::properties& tree);

 private:
  acceptor(acceptor const& other);
  acceptor& operator=(acceptor const& other);

  std::list<std::string> _children;
  std::mutex _childrenm;
  std::mutex _mutex;
  unsigned short _port;
  int _read_timeout;
  std::unique_ptr<asio::ip::tcp::socket> _socket;
  asio::io_context _io_context;
  int _write_timeout;
};
}

CCB_END()

#endif // !CCB_TCP_ACCEPTOR_HH
