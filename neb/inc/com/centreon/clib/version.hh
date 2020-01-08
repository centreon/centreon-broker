/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CC_CLIB_VERSION_HH
# define CC_CLIB_VERSION_HH

// Compile-time values.
# define CENTREON_CLIB_VERSION_MAJOR  20
# define CENTREON_CLIB_VERSION_MINOR  04
# define CENTREON_CLIB_VERSION_PATCH  0
# define CENTREON_CLIB_VERSION_STRING "20.04.0"

# include "com/centreon/namespace.hh"

CC_BEGIN()

namespace              clib {
  namespace            version {
    // Compile-time values.
    unsigned int const major = 20;
    unsigned int const minor = 04;
    unsigned int const patch = 0;
    char const* const  string = "20.04.0";

    // Run-time values.
    unsigned int       get_major() throw ();
    unsigned int       get_minor() throw ();
    unsigned int       get_patch() throw ();
    char const*        get_string() throw ();
  }
}

CC_END()

#endif // !CC_HANDLE_HH
