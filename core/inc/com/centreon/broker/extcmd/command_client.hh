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

#ifndef CCB_EXTCMD_COMMAND_CLIENT_HH
#  define CCB_EXTCMD_COMMAND_CLIENT_HH

#  include <memory>
#  include <QLocalSocket>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             extcmd {
  // Forward declaration.
  class               command_listener;

  /**
   *  @class command_client command_client.hh "com/centreon/broker/extcmd/command_client.hh"
   *  @brief Command client.
   *
   *  This class represents a user connected to the command server.
   */
  class               command_client : public io::stream {
  public:
                      command_client(
                        int native_socket,
                        command_listener* listener);
                      ~command_client();
    void              process(bool in, bool out);
    void              read(misc::shared_ptr<io::data>& d);
    unsigned int      write(misc::shared_ptr<io::data> const& d);

  private:
                      command_client(command_client const& other);
    command_client&   operator=(command_client const& other);
    void              _initialize_socket();

    std::string       _buffer;
    command_listener* _listener;
    std::auto_ptr<QLocalSocket>
                      _socket;
    int               _socket_native;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_CLIENT_HH
