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

#ifndef IO_FILE_H_
# define IO_FILE_H_

# include <fcntl.h>
# include "io/fd.h"

namespace IO
{
  /**
   *  \class File file.h "io/file.h"
   *  \brief Wraps a file access.
   *
   *  The File class can open a file and perform I/O operations on top of it.
   *
   *  \see FD
   */
  class   File : public FD
  {
   public:
    enum  Flags
    {
      CREATE = O_CREAT,
      READ = O_RDONLY,
      TRUNCATE = O_TRUNC,
      WRITE = O_WRONLY
    };
          File();
          File(const File& file);
          ~File();
    File& operator=(const File& file);
    void  Open(const char* filename, int flags = READ);
  };
}

#endif /* !IO_FILE_H_ */
