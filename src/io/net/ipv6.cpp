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

#include <arpa/inet.h>  // for inet_addr and inet_pton
#include <errno.h>
#include <netdb.h>      // for freeaddrinfo, getaddrinfo
#include <netinet/in.h> // for sockaddr_in
#include <string.h>     // for memset
#include <sys/socket.h> // for shutdown, socket
#include <unistd.h>     // for close, fsync
#include "exception.h"
#include "io/net/ipv6.h"

using namespace IO::Net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                IPv6Acceptor                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  IPv6Acceptor default constructor.
 */
IPv6Acceptor::IPv6Acceptor() : sockfd_(-1) {}

/**
 *  \brief IPv6Acceptor copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have.
 *
 *  \param[in] ipv6a IPv6Acceptor to duplicate.
 *
 *  \see operator=(const IPv6&)
 */
IPv6Acceptor::IPv6Acceptor(const IPv6Acceptor& ipv6a)
  : Acceptor(ipv6a), sockfd_(-1)
{
  this->operator=(ipv6a);
}

/**
 *  \brief IPv6Acceptor destructor.
 *
 *  Will close the socket if it has not already been done.
 */
IPv6Acceptor::~IPv6Acceptor()
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
 *  \param[in] ipv6a IPv6Acceptor to duplicate.
 *
 *  \return *this
 *
 *  \throw Exception Either ipv6a is invalid or the duplication failed.
 */
IPv6Acceptor& IPv6Acceptor::operator=(const IPv6Acceptor& ipv6a)
{
  int fd;

  fd = dup(ipv6a.sockfd_);
  if (fd < 0)
    throw (Exception(errno, strerror(errno)));
  this->Close();
  this->sockfd_ = fd;
  this->Acceptor::operator=(ipv6a);
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
 *  \see IO::Socket
 */
IO::Stream* IPv6Acceptor::Accept()
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
void IPv6Acceptor::Close()
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
 *  Before being able to Accept() new clients, the IPv6Acceptor has to be put
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
void IPv6Acceptor::Listen(unsigned short port, const char* iface)
{
  sockaddr_in6 sin6;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_INET6, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (Exception(errno, strerror(errno)));

  // Set the binding structure
  memset(&sin6, 0, sizeof(sin6));
  if (iface)
    {
      if (inet_pton(AF_INET6, iface, sin6.sin6_addr.s6_addr) != 1)
        throw (Exception(errno, strerror(errno)));
    }
  else
    sin6.sin6_addr = in6addr_any;
  sin6.sin6_family = AF_INET6;
  sin6.sin6_port = htons(port);

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sin6, sizeof(sin6))
      || listen(this->sockfd_, 0))
    throw (Exception(errno, strerror(errno)));

  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                IPv6Connector                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  IPv6Connector default constructor.
 *
 *  \throw Exception Socket creation failed.
 */
IPv6Connector::IPv6Connector() : Socket(-1)
{
  this->fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (this->fd < 0)
    throw (Exception(errno, strerror(errno)));
}

/**
 *  IPv6Connector copy constructor.
 *
 *  \param[in] ipv6c Object to copy from.
 *
 *  \see Socket::Socket(const Socket&)
 */
IPv6Connector::IPv6Connector(const IPv6Connector& ipv6c) : Socket(ipv6c) {}

/**
 *  IPv6Connector destructor.
 */
IPv6Connector::~IPv6Connector() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] ipv6c Object to copy from.
 *
 *  \return *this
 *
 *  \see Socket::operator=(const Socket&)
 */
IPv6Connector& IPv6Connector::operator=(const IPv6Connector& ipv6c)
{
  this->Socket::operator=(ipv6c);
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
void IPv6Connector::Connect(const char* host, unsigned short port)
{
  sockaddr_in6 sin6;

  // If host is not NULL.
  if (host)
    {
      memset(&sin6, 0, sizeof(sin6));

      // Address family (IPv6).
      sin6.sin6_family = AF_INET6;

      // Check if host is an IP address first, with the hope that we will avoid
      // a DNS lookup.
      if (inet_pton(AF_INET6, host, &sin6.sin6_addr.s6_addr) != 1)
        {
          // Not an IP address, check if it's an host name.
          addrinfo* addr_info;
          addrinfo hint;

          addr_info = NULL;
          memset(&hint, 0, sizeof(hint));
          hint.ai_family = AF_INET6;
          if (getaddrinfo(host, NULL, &hint, &addr_info))
            throw (Exception(0, "Invalid hostname provided."));
          memcpy(sin6.sin6_addr.s6_addr,
                 ((sockaddr_in6*)addr_info->ai_addr)->sin6_addr.s6_addr,
                 sizeof(sin6.sin6_addr.s6_addr));
          freeaddrinfo(addr_info);
        }

      // Set port.
      sin6.sin6_port = htons(port);

      // Connect !
      if (connect(this->fd, (sockaddr*)&sin6, sizeof(sin6)))
        throw (Exception(errno, strerror(errno)));
    }
  else
    throw (Exception(0, "NULL hostname provided."));

  return ;
}
