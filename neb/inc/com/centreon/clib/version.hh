/*
** Copyright 2011-2013 Merethis
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

#ifndef CC_CLIB_VERSION_HH
# define CC_CLIB_VERSION_HH

// Compile-time values.
# define CENTREON_CLIB_VERSION_MAJOR  1
# define CENTREON_CLIB_VERSION_MINOR  4
# define CENTREON_CLIB_VERSION_PATCH  2
# define CENTREON_CLIB_VERSION_STRING "1.4.2"

# include "com/centreon/namespace.hh"

CC_BEGIN()

namespace              clib {
  namespace            version {
    // Compile-time values.
    unsigned int const major = 1;
    unsigned int const minor = 4;
    unsigned int const patch = 2;
    char const* const  string = "1.4.2";

    // Run-time values.
    unsigned int       get_major() throw ();
    unsigned int       get_minor() throw ();
    unsigned int       get_patch() throw ();
    char const*        get_string() throw ();
  }
}

CC_END()

#endif // !CC_HANDLE_HH
