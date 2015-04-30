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

#ifndef CC_LIBRARY_POSIX_HH
#  define CC_LIBRARY_POSIX_HH

#  include <dlfcn.h>
#  include <string>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class library library.hh "com/centreon/library.hh"
 *  @brief Wrapper of libc's library loader.
 *
 *  Wrap standard library loader objects.
 */
class                library {
public:
                     library(std::string const& filename);
                     ~library() throw ();
  std::string const& filename() const throw ();
  bool               is_loaded() const throw ();
  void               load();
  void*              resolve(char const* symbol);
  void*              resolve(std::string const& symbol);
  void (*            resolve_proc(char const* symbol))();
  void (*            resolve_proc(std::string const& symbol))();
  void               unload();

private:
                     library(library const& right);
  library&           operator=(library const& right);

  std::string        _filename;
  void*              _handle;
};

CC_END()

#endif // !CC_LIBRARY_POSIX_HH
