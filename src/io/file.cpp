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
#include <fcntl.h>
#include <string.h>    // for strerror
#include "exception.h"
#include "io/file.h"

using namespace IO;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  File default constructor.
 */
File::File() {}

/**
 *  File copy constructor.
 *
 *  \param[in] file File to duplicate.
 *
 *  \see FD::FD(const FD&)
 */
File::File(const File& file) : FD(file) {}

/**
 *  File destructor.
 */
File::~File() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] file File to duplicate.
 *
 *  \return *this
 *
 *  \see FD::operator=(const FD&)
 */
File& File::operator=(const File& file)
{
  this->FD::operator=(file);
  return (*this);
}

/**
 *  \brief Open a file.
 *
 *  Open a file with the modes described in flags.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] filename Name of the file to open.
 *  \param[in] flags    Mode on which flag should be opened.
 *
 *  \throw Exception Could not open file.
 */
void File::Open(const char* filename, int flags)
{
  if (filename)
    {
      int fildes;

      // O_RDWR is not guaranteed to be equal to O_RDONLY|O_WRONLY.
      if (flags & (READ | WRITE))
	flags = ((flags & ~(READ | WRITE)) | O_RDWR);

      fildes = open(filename, flags, 0644);
      if (fildes < 0)
	throw (Exception(errno, strerror(errno)));
      this->Close();
      this->fd = fildes;
    }
  else
    throw (Exception(0, "Tried to open NULL file."));
  return ;
}
