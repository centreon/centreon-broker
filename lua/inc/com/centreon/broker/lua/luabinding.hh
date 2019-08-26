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

#ifndef CCB_LUA_LUABINDING_HH
#  define CCB_LUA_LUABINDING_HH

#  include <map>
#  include "com/centreon/broker/misc/variant.hh"
#  include "com/centreon/broker/lua/macro_cache.hh"

extern "C" {
#  include "lua.h"
#  include "lauxlib.h"
#  include "lualib.h"
}

CCB_BEGIN()

namespace               lua {
  /**
   *  @class luabinding luabinding.hh "com/centreon/broker/luabinding/luabinding.hh"
   *  @brief Class managing exchange with the the lua interpreter.
   *
   *  Expose an api to simplify exchanges with the Lua interpreter.
   */
  class                 luabinding {
   public:
                        luabinding(
                          std::string const& lua_script,
                          std::map<std::string, misc::variant> const& conf_params,
                          macro_cache const& cache);
                        ~luabinding();
    bool                has_filter() const;
    int                 write(std::shared_ptr<io::data> const& data);

   private:
                        luabinding(luabinding const& other);
    luabinding&         operator=(luabinding const& other);
    lua_State*          _load_interpreter();
    void                _load_script();
    void                _init_script(
                          std::map<std::string, misc::variant> const& conf_params);
    void                _update_lua_path(std::string const& path);

    // Event conversion to Lua table.
    void                _parse_entries(io::data const& d);

    // The Lua state machine.
    lua_State*          _L;

    // True if there is a filter() function in the Lua script.
    bool                _filter;

    // The Lua script name.
    std::string const&  _lua_script;

    // The cache.
    macro_cache const&  _cache;

    // Count on events
    int                 _total;
  };
}

CCB_END()

#endif // !CCB_LUA_LUA_HH
