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

#include "com/centreon/broker/lua/broker_log.hh"
#include <fstream>
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

/**
 *  The broker_log destructor
 *
 *  @param L The Lua interpreter
 *
 *  @return An integer
 */
static int l_broker_log_destructor(lua_State* L) {
  delete *static_cast<broker_log**>(luaL_checkudata(L, 1, "lua_broker_log"));
  return 0;
}

/**
 *  The broker_log set_parameters method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_log_set_parameters(lua_State* L) {
  broker_log* bl(
      *static_cast<broker_log**>(luaL_checkudata(L, 1, "lua_broker_log")));
  int level(lua_tointeger(L, 2));
  char const* text(lua_tostring(L, 3));
  if (level < 0 || level > 3)
    luaL_error(L,
               "set_parameters expects the first argument to be 0, 1, 2 or 3.");

  bl->set_parameters(level, text);
  return 0;
}

int _log_func(logging::logger& log_func, lua_State* L, const char* header) {
  broker_log* bl(
      *static_cast<broker_log**>(luaL_checkudata(L, 1, "lua_broker_log")));
  int level(lua_tointeger(L, 2));
  char const* text(lua_tostring(L, 3));
  if (level <= bl->get_level()) {
    if (bl->get_file().empty())
      log_func(static_cast<logging::level>(level)) << "lua: " << text;
    else {
      std::ofstream of;
      of.open(bl->get_file().c_str(), std::ios_base::app);
      if (of.fail())
        logging::error(logging::medium)
            << "Unable to open the log file '" << bl->get_file() << "'";
      else {
        time_t now(time(nullptr));
        struct tm tmp;
        localtime_r(&now, &tmp);
        char buf[80];
        strftime(buf, sizeof(buf), "%c: ", &tmp);
        of << buf << header << text << std::endl;
      }
    }
  }
  return 0;
}

/**
 *  The broker_log info method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_log_info(lua_State* L) {
  return _log_func(logging::info, L, "INFO: ");
}

/**
 *  The broker_log error method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_log_error(lua_State* L) {
  return _log_func(logging::error, L, "ERROR: ");
}

/**
 *  The broker_log warning method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_log_warning(lua_State* L) {
  return _log_func(logging::error, L, "WARNING: ");
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_log::broker_log_reg(lua_State* L) {
  broker_log** udata(
      static_cast<broker_log**>(lua_newuserdata(L, sizeof(broker_log*))));
  *udata = new broker_log();

  luaL_Reg s_broker_log_regs[] = {
      {"__gc", l_broker_log_destructor},
      {"set_parameters", l_broker_log_set_parameters},
      {"info", l_broker_log_info},
      {"error", l_broker_log_error},
      {"warning", l_broker_log_warning},
      {nullptr, nullptr}};

  // Create a metatable. It is not exposed to Lua. It is not
  // exposed to Lua. The "lua_broker" label is used by Lua
  // internally to identify things.
  luaL_newmetatable(L, "lua_broker_log");

  // Register the C functions into the metatable we just created.
#ifdef LUA51
  luaL_register(L, NULL, s_broker_log_regs);
#else
  luaL_setfuncs(L, s_broker_log_regs, 0);
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
  lua_setglobal(L, "broker_log");
}

/**
 *  broker_log constructor
 *
 */
broker_log::broker_log() {}

/**
 *  The internal set_parameters method called when it is called from the Lua
 *
 *  @param level The level, 1 for a high level, 3 for a low level.
 *  @param log_file The file containing the logs.
 */
void broker_log::set_parameters(int level, std::string const& log_file) {
  _level = level;
  _file = log_file;
}

/**
 *  Accessor to the max level
 *
 *  @return the max level.
 */
int broker_log::get_level() const {
  return _level;
}

/**
 *  Accessor to the log file
 *
 *  @return a string containing the file name.
 */
std::string const& broker_log::get_file() const {
  return _file;
}
