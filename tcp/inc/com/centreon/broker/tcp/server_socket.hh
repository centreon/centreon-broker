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

#ifndef CCB_TCP_SERVER_SOCKET_HH
#  define CCB_TCP_SERVER_SOCKET_HH

#  include <QTcpServer>
#  include <queue>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/tcp/stream.hh"

CCB_BEGIN()

namespace          tcp {
  /**
   *  @class server_socket server_socket.hh "com/centreon/broker/tcp/server_socket.hh"
   *  @brief Wrap Qt's QTcpServer.
   *
   *  Wrap Qt's TCP server class to match Centreon Broker requirements.
   *  Mostly this class is used to lazy-initialize QTcpSockets until
   *  they are finally used in the proper thread. Otherwise they would
   *  belong to the incoming connection accepting thread which is not
   *  supported.
   */
  class            server_socket : private QTcpServer {
  public:
                   server_socket(unsigned short port);
                   ~server_socket();
    void           close();
    QString        error_string() const;
    bool           has_pending_connections() const;
    misc::shared_ptr<stream>
                   next_pending_connection();
    bool           wait_for_new_connection(
                     int timeout = -1,
                     bool* timed_out = NULL);

  private:
                   server_socket(server_socket const& other);
    server_socket& operator=(server_socket const& other);
    void           incomingConnection(int socket_descriptor);

    std::queue<misc::shared_ptr<stream> >
                   _pending;
  };
}

CCB_END()

#endif // !CCB_TCP_SERVER_SOCKET_HH
