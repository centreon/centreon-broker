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
#include <fcntl.h>
#include "exception.h"
#include "io/file.h"

using namespace CentreonBroker::IO;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  FileStream constructor.
 *
 *  \param[in] filename Name of the file to open.
 *  \param[in] mode     Mode in which file should be opened.
 */
FileStream::FileStream(const std::string& filename, FileStream::Mode mode)
  : FDStream(-1)
{
  int flags;

  // Extract open flags.
  if (mode == READ)
    flags = O_RDONLY;
  else if (mode == WRITE)
    flags = O_WRONLY;
  else if (mode == READWRITE)
    flags = O_RDWR;
  else
    flags = 0;

  // Open the file.
  this->fd_ = open(filename.c_str(), flags);
  if (this->fd_ < 0)
    throw (CentreonBroker::Exception(errno, strerror(errno)));
}

/**
 *  \brief FileStream copy constructor.
 *
 *  Duplicate the file given as a parameter and obtain a new object
 *  manipulating the same file.
 *
 *  \param[in] file_stream Object to duplicate.
 */
FileStream::FileStream(const FileStream& file_stream) : FDStream(file_stream)
{}

/**
 *  \brief FileStream destructor.
 *
 *  Close the file if it is open.
 */
FileStream::~FileStream() throw () {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Duplicate the file given as a parameter and obtain a new object
 *  manipulating the same file.
 *
 *  \param[in] file_stream Object to duplicate.
 *
 *  \return *this
 */
FileStream& FileStream::operator=(const FileStream& file_stream)
{
  this->FDStream::operator=(file_stream);
  return (*this);
}
