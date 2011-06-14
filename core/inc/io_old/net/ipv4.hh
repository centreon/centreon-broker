/*
** Copyright 2009-2011 MERETHIS
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

#ifndef IO_NET_IPV4_HH_
# define IO_NET_IPV4_HH_

# include <stddef.h>
# include "io/acceptor.hh"
# include "io/net/socket.hh"

namespace            io {
  namespace          net {
    /**
     *  @class ipv4_acceptor ipv4.hh "io/net/ipv4.hh"
     *  @brief Listen on a specified port to wait for incoming clients.
     *
     *  This class is used to listen on a specified port of the local
     *  host. If one network client connects to this port, the
     *  ipv4_acceptor can generate a new stream object corresponding to
     *  this specific client.
     *
     *  Usage is pretty simple. Just call the listen() method with the
     *  desired port as argument. Then, just call accept() to get the
     *  next available incoming client. Once you're over with the the
     *  acceptor, just call close() to shut it down. If you want to,
     *  start the cycle again with a potentially different port.
     *
     *  @see io::acceptor
     *  @see socket
     */
    class            ipv4_acceptor : public io::acceptor {
     private:
      int            _sockfd;

     public:
                     ipv4_acceptor();
                     ipv4_acceptor(ipv4_acceptor const& ipv4a);
                     ~ipv4_acceptor();
      ipv4_acceptor& operator=(ipv4_acceptor const& ipv4a);
      io::stream*    accept();
      void           close();
      void           listen(unsigned short port,
                       char const* iface = NULL);
    };

    /**
     *  @class ipv4_connector ipv4.hh "io/net/ipv4.hh"
     *  @brief Connect to an host.
     *
     *  Connect to an arbitrary host on an TCP/IPv4 network.
     *
     *  @see socket
     */
    class             ipv4_connector : public socket {
     public:
                      ipv4_connector();
                      ipv4_connector(ipv4_connector const& ipv4c);
                      ~ipv4_connector();
      ipv4_connector& operator=(ipv4_connector const& ipv4c);
      void            connect(char const* host, unsigned short port);
    };
  }
}

#endif /* !IO_NET_IPV4_HH_ */
