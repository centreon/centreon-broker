/*
** Copyright 2015 Centreon
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

#ifndef CCB_EXTCMD_COMMAND_SERVER_HH
#  define CCB_EXTCMD_COMMAND_SERVER_HH

#  include <asio.hpp>
#  include <memory>
#  include <string>
#  include "com/centreon/broker/extcmd/command_listener.hh"
#  include "com/centreon/broker/extcmd/command_parser.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace               processing {
  class                 thread;
}

namespace               extcmd {
  // Forward declarations.
  class                 command_listener;
  class                 server_socket;

  /**
   *  @class command_server command_server.hh "com/centreon/broker/extcmd/command_server.hh"
   *  @brief Command server.
   *
   *  Users whishing to send external commands should connect to a
   *  command_server which is a Unix socket server.
   */
  class                 command_server : public io::endpoint {
  public:
    enum                protocol {
                        plaintext,
                        json
    };
                        command_server(
                          protocol prot,
                          std::string const& socket_file,
                          std::shared_ptr<persistent_cache> cache);
                        ~command_server();
    std::shared_ptr<io::stream>
                        open();

  private:
                        command_server(command_server const& other);
    command_server&     operator=(command_server const& other);

    std::shared_ptr<command_listener>
                        _listener;
    std::shared_ptr<command_parser>
                        _parser;
    processing::thread* _listener_thread;
    protocol            _protocol;
    asio::io_context    _io_context;
    std::unique_ptr<asio::local::stream_protocol::socket>
                        _socket;
    std::string         _socket_file;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_SERVER_HH
