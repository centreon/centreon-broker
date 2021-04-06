/*
** Copyright 2011-2013, 2021 Centreon
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

#ifndef CCB_MODULES_HANDLE_HH
#define CCB_MODULES_HANDLE_HH

#include <dlfcn.h>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace modules {
/**
 *  @class handle handle.hh "com/centreon/broker/modules/handle.hh"
 *  @brief Plugin library handle.
 *
 *  Centreon Broker can load plugins. This class represents such
 *  plugins.
 */
class handle {
  const std::string _filename;
  void* _handle;

  void _init(const void* arg = nullptr);
  void _check_version();
  void _close();

 public:
  handle(const std::string& filename, const void* arg = nullptr);
  ~handle() noexcept;
  handle() = delete;
  handle(const handle&) = delete;
  handle& operator=(const handle&) = delete;

  bool is_open() const;
  void update(const void* arg = nullptr);

  static char const* deinitialization;
  static char const* initialization;
  static char const* updatization;
  static char const* versionning;
};
}  // namespace modules

CCB_END()

#endif  // !CCB_MODULES_HANDLE_HH
