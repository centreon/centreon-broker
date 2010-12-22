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
#include <fcntl.h>
#include <string.h>
#include "exceptions/retval.hh"
#include "io/file.hh"

using namespace io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
file::file() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f File to duplicate.
 *
 *  @see fd::fd(fd const&)
 */
file::file(file const& f) : fd(f) {}

/**
 *  Destructor.
 */
file::~file() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f File to duplicate.
 *
 *  @return This object.
 *
 *  @see fd::operator=(fd const&)
 */
file& file::operator=(file const& f) {
  fd::operator=(f);
  return (*this);
}

/**
 *  @brief Open a file.
 *
 *  Open a file with the modes described in flags.
 *
 *  @param[in] filename Name of the file to open.
 *  @param[in] flag     Mode on which flag should be opened.
 */
void file::open(char const* filename, int flag) {
  if (filename) {
    // O_RDWR is not guaranteed to be equal to O_RDONLY|O_WRONLY.
    if (flag & (READ | WRITE))
      flag = ((flag & ~(READ | WRITE)) | O_RDWR);

    // Open file.
    int fildes(::open(filename, flag, 0644));
    if (fildes < 0) {
      int e(errno);
      throw (exceptions::retval(e) << "file::open: " << strerror(e));
    }
    close();
    _fd = fildes;
  }
  else
    throw (exceptions::retval(EACCES) << "file::open: tried to open NULL file");
  return ;
}
