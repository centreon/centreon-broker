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

#  include <list>
#  include <memory>
#  include <string>
#  include "com/centreon/broker/extcmd/command_listener.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace               processing {
  class                 feeder;
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
                        command_server(
                          std::string const& socket_file,
                          QString const& name);
                        ~command_server();
    io::endpoint*       clone() const;
    void                close();
    misc::shared_ptr<io::stream>
                        open();
    misc::shared_ptr<io::stream>
                        open(QString const& id);

  private:
                        command_server(command_server const& other);
    command_server&     operator=(command_server const& other);

    std::list<processing::feeder*>
                        _clients;
    misc::shared_ptr<command_listener>
                        _listener;
    processing::feeder* _listener_thread;
    QString             _name;
    std::auto_ptr<server_socket>
                        _socket;
    std::string         _socket_file;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_SERVER_HH
