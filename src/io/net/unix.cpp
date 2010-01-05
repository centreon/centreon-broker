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

#include <errno.h>
#include <string.h>      // for memset, strerror
#include <sys/socket.h>  // for shutdown, socket
#include <sys/un.h>      // for sockaddr_un
#include <unistd.h>      // for close, dup, fsync
#include "exception.h"
#include "io/net/unix.h"

using namespace IO::Net;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                 UnixAcceptor                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  UnixAcceptor default constructor.
 */
UnixAcceptor::UnixAcceptor() : sockfd_(-1) {}

/**
 *  \brief UnixAcceptor copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have.
 *
 *  \param[in] ua UnixAcceptor to duplicate.
 *
 *  \see operator=(const UnixAcceptor&)
 */
UnixAcceptor::UnixAcceptor(const UnixAcceptor& ua) : Acceptor(ua), sockfd_(-1)
{
  this->operator=(ua);
}

/**
 *  \brief UnixAcceptor destructor.
 *
 *  Will close the socket if it has not already been done.
 */
UnixAcceptor::~UnixAcceptor()
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
 *  \param[in] ua UnixAcceptor to duplicate.
 *
 *  \return *this
 *
 *  \throw Exception Either ua is invalid or the duplication failed.
 */
UnixAcceptor& UnixAcceptor::operator=(const UnixAcceptor& ua)
{
  int fd;

  fd = dup(ua.sockfd_);
  if (fd < 0)
    throw (Exception(errno, strerror(errno)));
  this->Close();
  this->sockfd_ = fd;
  this->Acceptor::operator=(ua);
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
IO::Stream* UnixAcceptor::Accept()
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
void UnixAcceptor::Close()
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
 *  Before being able to Accept() new clients, the UnixAcceptor has to be put
 *  in the listen mode. Call this method with the desired port to do so. If an
 *  error occur, the object will be in a closed state.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] sock_path Name of the Unix domain socket.
 *
 *  \throw Exception Either socket creation failed or socket binding failed.
 */
void UnixAcceptor::Listen(const char* sock_path)
{
  sockaddr_un sockaddrun;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (Exception(errno, strerror(errno)));

  // Set the binding structure
  memset(&sockaddrun, 0, sizeof(sockaddrun));
  sockaddrun.sun_family = AF_UNIX;
  strncpy(sockaddrun.sun_path, sock_path, sizeof(sockaddrun.sun_path) - 1);
  sockaddrun.sun_path[sizeof(sockaddrun.sun_path) - 1] = '\0';

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sockaddrun, sizeof(sockaddrun))
      || listen(this->sockfd_, 0))
    throw (Exception(errno, strerror(errno)));
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               UnixConnector                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  UnixConnector default constructor.
 *
 *  \throw Exception Socket creation failed.
 */
UnixConnector::UnixConnector() : Socket(-1)
{
  this->fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (this->fd < 0)
    throw (Exception(errno, strerror(errno)));
}

/**
 *  UnixConnector copy constructor.
 *
 *  \param[in] uc Object to copy from.
 *
 *  \see Socket::Socket(const Socket&)
 */
UnixConnector::UnixConnector(const UnixConnector& uc) : Socket(uc) {}

/**
 *  UnixConnector destructor.
 */
UnixConnector::~UnixConnector() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] uc Object to copy from.
 *
 *  \return *this
 *
 *  \see Socket::operator=(const Socket&)
 */
UnixConnector& UnixConnector::operator=(const UnixConnector& uc)
{
  this->Socket::operator=(uc);
  return (*this);
}

/**
 *  \brief Connect to an Unix socket.
 *
 *  Connect to the specified Unix socket.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] sock_path Path to the Unix domain socket.
 *
 *  \throw Exception sock_path is invalid or connection failed.
 */
void UnixConnector::Connect(const char* sock_path)
{
  sockaddr_un sun;

  // If sock_path is not NULL.
  if (sock_path)
    {
      // Set connection structure.
      memset(&sun, 0, sizeof(sun));
      sun.sun_family = AF_UNIX;
      strncpy(sun.sun_path, sock_path, sizeof(sun.sun_path));
      sun.sun_path[sizeof(sun.sun_path) - 1] = '\0';

      // Connect !
      if (connect(this->fd, (sockaddr*)&sun, sizeof(sun)))
        throw (Exception(errno, strerror(errno)));
    }
  else
    throw (Exception(0, "NULL Unix socket path provided."));

  return ;
}
