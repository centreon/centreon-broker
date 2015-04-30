/*
** Copyright 2012-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_IO_FILE_ENTRY_HH
#  define CC_IO_FILE_ENTRY_HH

#  include <string>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include "com/centreon/handle.hh"
#  include "com/centreon/namespace.hh"

#  ifdef _WIN32
#    define stat _stat
#  endif // _WIN32

CC_BEGIN()

namespace              io {
  /**
   *  @class file_entry file_entry.hh "com/centreon/io/file_entry.hh"
   *  @brief Wrapper of stat information.
   *
   *  Wrap standard stat information.
   */
  class                file_entry {
  public:
                       file_entry(char const* path = NULL);
                       file_entry(std::string const& path);
                       file_entry(file_entry const& right);
                       ~file_entry() throw ();
    file_entry&        operator=(file_entry const& right);
    bool               operator==(file_entry const& right) const throw ();
    bool               operator!=(file_entry const& right) const throw ();
    std::string        base_name() const;
    std::string        directory_name() const;
    std::string        file_name() const;
    bool               is_directory() const throw ();
    bool               is_link() const throw ();
    bool               is_regular() const throw ();
    std::string const& path() const throw ();
    void               path(char const* path);
    void               path(std::string const& path);
    void               refresh();
    unsigned long long size() const throw ();

  private:
    void               _internal_copy(file_entry const& right);

    std::string        _path;
    struct stat        _sbuf;
  };
}

CC_END()

#endif // !CC_IO_FILE_ENTRY_HH
