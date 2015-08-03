/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_EXTCMD_COMMAND_SERVER_HH
#  define CCB_EXTCMD_COMMAND_SERVER_HH

#  include <memory>
#  include <string>
#  include "com/centreon/broker/extcmd/command_listener.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

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
                        command_server(std::string const& socket_file);
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

    misc::shared_ptr<command_listener>
                        _listener;
    std::auto_ptr<server_socket>
                        _socket;
    std::string         _socket_file;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_SERVER_HH
