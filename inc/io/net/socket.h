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

#ifndef IO_NET_SOCKET_H_
# define IO_NET_SOCKET_H_

# include "io/fd.h"

namespace     IO
{
  namespace   Net
  {
    /**
     *  \class Socket socket.h "io/net/socket.h"
     *  \brief Socket file descriptor wrapper.
     *
     *  Wraps a socket file descriptor. It subclasses IO::FD but provides more
     *  socket-specific calls.
     *
     *  \see IO::FD
     */
    class     Socket : public FD
    {
     public:
              Socket(int sockfd);
              Socket(const Socket& sock);
      virtual ~Socket();
      Socket& operator=(const Socket& sock);
      void    Close();
    };
  }
}

#endif /* !IO_NET_SOCKET_H_ */
