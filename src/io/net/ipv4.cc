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

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "exceptions/basic.hh"
#include "exceptions/retval.hh"
#include "io/net/ipv4.hh"

using namespace io::net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                ipv4_acceptor                                *
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
ipv4_acceptor::ipv4_acceptor() : _sockfd(-1) {}

/**
 *  @brief Copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ipv4a ipv4_acceptor to duplicate.
 *
 *  @see operator=(ipv4_acceptor const&)
 */
ipv4_acceptor::ipv4_acceptor(ipv4_acceptor const& ipv4a)
  : acceptor(ipv4a), _sockfd(-1) {
  operator=(ipv4a);
}

/**
 *  @brief Destructor.
 *
 *  Will close the socket if it has not already been done.
 */
ipv4_acceptor::~ipv4_acceptor() {
  close();
}

/**
 *  @brief Assignement operator.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ipv4a ipv4_acceptor to duplicate.
 *
 *  @return This object.
 */
ipv4_acceptor& ipv4_acceptor::operator=(ipv4_acceptor const& ipv4a) {
  int fd(dup(ipv4a._sockfd));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv4_acceptor::operator=: "
                                 << strerror(e));
  }
  close();
  _sockfd = fd;
  acceptor::operator=(ipv4a);
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
 *  @see io::stream
 */
io::stream* ipv4_acceptor::accept() {
  // Accept new client.
  int fd(::accept(_sockfd, NULL, NULL));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv4_acceptor::accept: "
                                 << strerror(e));
  }

  // Create new socket object.
  socket* sock(NULL);
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
void ipv4_acceptor::close() {
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
 *  Before being able to accept() new clients, the ipv4_acceptor has to
 *  be put in the listen mode. Call this method with the desired port to
 *  do so. If an error occur, the object will be in a closed state.
 *
 *  @param[in] port  Port on which the acceptor should listen on.
 *  @param[in] iface IP address of the interface the acceptor should be
 *                   to. If NULL (default), bind to all available
 *                   interfaces.
 */
void ipv4_acceptor::listen(unsigned short port, char const* iface) {
  // Close the socket if it was previously open
  close();

  // Create the new socket
  _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv4_acceptor::listen: "
                                 << strerror(e));
  }

  // Set the binding structure
  sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  if (iface) {
    if (inet_pton(AF_INET, iface, &sin.sin_addr.s_addr) != 1) {
      int e(errno);
      throw (exceptions::retval(e) << "ipv4_acceptor::listen: "
                                   << strerror(e));
    }
  }
  else
    sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  // Bind
  if (bind(_sockfd, (struct sockaddr*)&sin, sizeof(sin))
      || ::listen(_sockfd, 0)) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv4_acceptor::listen: "
                                 << strerror(e));
  }

  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                ipv4_connector                               *
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
ipv4_connector::ipv4_connector() : socket(-1) {
  _fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (_fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "ipv4_connector ctor: "
                                 << strerror(e));
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] ipv4c Object to copy.
 *
 *  @see socket::socket(socket const&)
 */
ipv4_connector::ipv4_connector(ipv4_connector const& ipv4c)
  : socket(ipv4c) {}

/**
 *  Destructor.
 */
ipv4_connector::~ipv4_connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ipv4c Object to copy.
 *
 *  @return This object.
 *
 *  @see socket::operator=(socket const&)
 */
ipv4_connector& ipv4_connector::operator=(ipv4_connector const& ipv4c) {
  socket::operator=(ipv4c);
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
void ipv4_connector::connect(char const* host, unsigned short port) {
  // If host is not NULL.
  if (host) {
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    // Address family (IPv4).
    sin.sin_family = AF_INET;

    // Check if host is an IP address first, with the hope that we will
    // avoid a DNS lookup.
    if (inet_pton(AF_INET, host, &sin.sin_addr.s_addr) != 1) {
      // Not an IP address, check if it's an host name.
      addrinfo* addr_info(NULL);
      addrinfo hint;
      memset(&hint, 0, sizeof(hint));
      hint.ai_family = AF_INET;
      if (getaddrinfo(host, NULL, &hint, &addr_info))
        throw (exceptions::basic() << "invalid hostname provided");
      sin.sin_addr.s_addr
        = ((sockaddr_in*)addr_info->ai_addr)->sin_addr.s_addr;
      freeaddrinfo(addr_info);
    }

    // Set port.
    sin.sin_port = htons(port);

    // Connect !
    if (::connect(_fd, (sockaddr*)&sin, sizeof(sin))) {
      int e(errno);
      throw (exceptions::retval(e) << "ipv4_connector::connect: "
                                   << strerror(e));
      }
    }
  else
    throw (exceptions::basic() << "NULL hostname provided");

  return ;
}
