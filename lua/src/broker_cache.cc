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

#include "com/centreon/broker/lua/broker_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

/**
 *  broker_cache destructor
 *
 *  @param L The Lua interpreter
 *
 *  @return an integer, here always 0.
 */
static int l_broker_cache_destructor(lua_State* L) {
  (void)L;
  return (0);
}

/**
 *  The get_hostname() method available in the Lua interpreter
 *
 *  @param L The Lua interpreter
 *
 *  @return an integer
 */
static int l_broker_cache_get_hostname(lua_State* L) {
  macro_cache const* cache(
    *static_cast<macro_cache**>(luaL_checkudata(L, 1, "lua_broker_cache")));
  int id(luaL_checkinteger(L, 2));

  try {
    QString hst(cache->get_host_name(id));
    lua_pushstring(L, hst.toStdString().c_str());
  }
  catch (std::exception const& e) {
    (void) e;
    lua_pushnil(L);
  }
  return 1;
}

/**
 *  The get_service_description() method available in the Lua interpreter
 *
 *  @param L The Lua interpreter
 *
 *  @return an integer
 */
static int l_broker_cache_get_service_description(lua_State* L) {
  macro_cache const* cache(
    *static_cast<macro_cache**>(luaL_checkudata(L, 1, "lua_broker_cache")));
  int host_id(luaL_checkinteger(L, 2));
  int service_id(luaL_checkinteger(L, 3));

  try {
    QString svc(cache->get_service_description(host_id, service_id));
    lua_pushstring(L, svc.toStdString().c_str());
  }
  catch (std::exception const& e) {
    (void) e;
    lua_pushnil(L);
  }
  return 1;
}

/**
 *  The get_index_mapping() method available in the Lua interpreter
 *
 *  @param L The Lua interpreter
 *
 *  @return a table with three keys: index_id, host_id and service_id.
 */
static int l_broker_cache_get_index_mapping(lua_State* L) {
  macro_cache const* cache(
    *static_cast<macro_cache**>(luaL_checkudata(L, 1, "lua_broker_cache")));
  int index_id(luaL_checkinteger(L, 2));

  try {
    storage::index_mapping const& mapping(cache->get_index_mapping(index_id));
    lua_createtable(L, 0, 2);

    lua_pushstring(L, "index_id");
    lua_pushinteger(L, mapping.index_id);
    lua_settable(L, -3);

    lua_pushstring(L, "host_id");
    lua_pushinteger(L, mapping.host_id);
    lua_settable(L, -3);

    lua_pushstring(L, "service_id");
    lua_pushinteger(L, mapping.service_id);
    lua_settable(L, -3);
  }
  catch (std::exception const& e) {
    (void) e;
    lua_pushnil(L);
  }
  return 1;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @param L The Lua interpreter
 *  @param cache The cache to share with the Lua interpreter
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_cache::broker_cache_reg(lua_State* L, macro_cache const& cache) {
  macro_cache const** udata(
    static_cast<macro_cache const**>(lua_newuserdata(L, sizeof(macro_cache*))));
  *udata = &cache;

  luaL_Reg s_broker_cache_regs[] = {
    { "__gc", l_broker_cache_destructor },
    { "get_hostname", l_broker_cache_get_hostname },
    { "get_service_description", l_broker_cache_get_service_description },
    { "get_index_mapping", l_broker_cache_get_index_mapping },
    { NULL, NULL }
  };

  // Create a metatable. It is not exposed to Lua.
  // The "lua_broker" label is used by Lua internally to identify things.
  luaL_newmetatable(L, "lua_broker_cache");

  // Register the C functions into the metatable we just created.
#ifdef LUA51
  luaL_register(L, NULL, s_broker_cache_regs);
#else
  luaL_setfuncs(L, s_broker_cache_regs, 0);
#endif

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -2
  // 2  =>  metatable "lua_broker"    => -1
  lua_pushvalue(L, -1);

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -3
  // 2  =>  metatable "lua_broker"    => -2
  // 3  =>  metatable "lua_broker"    => -1

  // Set the __index field of the metatable to point to itself
  lua_setfield(L, -1, "__index");

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -2
  // 2  =>  metatable "lua_broker"    => -1

  // Now, we use setmetatable to set it to our userdata
  lua_setmetatable(L, -2);

  // And now, we use setglobal to store userdata as the variable "broker".
  lua_setglobal(L, "broker_cache");
}
