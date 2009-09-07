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
#include <unistd.h>
#include "exception.h"
#include "io/fd.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 FDStream                                    *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Non-overloaded method to close the internal file descriptor.
 *
 *  \see Close
 */
void FDStream::InternalClose() throw ()
{
  if (this->fd_ >= 0)
    {
      close(this->fd_);
      this->fd_ = -1;
    }
  return ;
}

/**
 *  \brief Duplicate the given object's file descriptor.
 *
 *  This function is used by the copy constructor and operator= to duplicate
 *  the file descriptor of the object given as a parameter. Namely the file
 *  descriptor will be dup()ed and the resulting new file descriptor will be
 *  stored with the current instance. If dup() failed, the method will throw a
 *  CentreonBroker::Exception.
 *
 *  \param[in] fds Object which holds the original file descriptor.
 *
 *  \see FDStream
 *  \see operator=
 */
void FDStream::InternalCopy(const FDStream& fds)
  throw (CentreonBroker::Exception)
{
  this->fd_ = dup(fds.fd_);
  if (this->fd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief FDStream constructor.
 *
 *  Build a FDStream by providing an already opened file descriptor. Once the
 *  constructor has been successfully executed, the FDStream object is
 *  responsible of the file descriptor (ie. it will handle all I/O operations
 *  as well as closing).
 *
 *  \param[in] fd Open file descriptor.
 */
FDStream::FDStream(int fd) throw () : fd_(fd) {}

/**
 *  \brief FDStream copy constructor.
 *
 *  Duplicate the FDStream object given as a parameter. The internal file
 *  descriptor will be dup()ed. A CentreonBroker::Exception will be thrown in
 *  case of error.
 *
 *  \param[in] fds FDStream to duplicate.
 */
FDStream::FDStream(const FDStream& fds) throw (CentreonBroker::Exception)
  : Stream(fds)
{
  this->InternalCopy(fds);
}

/**
 *  \brief FDStream destructor.
 *
 *  The destructor will close the file descriptor properly if it has not
 *  already been closed.
 */
FDStream::~FDStream() throw ()
{
  this->InternalClose();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Close the current file descriptor (if open) and duplicate the FDStream
 *  object given as a parameter. The internal file descriptor will be dup()ed.
 *  In case of error, the current object will be in a closed state and a
 *  CentreonBroker::Exception will be thrown.
 *
 *  \param[in] fds FDStream to duplicate.
 *
 *  \return *this
 */
FDStream& FDStream::operator=(const FDStream& fds)
  throw (CentreonBroker::Exception)
{
  this->InternalClose();
  this->Stream::operator=(fds);
  this->InternalCopy(fds);
  return (*this);
}

/**
 *  \brief Close the internal file descriptor.
 *
 *  If called, it won't be possible to use the object without error anymore.
 */
void FDStream::Close() throw ()
{
  this->InternalClose();
  return ;
}

/**
 *  \brief Receive data from the source connected to the file descriptor.
 *
 *  Receive at most size bytes from the FD and store them in buffer. The number
 *  of bytes read is then returned. This number can be less than size. In case
 *  of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[out] buffer Buffer on which to store received data.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \return Number of bytes read from the FD. 0 if the FD has been closed.
 */
unsigned int FDStream::Receive(void* buffer, unsigned int size)
  throw (CentreonBroker::Exception)
{
  int ret;

  ret = read(this->fd_, buffer, size);
  if (ret < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ((unsigned int)ret);
}

/**
 *  \brief Send data to the file descriptor.
 *
 *  Send at most size bytes from the buffer. The number of bytes actually sent
 *  is returned. This number can be less than size. In case of error, a
 *  CentreonBroker::Exception is thrown.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \return Number of bytes actually sent to the network stream. 0 if the FD
 *          has been closed.
 */
unsigned int FDStream::Send(const void* buffer, unsigned int size)
  throw (CentreonBroker::Exception)
{
  int ret;

  ret = write(this->fd_, buffer, size);
  if (ret < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
  return ((unsigned int)ret);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                              SocketStream                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief SocketStream constructor.
 *
 *  Build a SocketStream by providing an already opened socket file descriptor.
 *  The type of the socket (AF_INET, AF_UNIX, ...) doesn't matter. Once the
 *  constructor has been successfully executed, the FDStream object is
 *  responsible of the file descriptor (ie. it will handle all I/O operations
 *  as well as closing).
 *
 *  \param[in] sockfd Open socket.
 */
SocketStream::SocketStream(int sockfd) throw () : FDStream(sockfd) {}

/**
 *  \brief SocketStream copy constructor.
 *
 *  Duplicate the SocketStream object duplicate as a parameter. Refer to
 *  FDStream copy constructor for more informations.
 *
 *  \param[in] sockstream SocketStream to duplicate.
 */
SocketStream::SocketStream(const SocketStream& sockstream)
  throw (CentreonBroker::Exception)
  : FDStream(sockstream) {}

/**
 *  \brief SocketStream destructor.
 *
 *  The destructor will shutdown the connection and close the socket file
 *  descriptor if it has not already been closed.
 */
SocketStream::~SocketStream() throw ()
{
  this->Close();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Shutdown the current socket properly and close it if open. Then duplicate
 *  the SocketStream argument. Refer to FDStream::operator= for more
 *  informations.
 *
 *  \param[in] sockstream SocketStream to duplicate.
 *
 *  \return *this
 */
SocketStream& SocketStream::operator=(const SocketStream& sockstream)
  throw (CentreonBroker::Exception)
{
  this->Close();
  this->FDStream::operator=(sockstream);
  return (*this);
}

/**
 *  Shutdown the connection and close the file descriptor.
 */
void SocketStream::Close() throw ()
{
  if (this->fd_ >= 0)
    {
      shutdown(this->fd_, SHUT_RDWR);
      this->FDStream::Close();
    }
  return ;
}
