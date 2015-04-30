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

#ifndef CC_CLIB_HH
#  define CC_CLIB_HH

#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class clib clib.hh "com/centreon/clib.hh"
 *  @brief Initialize all clib manager.
 *
 *  Initialize all clib manager and make base configuration.
 */
class         clib {
public:
  enum        load_flags {
    with_logging_engine = 1 << 0,
    with_process_manager = 1 << 1
  };

  static void load(unsigned int flags = with_logging_engine
                                        | with_process_manager);
  static void unload();

private:
              clib(unsigned int flags);
              clib(clib const& right);
              ~clib() throw ();
  clib&       operator=(clib const& right);
};

CC_END()

#endif // !CC_CLIB_HH
