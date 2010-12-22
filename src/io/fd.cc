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
#include <unistd.h>
#include "exceptions/retval.hh"
#include "io/fd.hh"

using namespace io;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Non-overloaded method to close the internal file descriptor.
 *
 *  @see close()
 */
void fd::_internal_close() {
  if (_fd >= 0) {
    ::close(_fd);
    _fd = -1;
  }
  return ;
}

/**
 *  @brief Duplicate the given object's file descriptor.
 *
 *  This function is used by the copy constructor and the assignment
 *  operator to duplicate the file descriptor of the object given as a
 *  parameter. Namely the file descriptor will be dup()'ed and the
 *  resulting new file descriptor will be stored with the current
 *  instance. If dup() failed, the method will throw an exception.
 *
 *  @param[in] f Object which holds the original file descriptor.
 *
 *  @see fd(fd const&)
 *  @see operator=(fd const&)
 */
void fd::_internal_copy(fd const& f) {
  _fd = dup(f._fd);
  if (_fd < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "fd::_internal_copy: "
                                 << strerror(e));
  }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Build a FD by providing an already opened file descriptor. If no
 *  file descriptor is provided, -1 is used as a default value. Once the
 *  constructor has been successfully executed, the fd object is
 *  responsible of the file descriptor (ie. it will handle all I/O
 *  operations as well as closing).
 *
 *  @param[in] f Open file descriptor.
 */
fd::fd(int fildes) : _fd(fildes) {}

/**
 *  @brief Copy constructor.
 *
 *  Duplicate the fd object given as a parameter. The internal file
 *  descriptor will be dup()'ed. An exception will be thrown in case of
 *  error.
 *
 *  @param[in] f FD to duplicate.
 */
fd::fd(fd const& f) : stream(f) {
  _internal_copy(f);
}

/**
 *  @brief Destructor.
 *
 *  The destructor will close the file descriptor properly if it has not
 *  already been closed.
 */
fd::~fd() {
  _internal_close();
}

/**
 *  @brief Assignement operator.
 *
 *  Close the current file descriptor (if open) and duplicate the FD
 *  object given as a parameter. The internal file descriptor will be
 *  dup()'ed. In case of error, the current object will be in a closed
 *  state and an exception will be thrown.
 *
 *  @param[in] f FD to duplicate.
 *
 *  @return This object.
 */
fd& fd::operator=(fd const& f) {
  _internal_close();
  stream::operator=(f);
  _internal_copy(f);
  return (*this);
}

/**
 *  @brief Close the internal file descriptor.
 *
 *  If called, it won't be possible to use the object without error
 *  anymore.
 */
void fd::close() {
  _internal_close();
  return ;
}

/**
 *  @brief Receive data from the source connected to the file
 *         descriptor.
 *
 *  Receive at most size bytes from the FD and store them in buffer. The
 *  number of bytes read is then returned. This number can be less than
 *  size. In case of error, an exception is thrown.
 *
 *  @param[out] buffer Buffer on which to store received data.
 *  @param[in]  size   Maximum number of bytes to read.
 *
 *  @return Number of bytes read from the file descriptor.
 */
unsigned int fd::receive(void* buffer, unsigned int size) {
  int ret(read(_fd, buffer, size));
  if (ret < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "fd::receive: " << strerror(e));
  }
  return (static_cast<unsigned int>(ret));
}

/**
 *  @brief Send data to the file descriptor.
 *
 *  Send at most size bytes from the buffer. The number of bytes
 *  actually sent is returned. This number can be less than size. In
 *  case of error, an exception is thrown.
 *
 *  @param[in] buffer Data to send.
 *  @param[in] size   Maximum number of bytes to send.
 *
 *  @return Number of bytes actually sent to the file descriptor.
 */
unsigned int fd::send(void const* buffer, unsigned int size) {
  int ret(write(_fd, buffer, size));
  if (ret < 0) {
    int e(errno);
    throw (exceptions::retval(e) << "fd::send: " << strerror(e));
  }
  return (static_cast<unsigned int>(ret));
}
