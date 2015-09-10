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
