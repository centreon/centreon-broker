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

#include <sys/socket.h>
#include "io/net/socket.hh"

using namespace io::net;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Build a socket by providing an already opened socket file
 *  descriptor. The type of the socket (AF_INET, AF_UNIX, ...) doesn't
 *  matter. Once the constructor has been successfully executed, the
 *  socket object is responsible of the file descriptor (ie. it will
 *  handle all I/O operations as well as closing).
 *
 *  @param[in] sockfd Open socket.
 */
io::net::socket::socket(int sockfd) : fd(sockfd) {}

/**
 *  @brief Copy constructor.
 *
 *  Duplicate the socket object given as a parameter. Refer to the
 *  io::fd copy constructor for more information.
 *
 *  @param[in] sock Socket to duplicate.
 *
 *  @see io::fd::fd(io::fd const&)
 */
io::net::socket::socket(io::net::socket const& sock) : fd(sock) {}

/**
 *  @brief Destructor.
 *
 *  The destructor will shutdown the connection and close the socket
 *  file descriptor if it has not already been closed.
 */
io::net::socket::~socket() {
  close();
}

/**
 *  @brief Assignement operator.
 *
 *  Shutdown the current socket properly and close it if open. Then
 *  duplicate the socket argument. Refer to the io::fd assignment
 *  operator overload for more information.
 *
 *  @param[in] sock Socket to duplicate.
 *
 *  @return This object.
 *
 *  @see io::fd::operator=(io::fd const&)
 */
io::net::socket& io::net::socket::operator=(io::net::socket const& sock) {
  close();
  io::fd::operator=(sock);
  return (*this);
}

/**
 *  Shutdown the connection and close the file descriptor.
 */
void io::net::socket::close() {
  if (_fd >= 0) {
    shutdown(_fd, SHUT_RDWR);
    io::fd::close();
  }
  return ;
}
