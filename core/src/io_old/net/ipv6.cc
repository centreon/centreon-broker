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

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "exceptions/basic.hh"
#include "exceptions/retval.hh"
#include "io/net/ipv6.hh"

using namespace io::net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                ipv6_acceptor                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
ipv6_acceptor::ipv6_acceptor() : _sockfd(-1) {}

/**
 *  @brief Copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ipv6a ipv6_acceptor to duplicate.
 *
 *  @see operator=(ipv6_acceptor const&)
 */
ipv6_acceptor::ipv6_acceptor(ipv6_acceptor const& ipv6a)
  : acceptor(ipv6a), _sockfd(-1) {
  operator=(ipv6a);
}

/**
 *  @brief Destructor.
 *
 *  Will close the socket if it has not already been done.
 */
ipv6_acceptor::~ipv6_acceptor() {
  close();
}

/**
 *  @brief Assignement operator.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ipv6a ipv6_acceptor to duplicate.
 *
 *  @return This object.
 */
ipv6_acceptor& ipv6_acceptor::operator=(ipv6_acceptor const& ipv6a) {
  int fd(dup(ipv6a._sockfd));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv6_acceptor::operator=: "
                                 << strerror(e));
  }
  close();
  _sockfd = fd;
  acceptor::operator=(ipv6a);
  return (*this);
}

/**
 *  @brief Wait for a new incoming client.
 *
 *  Once the acceptor is in a listening state, one can wait for incoming
 *  client by using this method. Once a client is properly connected,
 *  this method will return a stream object (in fact a socket object).
 *
 *  @return A stream object representing the new client connection.
 *
 *  @see io::socket
 */
io::stream* ipv6_acceptor::accept() {
  // Accept new client.
  int fd(::accept(_sockfd, NULL, NULL));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv6_acceptor::accept: "
                                 << strerror(e));
  }

  // Create socket object.
  socket* sock = NULL;
  try {
    sock = new socket(fd);
  }
  catch (...) {
    ::close(fd);
    throw ;
  }
  return (sock);
}

/**
 *  @brief Close the acceptor.
 *
 *  Shutdown the socket properly. No more client connection can be made
 *  through this acceptor.
 */
void ipv6_acceptor::close() {
  if (_sockfd >= 0) {
    shutdown(_sockfd, SHUT_RDWR);
    fsync(_sockfd);
    ::close(_sockfd);
    _sockfd = -1;
  }
  return ;
}

/**
 *  @brief Put the acceptor in listen mode.
 *
 *  Before being able to accept() new clients, the ipv6_acceptor has to
 *  be put in the listen mode. Call this method with the desired port to
 *  do so. If an error occur, the object will be in a closed state.
 *
 *  @param[in] port  Port on which the acceptor should listen on.
 *  @param[in] iface IP address of the interface the acceptor should be
 *                   to. If NULL (default), bind to all available
 *                   interfaces.
 */
void ipv6_acceptor::listen(unsigned short port, char const* iface) {
  // Close the socket if it was previously open
  close();

  // Create the new socket
  _sockfd = ::socket(AF_INET6, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv6_acceptor::listen: "
                                 << strerror(e));
  }

  // Set the binding structure
  sockaddr_in6 sin6;
  memset(&sin6, 0, sizeof(sin6));
  if (iface) {
    if (inet_pton(AF_INET6, iface, sin6.sin6_addr.s6_addr) != 1) {
      int e(errno);
      throw (exceptions::retval(e) << "ipv6_acceptor::listen: "
                                   << strerror(e));
    }
  }
  else
    sin6.sin6_addr = in6addr_any;
  sin6.sin6_family = AF_INET6;
  sin6.sin6_port = htons(port);

  // Bind
  if (bind(_sockfd, (struct sockaddr*)&sin6, sizeof(sin6))
      || ::listen(_sockfd, 0)) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv6_acceptor::listen: "
                                 << strerror(e));
  }

  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                ipv6_connector                               *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
ipv6_connector::ipv6_connector() : socket(-1) {
  _fd = ::socket(AF_INET6, SOCK_STREAM, 0);
  if (_fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv6_connector ctor: "
                                 << strerror(e));
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] ipv6c Object to copy.
 *
 *  @see socket::socket(socket const&)
 */
ipv6_connector::ipv6_connector(ipv6_connector const& ipv6c)
  : socket(ipv6c) {}

/**
 *  Destructor.
 */
ipv6_connector::~ipv6_connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ipv6c Object to copy.
 *
 *  @return This object.
 *
 *  @see socket::operator=(socket const&)
 */
ipv6_connector& ipv6_connector::operator=(ipv6_connector const& ipv6c) {
  socket::operator=(ipv6c);
  return (*this);
}

/**
 *  @brief Connect to an host.
 *
 *  Connect to the specified host on the specified port.
 *
 *  @param[in] host Hostname or IP address to connect to.
 *  @param[in] port Port on which connection will be made.
 */
void ipv6_connector::connect(char const* host, unsigned short port) {
  // If host is not NULL.
  if (host) {
    sockaddr_in6 sin6;
    memset(&sin6, 0, sizeof(sin6));

    // Address family (IPv6).
    sin6.sin6_family = AF_INET6;

    // Check if host is an IP address first, with the hope that we will
    // avoid a DNS lookup.
    if (inet_pton(AF_INET6, host, &sin6.sin6_addr.s6_addr) != 1) {
      // Not an IP address, check if it's an host name.
      addrinfo* addr_info(NULL);
      addrinfo hint;
      addr_info = NULL;
      memset(&hint, 0, sizeof(hint));
      hint.ai_family = AF_INET6;
      if (getaddrinfo(host, NULL, &hint, &addr_info))
        throw (exceptions::basic() << "invalid hostname provided");
      memcpy(sin6.sin6_addr.s6_addr,
             ((sockaddr_in6*)addr_info->ai_addr)->sin6_addr.s6_addr,
             sizeof(sin6.sin6_addr.s6_addr));
      freeaddrinfo(addr_info);
    }

    // Set port.
    sin6.sin6_port = htons(port);

    // Connect !
    if (::connect(_fd, (sockaddr*)&sin6, sizeof(sin6))) {
      int e(errno);
      throw (exceptions::retval(e) << "ipv6_connector::connect: "
                                   << strerror(e));
    }
  }
  else
    throw (exceptions::basic() << "NULL hostname provided");

  return ;
}
