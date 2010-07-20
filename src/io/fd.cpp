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
#include <string.h>    // for strerror
#include <unistd.h>    // for close
#include "exception.h"
#include "io/fd.h"

using namespace IO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Non-overloaded method to close the internal file descriptor.
 *
 *  \see Close()
 */
void FD::InternalClose()
{
  if (this->fd >= 0)
    {
      close(this->fd);
      this->fd = -1;
    }
  return ;
}

/**
 *  \brief Duplicate the given object's file descriptor.
 *
 *  This function is used by the copy constructor and the assignment operator
 *  to duplicate the file descriptor of the object given as a parameter. Namely
 *  the file descriptor will be dup()'ed and the resulting new file descriptor
 *  will be stored with the current instance. If dup() failed, the method will
 *  throw an Exception.
 *
 *  \param[in] fd Object which holds the original file descriptor.
 *
 *  \exception Exception File descriptor could not be duplicated.
 *
 *  \see FD(const FD&)
 *  \see operator=(const FD&)
 */
void FD::InternalCopy(const FD& fd)
{
  this->fd = dup(fd.fd);
  if (this->fd < 0)
    throw (Exception(errno, strerror(errno)));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief FD constructor.
 *
 *  Build a FD by providing an already opened file descriptor. If no file
 *  descriptor is provided, -1 is used as a default value. Once the constructor
 *  has been successfully executed, the FD object is responsible of the file
 *  descriptor (ie. it will handle all I/O operations as well as closing).
 *
 *  \param[in] fd Open file descriptor.
 */
FD::FD(int fildes) : fd(fildes) {}

/**
 *  \brief FD copy constructor.
 *
 *  Duplicate the FD object given as a parameter. The internal file descriptor
 *  will be dup()'ed. An Exception will be thrown in case of error.
 *
 *  \param[in] fd FD to duplicate.
 *
 *  \exception Exception File descriptor could not be duplicated.
 */
FD::FD(const FD& fd) : Stream(fd)
{
  this->InternalCopy(fd);
}

/**
 *  \brief FD destructor.
 *
 *  The destructor will close the file descriptor properly if it has not
 *  already been closed.
 */
FD::~FD()
{
  this->InternalClose();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  Close the current file descriptor (if open) and duplicate the FD object
 *  given as a parameter. The internal file descriptor will be dup()'ed. In
 *  case of error, the current object will be in a closed state and an
 *  Exception will be thrown.
 *
 *  \param[in] fd FD to duplicate.
 *
 *  \return *this
 */
FD& FD::operator=(const FD& fd)
{
  this->InternalClose();
  this->Stream::operator=(fd);
  this->InternalCopy(fd);
  return (*this);
}

/**
 *  \brief Close the internal file descriptor.
 *
 *  If called, it won't be possible to use the object without error anymore.
 */
void FD::Close()
{
  this->InternalClose();
  return ;
}

/**
 *  \brief Receive data from the source connected to the file descriptor.
 *
 *  Receive at most size bytes from the FD and store them in buffer. The number
 *  of bytes read is then returned. This number can be less than size. In case
 *  of error, a Exception is thrown.
 *
 *  \param[out] buffer Buffer on which to store received data.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \exception Exception An error occurred on the file descriptor. Refer to the
 *                       exception error code for more information.
 *
 *  \return Number of bytes read from the file descriptor.
 */
unsigned int FD::Receive(void* buffer, unsigned int size)
{
  int ret;

  ret = read(this->fd, buffer, size);
  if (ret < 0)
    throw (Exception(errno, strerror(errno)));
  return ((unsigned int)ret);
}

/**
 *  \brief Send data to the file descriptor.
 *
 *  Send at most size bytes from the buffer. The number of bytes actually sent
 *  is returned. This number can be less than size. In case of error, an
 *  Exception is thrown.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \exception Exception An error occurred on the file descriptor. Refer to the
 *                       exception error code for more information.
 *
 *  \return Number of bytes actually sent to the file descriptor.
 */
unsigned int FD::Send(const void* buffer, unsigned int size)
{
  int ret;

  ret = write(this->fd, buffer, size);
  if (ret < 0)
    throw (Exception(errno, strerror(errno)));
  return ((unsigned int)ret);
}
