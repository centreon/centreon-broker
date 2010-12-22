/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef IO_NET_SOCKET_HH_
# define IO_NET_SOCKET_HH_

# include "io/fd.hh"

namespace     io {
  namespace   net
  {
    /**
     *  @class socket socket.hh "io/net/socket.hh"
     *  @brief Socket file descriptor wrapper.
     *
     *  Wraps a socket file descriptor. It subclasses io::fd but
     *  provides more socket-specific calls.
     *
     *  @see io::fd
     */
    class     socket : public fd {
     public:
              socket(int sockfd);
              socket(socket const& sock);
      virtual ~socket();
      socket& operator=(socket const& sock);
      void    close();
    };
  }
}

#endif /* !IO_NET_SOCKET_HH_ */
