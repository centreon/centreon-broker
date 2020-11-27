//https://ericscrivner.me/2017/10/c-using-shared_ptr-lua-userdata/

/*
** Copyright 2020 Centreon
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

#include "com/centreon/broker/lua/broker_event.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

/**
 *  The Lua broker_event constructor
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
int l_broker_event_new(lua_State* L) {
  void* userdata = lua_newuserdata(L, sizeof(std::shared_ptr<io::data>));
  if (!userdata)
    return 0;

  new(userdata) std::shared_ptr<io::data>();

  luaL_getmetatable(L, "broker_event");
  lua_setmetatable(L, -2);

  return 1;
}

/**
 *  The Lua broker_event destructor
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_event_destructor(lua_State* L) {
  void* ptr = luaL_checkudata(L, 1, "broker_event");

  if (ptr) {
    auto event = static_cast<std::shared_ptr<io::data>*>(ptr);
    event->reset();
    delete event;
  }
  return 0;
}

static int l_broker_event_get(lua_State* L) {
  std::shared_ptr<io::data> e{*static_cast<std::shared_ptr<io::data>*>(luaL_checkudata(L, 1, "broker_event"))};
  const char* key = luaL_checkstring(L, 2);

  io::event_info const* info = io::events::instance().get_event_info(e->type());
  if (info) {
    for (const mapping::entry* current_entry = info->get_mapping();
         !current_entry->is_null();
         ++current_entry) {
      char const* entry_name(current_entry->get_name_v2());
      if (entry_name && strcmp(entry_name, key) == 0) {
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            lua_pushboolean(_L, current_entry->get_bool(d));
            break;
          case mapping::source::DOUBLE:
            lua_pushnumber(_L, current_entry->get_double(d));
            break;
          case mapping::source::INT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                int val(current_entry->get_int(d));
                if (val == 0)
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                int val(current_entry->get_int(d));
                if (val == -1)
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              default:
                lua_pushinteger(_L, current_entry->get_int(d));
            }
            break;
          case mapping::source::SHORT:
            lua_pushinteger(_L, current_entry->get_short(d));
            break;
          case mapping::source::STRING:
            if (current_entry->get_attribute() ==
                mapping::entry::invalid_on_zero) {
              std::string val{current_entry->get_string(d)};
              if (val.empty())
                lua_pushnil(_L);
              else
                lua_pushstring(_L, val.c_str());
            } else
              lua_pushstring(_L, current_entry->get_string(d).c_str());
            break;
          case mapping::source::TIME:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                time_t val = current_entry->get_time(d);
                if (val == 0)
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                time_t val = current_entry->get_time(d);
                if (val == -1)
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              default:
                lua_pushinteger(_L, current_entry->get_time(d));
            }
            break;
          case mapping::source::UINT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                uint32_t val = current_entry->get_uint(d);
                if (val == 0)
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                uint32_t val = current_entry->get_uint(d);
                if (val == static_cast<uint32_t>(-1))
                  lua_pushnil(_L);
                else
                  lua_pushinteger(_L, val);
              } break;
              default:
                lua_pushinteger(_L, current_entry->get_uint(d));
            }
            break;
          default:  // Error in one of the mappings.
            throw exceptions::msg() << "invalid mapping for object "
                                    << "of type '" << info->get_name()
                                    << "': " << current_entry->get_type()
                                    << " is not a known type ID";
        }
        return 1;
      }
    }
  } else
    throw exceptions::msg() << "cannot bind object of type " << e->type()
                            << " to database query: mapping does not exist";
  return 0;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_socket::broker_socket_reg(lua_State* L) {
  luaL_Reg s_broker_event_regs[] = {{"new", l_broker_event_constructor},
                                     {"__gc", l_broker_event_destructor},
                                     {"get", l_broker_event_get},
                                     {nullptr, nullptr}};

  const char* name = "broker_event";
  lua_newtable(L);
  luaL_newmetatable(L, name);

  // Register the C functions into the metatable we just created.
#ifdef LUA51
  luaL_register(L, NULL, s_broker_event_regs);
#else
  luaL_setfuncs(L, s_broker_event_regs, 0);
#endif

  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  lua_setglobal(L, name);
}
