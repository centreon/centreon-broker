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

#ifndef CC_HANDLE_HH
#  define CC_HANDLE_HH

#  ifdef _WIN32
#    include <windows.h>
#  endif // _WIN32
#  include "com/centreon/namespace.hh"

CC_BEGIN()

#  ifdef _WIN32
typedef HANDLE native_handle;
native_handle const native_handle_null = NULL;
#  else
typedef int native_handle;
native_handle const native_handle_null = -1;
#  endif // _WIN32

/**
 *  @class handle handle.hh "com/centreon/handle.hh"
 *  @brief Base for all handle objetcs.
 *
 *  This class is an interface for system handle.
 */
class                   handle {
public:
                        handle();
                        handle(handle const& right);
  virtual               ~handle() throw ();
  handle&               operator=(handle const& right);
  virtual void          close() = 0;
  virtual native_handle get_native_handle() = 0;
  virtual unsigned long read(void* data, unsigned long size) = 0;
  virtual unsigned long write(void const* data, unsigned long size) = 0;
};

CC_END()

#endif // !CC_HANDLE_HH
