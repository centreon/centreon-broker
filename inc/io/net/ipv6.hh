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

#ifndef IO_NET_IPV6_HH_
# define IO_NET_IPV6_HH_

# include "io/acceptor.hh"
# include "io/net/socket.hh"

namespace            io {
  namespace          net {
    /**
     *  @class ipv6_acceptor ipv6.hh "io/net/ipv6.hh"
     *  @brief Listen on a specified port to wait for incoming clients.
     *
     *  This class is used to listen on a specified port of the local
     *  host. If one network client connects to this port, the
     *  ipv6_acceptor can generate a new stream object corresponding to
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
    class            ipv6_acceptor : public io::acceptor {
     private:
      int            _sockfd;

     public:
                     ipv6_acceptor();
                     ipv6_acceptor(ipv6_acceptor const& ipv6a);
                     ~ipv6_acceptor();
      ipv6_acceptor& operator=(ipv6_acceptor const& ipv6a);
      io::stream*    accept();
      void           close();
      void           listen(unsigned short port,
                       char const* iface = NULL);
    };

    /**
     *  @class ipv6_connector ipv6.hh "io/net/ipv6.hh"
     *  @brief Connect to an host.
     *
     *  Connect to an arbitrary host on an TCP/IPv6 network.
     *
     *  @see socket
     */
    class             ipv6_connector : public socket
    {
     public:
                      ipv6_connector();
                      ipv6_connector(ipv6_connector const& ipv6c);
                      ~ipv6_connector();
      ipv6_connector& operator=(ipv6_connector const& ipv6c);
      void            connect(char const* host, unsigned short port);
    };
  }
}

#endif /* !IO_NET_IPV6_HH_ */
