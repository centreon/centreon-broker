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

#ifndef CC_HANDLE_MANAGER_HH
#  define CC_HANDLE_MANAGER_HH

#  ifdef _WIN32
#    include "com/centreon/handle_manager_win32.hh"
#  else
#    include "com/centreon/handle_manager_posix.hh"
#  endif // Windows or POSIX implementation.

#endif // !CC_HANDLE_MANAGER_HH
