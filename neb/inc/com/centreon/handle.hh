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

#ifndef CC_HANDLE_HH
#define CC_HANDLE_HH

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32
#include "com/centreon/namespace.hh"

CC_BEGIN()

#ifdef _WIN32
typedef HANDLE native_handle;
native_handle const native_handle_null = NULL;
#else
typedef int native_handle;
native_handle const native_handle_null = -1;
#endif  // _WIN32

/**
 *  @class handle handle.hh "com/centreon/handle.hh"
 *  @brief Base for all handle objetcs.
 *
 *  This class is an interface for system handle.
 */
class handle {
 public:
  handle();
  handle(handle const& right);
  virtual ~handle() throw();
  handle& operator=(handle const& right);
  virtual void close() = 0;
  virtual native_handle get_native_handle() = 0;
  virtual unsigned long read(void* data, unsigned long size) = 0;
  virtual unsigned long write(void const* data, unsigned long size) = 0;
};

CC_END()

#endif  // !CC_HANDLE_HH
