
/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef IO_NET_IPV6_H_
# define IO_NET_IPV6_H_

# include "io/acceptor.h"
# include "io/net/socket.h"

namespace            IO
{
  namespace          Net
  {
    /**
     *  \class IPv6Acceptor ipv6.h "io/net/ipv6.h"
     *  \brief Listen on a specified port to wait for incoming clients.
     *
     *  This class is used to listen on a specified port of the local host. If
     *  one network client connects to this port, the IPv6Acceptor can generate
     *  a new Stream object corresponding to this specific client.
     *
     *  Usage is pretty simple. Just call the Listen() method with the desired
     *  port as argument. Then, just call Accept() to get the next available
     *  incoming client. Once you're over with the the acceptor, just call
     *  Close() to shut it down. If you want to, start the cycle again with a
     *  potentially different port.
     *
     *  \see IO::Acceptor
     *  \see Socket
     */
    class            IPv6Acceptor : public IO::Acceptor
    {
     private:
      int            sockfd_;

     public:
                     IPv6Acceptor();
                     IPv6Acceptor(const IPv6Acceptor& ipv6a);
                     ~IPv6Acceptor();
      IPv6Acceptor&  operator=(const IPv6Acceptor& ipv6a);
      IO::Stream*    Accept();
      void           Close();
      void           Listen(unsigned short port, const char* iface = NULL);
    };

    /**
     *  \class IPv6Connector ipv6.h "io/net/ipv6.h"
     *  \brief Connect to an host.
     *
     *  Connect to an arbitrary host on an TCP/IPv6 network.
     *
     *  \see Socket
     */
    class            IPv6Connector : public Socket
    {
     public:
                     IPv6Connector();
                     IPv6Connector(const IPv6Connector& ipv6c);
                     ~IPv6Connector();
      IPv6Connector& operator=(const IPv6Connector& ipv6c);
      void           Connect(const char* host, unsigned short port);
    };
  }
}

#endif /* !IO_NET_IPV6_H_ */
