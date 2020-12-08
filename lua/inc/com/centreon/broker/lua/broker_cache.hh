/*
** Copyright 2018 Centreon
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

#ifndef CCB_LUA_BROKER_CACHE_HH
#define CCB_LUA_BROKER_CACHE_HH

#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/namespace.hh"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

CCB_BEGIN()

namespace lua {
/**
 *  @class broker_cache broker_cache.hh
 * "com/centreon/broker/lua/broker_cache.hh"
 *  @brief Class providing cache access to the lua interpreter
 *
 *  This class provides a binding to Lua to access cache.
 */
class broker_cache {
 public:
  static void broker_cache_reg(lua_State* L,
                               macro_cache const& cache,
                               uint32_t api_version);
};
}  // namespace lua

CCB_END()

#endif  // !CCB_LUA_BROKER_CACHE_HH
