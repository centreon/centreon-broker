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
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "exception.h"
#include "io/fd.h"
#include "io/unix.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 UnixAcceptor                                *
*                                                                             *
*                                                                             *
******************************************************************************/

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
 *  instance listening on the same socket name and with the same parameters as
 *  the given object. In case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] ua UnixAcceptor to duplicate.
 *
 *  \see UnixAcceptor
 *  \see operator=
 */
void UnixAcceptor::InternalCopy(const UnixAcceptor& ua)
  throw (CentreonBroker::Exception)
{
  this->sockfd_ = dup(ua.sockfd_);
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
 *  \brief UnixAcceptor default constructor.
 *
 *  Build a UnixAcceptor.
 */
UnixAcceptor::UnixAcceptor() throw () : sockfd_(-1) {}

/**
 *  \brief UnixAcceptor copy constructor.
 *
 *  Duplicate the already listening acceptor. The resulting acceptor will have
 *  the exact same parameters as the provided acceptor already have. In case of
 *  error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] ua UnixAcceptor to duplicate.
 */
UnixAcceptor::UnixAcceptor(const UnixAcceptor& ua)
  throw (CentreonBroker::Exception) : Acceptor(ua), sockfd_(-1)
{
  this->InternalCopy(ua);
}

/**
 *  \brief UnixAcceptor destructor.
 *
 *  Will close the socket if it has not already been done.
 */
UnixAcceptor::~UnixAcceptor() throw ()
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
 *  \param[in] ua UnixAcceptor to duplicate.
 *
 *  \return *this
 */
UnixAcceptor& UnixAcceptor::operator=(const UnixAcceptor& ua)
  throw (CentreonBroker::Exception)
{
  this->Close();
  this->Acceptor::operator=(ua);
  this->InternalCopy(ua);
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
Stream* UnixAcceptor::Accept()
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
void UnixAcceptor::Close() throw ()
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
 *  in the listen mode. Call this method with the desired port to do so. In
 *  case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] sock_path Name of the Unix domain socket.
 */
void UnixAcceptor::Listen(const char* sock_path)
  throw (CentreonBroker::Exception)
{
  struct sockaddr_un sockaddrun;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));

  // Set the binding structure
  sockaddrun.sun_family = AF_UNIX;
  strncpy(sockaddrun.sun_path, sock_path, sizeof(sockaddrun.sun_path) - 1);
  sockaddrun.sun_path[sizeof(sockaddrun.sun_path) - 1] = '\0';

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sockaddrun, sizeof(sockaddrun))
      || listen(this->sockfd_, 0))
    throw (CentreonBroker::Exception(errno, strerror(errno)));
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
 */
UnixConnector::UnixConnector() throw (CentreonBroker::Exception)
  : SocketStream(-1)
{
  this->fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (this->fd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
}

/**
 *  UnixConnector copy constructor.
 *
 *  \param[in] uc Object to copy from.
 */
UnixConnector::UnixConnector(const UnixConnector& uc)
  throw (CentreonBroker::Exception) : SocketStream(uc) {}

/**
 *  UnixConnector destructor.
 */
UnixConnector::~UnixConnector() throw () {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] uc Object to copy from.
 */
UnixConnector& UnixConnector::operator=(const UnixConnector& uc)
  throw (CentreonBroker::Exception)
{
  this->SocketStream::operator=(uc);
  return (*this);
}

/**
 *  \brief Connect to an Unix socket.
 *
 *  Connect to the specified Unix socket.
 *
 *  \param[in] sock_path Path to the Unix domain socket.
 */
void UnixConnector::Connect(const char* sock_path)
  throw (CentreonBroker::Exception)
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
      if (connect(this->fd_, (sockaddr*)&sun, sizeof(sun)))
        throw (CentreonBroker::Exception(errno, strerror(errno)));
    }
  else
    throw (CentreonBroker::Exception(0, "NULL Unix socket path provided."));

  return ;
}
