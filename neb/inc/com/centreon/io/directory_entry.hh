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

#ifndef CC_IO_DIRECTORY_ENTRY_HH
#  define CC_IO_DIRECTORY_ENTRY_HH

#  include <list>
#  include <string>
#  include "com/centreon/io/file_entry.hh"
#  include "com/centreon/handle.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace                 io {
  /**
   *  @class directory_entry directory_entry.hh "com/centreon/io/directory_entry.hh"
   *  @brief Wrapper of libc's directory_entryectory.
   *
   *  Wrap standard directory_entryectory objects.
   */
  class                   directory_entry {
  public:
                          directory_entry(char const* path = NULL);
                          directory_entry(std::string const& path);
                          directory_entry(directory_entry const& right);
    directory_entry&      operator=(directory_entry const& right);
    bool                  operator==(directory_entry const& right) const throw ();
    bool                  operator!=(directory_entry const& right) const throw ();
                          ~directory_entry() throw ();
    static std::string    current_path();
    file_entry const&     entry() const throw ();
    std::list<file_entry> const&
                          entry_list(std::string const& filter = "");

  private:
    void                  _internal_copy(directory_entry const& right);
    static int            _nmatch(char const* str, char const* pattern);

    file_entry            _entry;
    std::list<file_entry> _entry_lst;
  };
}

CC_END()

#endif // !CC_IO_DIRECTORY_ENTRY_HH
