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

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "exception.h"
#include "io/fd.h"
#include "io/net4.h"

using namespace CentreonBroker::IO;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Duplicate the internal file descriptor of the given object to the
 *         current instance.
 *
 *  This method is used by the copy constructor and operator= to duplicate the
 *  file descriptor of the given object. This will result in the current
 *  instance listening on the same port and with the same parameters as the
 *  given object. In case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] n4a Net4Acceptor to duplicate.
 *
 *  \see Net4Acceptor
 *  \see operator=
 */
void Net4Acceptor::InternalCopy(const Net4Acceptor& n4a)
  throw (CentreonBroker::Exception)
{
  this->sockfd_ = dup(n4a.sockfd_);
  if (this->sockfd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Net4Acceptor default constructor.
 *
 *  Build a Net4Acceptor.
 */
Net4Acceptor::Net4Acceptor() throw () : sockfd_(-1) {}

/**
 *  \brief Net4Acceptor copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have. In case of
 *  error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] n4a Net4Acceptor to duplicate.
 */
Net4Acceptor::Net4Acceptor(const Net4Acceptor& n4a)
  throw (CentreonBroker::Exception) : Acceptor(n4a), sockfd_(-1)
{
  this->InternalCopy(n4a);
}

/**
 *  \brief Net4Acceptor destructor.
 *
 *  Will close the socket if it has not already been done.
 */
Net4Acceptor::~Net4Acceptor() throw ()
{
  this->Close();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have. In case of
 *  error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] n4a Net4Acceptor to duplicate.
 *
 *  \return *this
 */
Net4Acceptor& Net4Acceptor::operator=(const Net4Acceptor& n4a)
  throw (CentreonBroker::Exception)
{
  this->Close();
  this->Acceptor::operator=(n4a);
  this->InternalCopy(n4a);
  return (*this);
}

/**
 *  \brief Wait for a new incoming client.
 *
 *  Once the acceptor is in a listening state, one can wait for incoming
 *  client by using this method. Once a client is properly connected, this
 *  method will return a Stream object (in fact a SocketStream object). In case
 *  of error, a CentreonBroker::Exception is thrown.
 *
 *  \return A stream object representing the new client connection.
 */
Stream* Net4Acceptor::Accept()
{
  int fd;

  fd = accept(this->sockfd_, NULL, NULL);
  if (fd < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return (new SocketStream(fd));
}

/**
 *  \brief Close the acceptor.
 *
 *  Shutdown the socket properly. No more client connection can be made through
 *  this acceptor.
 */
void Net4Acceptor::Close() throw ()
{
  if (this->sockfd_ >= 0)
    {
      shutdown(this->sockfd_, SHUT_RDWR);
      fsync(this->sockfd_);
      close(this->sockfd_);
      this->sockfd_ = -1;
    }
  return ;
}

/**
 *  \brief Put the acceptor in listen mode.
 *
 *  Before being able to Accept() new clients, the Net4Acceptor has to be put
 *  in the listen mode. Call this method with the desired port to do so. In
 *  case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] port  Port on which the acceptor should listen on.
 *  \param[in] iface IP address of the interface the acceptor should be to.
 *                   If NULL (default), bind to all available interfaces.
 */
void Net4Acceptor::Listen(unsigned short port, const char* iface)
  throw (CentreonBroker::Exception)
{
  struct sockaddr_in sin;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));

  // Set the binding structure
  memset(&sin, 0, sizeof(sin));
  if (iface)
    {
      sin.sin_addr.s_addr = inet_addr(iface);
      if ((in_addr_t)-1 == sin.sin_addr.s_addr)
	throw (CentreonBroker::Exception(errno, strerror(errno)));
    }
  else
    sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sin, sizeof(sin))
      || listen(this->sockfd_, 0))
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ;
}
