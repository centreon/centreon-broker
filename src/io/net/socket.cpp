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

#include <sys/socket.h>
#include "io/net/socket.h"

using namespace IO::Net;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Socket constructor.
 *
 *  Build a Socket by providing an already opened socket file descriptor. The
 *  type of the socket (AF_INET, AF_UNIX, ...) doesn't matter. Once the
 *  constructor has been successfully executed, the Socket object is
 *  responsible of the file descriptor (ie. it will handle all I/O operations
 *  as well as closing).
 *
 *  \param[in] sockfd Open socket.
 */
Socket::Socket(int sockfd) : FD(sockfd) {}

/**
 *  \brief Socket copy constructor.
 *
 *  Duplicate the Socket object given as a parameter. Refer to the IO::FD copy
 *  constructor for more information.
 *
 *  \param[in] sock Socket to duplicate.
 *
 *  \see IO::FD::FD(const IO::FD&)
 */
Socket::Socket(const Socket& sock) : FD(sock) {}

/**
 *  \brief Socket destructor.
 *
 *  The destructor will shutdown the connection and close the socket file
 *  descriptor if it has not already been closed.
 */
Socket::~Socket()
{
  this->Close();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Shutdown the current socket properly and close it if open. Then duplicate
 *  the Socket argument. Refer to the IO::FD assignment operator overload for
 *  more information.
 *
 *  \param[in] sock Socket to duplicate.
 *
 *  \return *this
 *
 *  \see IO::FD::operator=(const IO::FD&)
 */
Socket& Socket::operator=(const Socket& sock)
{
  this->Close();
  this->IO::FD::operator=(sock);
  return (*this);
}

/**
 *  Shutdown the connection and close the file descriptor.
 */
void Socket::Close()
{
  if (this->fd >= 0)
    {
      shutdown(this->fd, SHUT_RDWR);
      this->IO::FD::Close();
    }
  return ;
}
