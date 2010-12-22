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

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "exceptions/basic.hh"
#include "exceptions/retval.hh"
#include "io/net/unix.hh"

using namespace io::net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                 unix_acceptor                               *
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
unix_acceptor::unix_acceptor() : _sockfd(-1) {}

/**
 *  @brief Copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ua unix_acceptor to duplicate.
 *
 *  @see operator=(unix_acceptor const&)
 */
unix_acceptor::unix_acceptor(unix_acceptor const& ua)
  : acceptor(ua), _sockfd(-1) {
  operator=(ua);
}

/**
 *  @brief Destructor.
 *
 *  Will close the socket if it has not already been done.
 */
unix_acceptor::~unix_acceptor() {
  close();
}

/**
 *  @brief Assignement operator.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor
 *  will have the exact same parameters as the provided acceptor already
 *  have.
 *
 *  @param[in] ua unix_acceptor to duplicate.
 *
 *  @return This object.
 */
unix_acceptor& unix_acceptor::operator=(unix_acceptor const& ua) {
  int fd(dup(ua._sockfd));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "unix_acceptor::operator=: "
                                 << strerror(e));
  }
  close();
  _sockfd = fd;
  acceptor::operator=(ua);
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
io::stream* unix_acceptor::accept() {
  // Accept incoming client.
  int fd(::accept(_sockfd, NULL, NULL));
  if (fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "unix_acceptor::accept: "
                                 << strerror(e));
  }

  // Create socket object.
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
void unix_acceptor::close() {
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
 *  Before being able to accept() new clients, the unix_acceptor has to
 *  be put in the listen mode. Call this method with the desired port to
 *  do so. If an error occur, the object will be in a closed state.
 *
 *  @param[in] sock_path Name of the Unix domain socket.
 */
void unix_acceptor::listen(char const* sock_path) {
  // Close the socket if it was previously open
  close();

  // Create the new socket
  _sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "unix_acceptor::listen: "
                                 << strerror(e));
  }

  // Set the binding structure
  sockaddr_un sockaddrun;
  memset(&sockaddrun, 0, sizeof(sockaddrun));
  sockaddrun.sun_family = AF_UNIX;
  strncpy(sockaddrun.sun_path,
    sock_path,
    sizeof(sockaddrun.sun_path) - 1);
  sockaddrun.sun_path[sizeof(sockaddrun.sun_path) - 1] = '\0';

  // Bind
  if (bind(_sockfd, (struct sockaddr*)&sockaddrun, sizeof(sockaddrun))
      || ::listen(_sockfd, 0)) {
    int e(errno);
    throw (exceptions::retval(e) << "unix_acceptor::listen: "
                                 << strerror(e));
  }
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               unix_connector                                *
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
unix_connector::unix_connector() : socket(-1) {
  _fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
  if (_fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "unix_connector ctor: "
                                 << strerror(e));
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] uc Object to copy.
 *
 *  @see socket::socket(socket const&)
 */
unix_connector::unix_connector(unix_connector const& uc) : socket(uc) {}

/**
 *  Destructor.
 */
unix_connector::~unix_connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] uc Object to copy.
 *
 *  @return This object.
 *
 *  @see socket::operator=(socket const&)
 */
unix_connector& unix_connector::operator=(unix_connector const& uc) {
  socket::operator=(uc);
  return (*this);
}

/**
 *  @brief Connect to an Unix socket.
 *
 *  Connect to the specified Unix socket.
 *
 *  @param[in] sock_path Path to the Unix domain socket.
 */
void unix_connector::connect(char const* sock_path) {
  // If sock_path is not NULL.
  if (sock_path) {
    // Set connection structure.
    sockaddr_un sun;
    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, sock_path, sizeof(sun.sun_path));
    sun.sun_path[sizeof(sun.sun_path) - 1] = '\0';

    // Connect !
    if (::connect(_fd, (sockaddr*)&sun, sizeof(sun))) {
      int e(errno);
      throw (exceptions::retval(e) << "unix_acceptor::connect: "
                                   << strerror(e));
    }
  }
  else
    throw (exceptions::basic() << "NULL Unix socket path provided");

  return ;
}
