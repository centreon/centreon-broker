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

#ifndef IO_NET_UNIX_HH_
# define IO_NET_UNIX_HH_

# include "io/acceptor.hh"
# include "io/net/socket.hh"

namespace            io {
  namespace          net {
    /**
     *  @class unix_acceptor unix.hh "io/net/unix.hh"
     *  @brief Listen on a specified Unix domain socket for incoming
     *         clients.
     *
     *  This class is used to listen on a specified Unix domain socket
     *  on the local host. If one local client connects to this socket,
     *  the unix_acceptor can generate a new stream object corresponding
     *  to this specific client.
     *
     *  Usage is pretty simple. Just call the listen() method with the
     *  desired socket name as argument. Then, just call accept() to get
     *  the next available incoming client. Once you're over with the
     *  the acceptor, just call close() to shut it down. If you want to,
     *  start the cycle again with a potentially different socket name.
     *
     *  @see io::acceptor
     *  @see socket
     */
    class            unix_acceptor : public io::acceptor {
     private:
      int            _sockfd;

     public:
                     unix_acceptor();
                     unix_acceptor(unix_acceptor const& ua);
                     ~unix_acceptor();
      unix_acceptor& operator=(unix_acceptor const& ua);
      io::stream*    accept();
      void           close();
      void           listen(char const* sock_path);
    };

    /**
     *  @class unix_connector unix.hh "io/net/unix.hh"
     *  @brief Connect to an Unix domain socket.
     *
     *  Connect to a specific Unix domain socket.
     *
     *  @see socket
     */
    class             unix_connector : public socket {
     public:
                      unix_connector();
                      unix_connector(unix_connector const& uc);
                      ~unix_connector();
      unix_connector& operator=(unix_connector const& uc);
      void            connect(char const* sock_path);
    };
  }
}

#endif /* !IO_NET_UNIX_HH_ */
