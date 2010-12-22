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

#ifndef IO_FILE_HH_
# define IO_FILE_HH_

# include <fcntl.h>
# include "io/fd.hh"

namespace io {
  /**
   *  @class file file.hh "io/file.hh"
   *  @brief Wraps a file access.
   *
   *  The file class can open a file and perform I/O operations on top
   *  of it.
   *
   *  @see fd
   */
  class   file : public fd {
   public:
    enum  flags {
      CREATE = O_CREAT,
      READ = O_RDONLY,
      TRUNCATE = O_TRUNC,
      WRITE = O_WRONLY
    };
          file();
          file(file const& f);
          ~file();
    file& operator=(file const& f);
    void  open(char const* filename, int flag = READ);
  };
}

#endif /* !IO_FILE_HH_ */
