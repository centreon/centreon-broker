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

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "exception.h"
#include "io/unix.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 UnixStream                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Duplicate the given object's socket.
 *
 *  This function is used by the copy constructor and operator= to duplicate
 *  the file descriptor of the object given as a parameter. Namely the file
 *  descriptor will be dup()ed and the resulting new file descriptor will be
 *  stored with the current instance. If dup() failed, the method will throw a
 *  CentreonBroker::Exception.
 *
 *  \param[in] us Object which holds the original socket file descriptor.
 *
 *  \see UnixStream
 *  \see operator=
 */
void UnixStream::InternalCopy(const UnixStream& us)
  throw (CentreonBroker::Exception)
{
  this->sockfd_ = dup(us.sockfd_);
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
 *  \brief UnixStream constructor.
 *
 *  Build a UnixStream by providing an already opened Unix domain socket file
 *  descriptor. Once the constructor has been successfully executed, the
 *  UnixStream object is responsible of the file descriptor (ie. it will handle
 *  all I/O operations as well as closing).
 *
 *  \param[in] sockfd Berkeley style socket file descriptor.
 */
UnixStream::UnixStream(int sockfd) throw () : sockfd_(sockfd) {}

/**
 *  \brief UnixStream copy constructor.
 *
 *  Duplicate the UnixStream object given as a parameter. The internal socket
 *  file descriptor will be dup()ed. A CentreonBroker::Exception will be thrown
 *  in case of error.
 *
 *  \param[in] us UnixStream to duplicate.
 */
UnixStream::UnixStream(const UnixStream& us) throw (CentreonBroker::Exception)
  : Stream(us)
{
  this->InternalCopy(us);
}

/**
 *  \brief UnixStream destructor.
 *
 *  The destructor will call Close() if the call has not already been made.
 */
UnixStream::~UnixStream() throw ()
{
  this->Close();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Close the current socket and duplicate the UnixStream object given as a
 *  parameter. The internal socket file descriptor will be dup()ed. In case of
 *  error, the current object will be in a closed state and a
 *  CentreonBroker::Exception will be thrown.
 *
 *  \param[in] us UnixStream to duplicate.
 *
 *  \return *this
 */
UnixStream& UnixStream::operator=(const UnixStream& us)
  throw (CentreonBroker::Exception)
{
  this->Close();
  this->Stream::operator=(us);
  this->InternalCopy(us);
  return (*this);
}

/**
 *  \brief Close the UnixStream socket.
 *
 *  Close the current socket. If called directly, it won't be possible to use
 *  the object without error anymore.
 */
void UnixStream::Close() throw ()
{
  if (this->sockfd_ >= 0)
    {
      shutdown(this->sockfd_, SHUT_RDWR);
      close(this->sockfd_);
      this->sockfd_ = -1;
    }
  return ;
}

/**
 *  \brief Receive data from the IPC socket.
 *
 *  Receive at most size bytes from another process and store them in buffer.
 *  The number of bytes read is then returned. This number can be less than
 *  size. In case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[out] buffer Buffer on which to store received data.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \return Number of bytes read from the local stream. 0 if the connection
 *          has been shut down.
 */
int UnixStream::Receive(char* buffer, int size)
  throw (CentreonBroker::Exception)
{
  int ret;

  ret = recv(this->sockfd_, buffer, size, 0);
  if (ret < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return (ret);
}

/**
 *  \brief Send data across the IPC socket.
 *
 *  Send at most size bytes from the buffer. The number of bytes actually sent
 *  is returned. This number can be less than size. In case of error, a
 *  CentreonBroker::Exception is thrown.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \return Number of bytes actually sent to the peer process. 0 if the
 *          connection has been shut down.
 */
int UnixStream::Send(const char* buffer, int size)
  throw (CentreonBroker::Exception)
{
  int ret;

  ret = send(this->sockfd_, buffer, size, 0);
  if (ret < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return (ret);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               UnixAcceptor                                  *
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
 *  method will return a Stream object (in fact a UnixStream object). In case
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
  return (new UnixStream(fd));
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
  struct sockaddr_un sun;

  // Close the socket if it was previously open
  this->Close();

  // Create the new socket
  this->sockfd_ = socket(AF_UNIX, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));

  // Set the binding structure
  sun.sun_family = AF_UNIX;
  strncpy(sun.sun_path, sock_path, sizeof(sun.sun_path) - 1);
  sun.sun_path[sizeof(sun.sun_path) - 1] = '\0';

  // Bind
  if (bind(this->sockfd_, (struct sockaddr*)&sun, sizeof(sun))
      || listen(this->sockfd_, 0))
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ;
}
