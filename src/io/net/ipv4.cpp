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

#include <arpa/inet.h>  // for inet_pton
#include <errno.h>
#include <netdb.h>      // for freeaddrinfo, getaddrinfo
#include <netinet/in.h> // for sockaddr_in
#include <string.h>     // for memset
#include <sys/socket.h> // for shutdown, socket
#include <unistd.h>     // for close, dup, fsync
#include "exception.h"
#include "io/net/ipv4.h"

using namespace IO::Net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                IPv4Acceptor                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  IPv4Acceptor default constructor.
 */
IPv4Acceptor::IPv4Acceptor() : sockfd_(-1) {}

/**
 *  \brief IPv4Acceptor copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have.
 *
 *  \param[in] ipv4a IPv4Acceptor to duplicate.
 *
 *  \see operator=(const IPv4Acceptor&)
 */
IPv4Acceptor::IPv4Acceptor(const IPv4Acceptor& ipv4a)
  : Acceptor(ipv4a), sockfd_(-1)
{
  this->operator=(ipv4a);
}

/**
 *  \brief IPv4Acceptor destructor.
 *
 *  Will close the socket if it has not already been done.
 */
IPv4Acceptor::~IPv4Acceptor()
{
  this->Close();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] ipv4a IPv4Acceptor to duplicate.
 *
 *  \return *this
 *
 *  \throw Exception Either ipv4a is invalid or the duplication failed.
 */
IPv4Acceptor& IPv4Acceptor::operator=(const IPv4Acceptor& ipv4a)
{
  int fd;

  fd = dup(ipv4a.sockfd_);
  if (fd < 0)
    throw (Exception(errno, strerror(errno)));
  this->Close();
  this->sockfd_ = fd;
  this->Acceptor::operator=(ipv4a);
  return (*this);
}

/**
 *  \brief Wait for a new incoming client.
 *
 *  Once the acceptor is in a listening state, one can wait for incoming
 *  client by using this method. Once a client is properly connected, this
 *  method will return a Stream object (in fact a Socket object).
 *  \par Safety Basic exception safety.
 *
 *  \return A stream object representing the new client connection.
 *
 *  \throw Exception      Error occured while accepting a new incoming
 *                        connection.
 *  \throw std::exception Error occured while creating the new Socket object.
 *
 *  \see IO::Stream
 */
IO::Stream* IPv4Acceptor::Accept()
{
  int fd;
  Socket* sock;

  fd = accept(this->sockfd_, NULL, NULL);
  if (fd < 0)
    throw (Exception(errno, strerror(errno)));
  try
    {
      sock = new Socket(fd);
    }
  catch (...)
    {
      close(fd);
      throw ;
    }
  return (sock);
}

/**
 *  \brief Close the acceptor.
 *
 *  Shutdown the socket properly. No more client connection can be made through
 *  this acceptor.
 *  \par Safety No throw guarantee.
 */
void IPv4Acceptor::Close()
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
 *  Before being able to Accept() new clients, the IPv4Acceptor has to be put
 *  in the listen mode. Call this method with the desired port to do so. If an
 *  error occur, the object will be in a closed state.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] port  Port on which the acceptor should listen on.
 *  \param[in] iface IP address of the interface the acceptor should be to.
 *                   If NULL (default), bind to all available interfaces.
 *
 *  \throw Exception Either socket creation failed or interface address is
 *                   invalid or socket binding failed.
 */
void IPv4Acceptor::Listen(unsigned short port, const char* iface)
{
  sockaddr_in sin;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (Exception(errno, strerror(errno)));

  // Set the binding structure
  memset(&sin, 0, sizeof(sin));
  if (iface)
    {
      if (inet_pton(AF_INET, iface, &sin.sin_addr.s_addr) != 1)
        throw (Exception(errno, strerror(errno)));
    }
  else
    sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sin, sizeof(sin))
      || listen(this->sockfd_, 0))
    throw (Exception(errno, strerror(errno)));

  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                IPv4Connector                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  IPv4Connector default constructor.
 *
 *  \throw Exception Socket creation failed.
 */
IPv4Connector::IPv4Connector() : Socket(-1)
{
  this->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->fd < 0)
    throw (Exception(errno, strerror(errno)));
}

/**
 *  IPv4Connector copy constructor.
 *
 *  \param[in] ipv4c Object to copy from.
 *
 *  \see Socket::Socket(const Socket&)
 */
IPv4Connector::IPv4Connector(const IPv4Connector& ipv4c) : Socket(ipv4c) {}

/**
 *  IPv4Connector destructor.
 */
IPv4Connector::~IPv4Connector() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] ipv4c Object to copy from.
 *
 *  \return *this
 *
 *  \see Socket::operator=(const Socket&)
 */
IPv4Connector& IPv4Connector::operator=(const IPv4Connector& ipv4c)
{
  this->Socket::operator=(ipv4c);
  return (*this);
}

/**
 *  \brief Connect to an host.
 *
 *  Connect to the specified host on the specified port.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] host Hostname or IP address to connect to.
 *  \param[in] port Port on which connection will be made.
 *
 *  \throw Exception Either host is not a valid host nor IP address or
 *                   connection to host failed.
 */
void IPv4Connector::Connect(const char* host, unsigned short port)
{
  sockaddr_in sin;

  // If host is not NULL.
  if (host)
    {
      memset(&sin, 0, sizeof(sin));

      // Address family (IPv4).
      sin.sin_family = AF_INET;

      // Check if host is an IP address first, with the hope that we will avoid
      // a DNS lookup.
      if (inet_pton(AF_INET, host, &sin.sin_addr.s_addr) != 1)
        {
          // Not an IP address, check if it's an host name.
          addrinfo* addr_info;
          addrinfo hint;

          addr_info = NULL;
          memset(&hint, 0, sizeof(hint));
          hint.ai_family = AF_INET;
          if (getaddrinfo(host, NULL, &hint, &addr_info))
            throw (Exception(0, "Invalid hostname provided."));
          sin.sin_addr.s_addr =
            ((sockaddr_in*)addr_info->ai_addr)->sin_addr.s_addr;
          freeaddrinfo(addr_info);
        }

      // Set port.
      sin.sin_port = htons(port);

      // Connect !
      if (connect(this->fd, (sockaddr*)&sin, sizeof(sin)))
        throw (Exception(errno, strerror(errno)));
    }
  else
    throw (Exception(0, "NULL hostname provided."));

  return ;
}
