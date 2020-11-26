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

#include <cstring>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

/**
 *  The Lua broker_event constructor
 *
 *  @param L The Lua interpreter
 *  @param e An event to wrap
 *
 */
void broker_event::create(lua_State* L, std::shared_ptr<io::data> e) {
  void* userdata = lua_newuserdata(L, sizeof(std::shared_ptr<io::data>));
  if (!userdata)
    throw exceptions::msg() << "Unable to build a lua broker_event";

  new (userdata) std::shared_ptr<io::data>(e);

  luaL_getmetatable(L, "broker_event");
  lua_setmetatable(L, -2);
}

/**
 *  Given an event d, this method converts it to a Lua table.
 *  The result is stored on the Lua interpreter stack. This internal function
 *  is essentially needed by the broker api version 1. We do not transpose
 *  events to table in api v2.
 *
 *  @param L The Lua interpreter.
 *  @param d The event to convert.
 */
void broker_event::create_as_table(lua_State* L, const io::data& d) {
  uint32_t type(d.type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));
  lua_newtable(L);
  lua_pushstring(L, "_type");
  lua_pushinteger(L, type);
  lua_rawset(L, -3);

  lua_pushstring(L, "category");
  lua_pushinteger(L, cat);
  lua_rawset(L, -3);

  lua_pushstring(L, "element");
  lua_pushinteger(L, elem);
  lua_rawset(L, -3);
  io::event_info const* info(io::events::instance().get_event_info(d.type()));
  if (info) {
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null(); ++current_entry) {
      char const* entry_name(current_entry->get_name_v2());
      if (entry_name && entry_name[0]) {
        lua_pushstring(L, entry_name);
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            lua_pushboolean(L, current_entry->get_bool(d));
            break;
          case mapping::source::DOUBLE:
            lua_pushnumber(L, current_entry->get_double(d));
            break;
          case mapping::source::INT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                int val(current_entry->get_int(d));
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                int val(current_entry->get_int(d));
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_int(d));
            }
            break;
          case mapping::source::SHORT:
            lua_pushinteger(L, current_entry->get_short(d));
            break;
          case mapping::source::STRING:
            if (current_entry->get_attribute() ==
                mapping::entry::invalid_on_zero) {
              std::string val{current_entry->get_string(d)};
              if (val.empty())
                lua_pushnil(L);
              else
                lua_pushstring(L, val.c_str());
            } else
              lua_pushstring(L, current_entry->get_string(d).c_str());
            break;
          case mapping::source::TIME:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                time_t val = current_entry->get_time(d);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                time_t val = current_entry->get_time(d);
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_time(d));
            }
            break;
          case mapping::source::UINT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                uint32_t val = current_entry->get_uint(d);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                uint32_t val = current_entry->get_uint(d);
                if (val == static_cast<uint32_t>(-1))
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_uint(d));
            }
            break;
          default:  // Error in one of the mappings.
            throw exceptions::msg() << "invalid mapping for object "
                                    << "of type '" << info->get_name()
                                    << "': " << current_entry->get_type()
                                    << " is not a known type ID";
        }
        lua_rawset(L, -3);
      }
    }
  } else
    throw exceptions::msg() << "cannot bind object of type " << d.type()
                            << " to database query: mapping does not exist";
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
  }
  return 0;
}

/**
 * @brief The pairs() Lua function needed for iteration on Broker event content.
 *
 * @param L The Lua interpreter. We need to access its stack where our element
 * is.
 *
 * @return 3 (The number of element on the stack to return: the __next internal
 * function, the broker event and nil).
 */
static int l_broker_event_pairs(lua_State* L) {
  std::shared_ptr<io::data> e{*static_cast<std::shared_ptr<io::data>*>(
      luaL_checkudata(L, 1, "broker_event"))};
  luaL_getmetafield(L, 1, "__next");
  lua_insert(L, 1);
  lua_pushnil(L);
  return 3;
}

/**
 * @brief The next() internal function needed for iteration on Broker event
 * content.
 *
 * @param L The Lua interpreter. We need to access its stack.
 *
 * @return Usually 2 (the number of elements when the function exits. Elements
 * are a key and its value.) or 1 when the iteration is over (the returned
 * element is nil).
 */
static int l_broker_event_next(lua_State* L) {
  std::shared_ptr<io::data> e{*static_cast<std::shared_ptr<io::data>*>(
      luaL_checkudata(L, 1, "broker_event"))};
  size_t keyl;
  const char* key = lua_tolstring(L, 2, &keyl);

  if (key == nullptr) {
    lua_pushstring(L, "_type");
    lua_pushinteger(L, e->type());
    return 2;
  } else if (strcmp(key, "_type") == 0) {
    lua_pushstring(L, "category");
    lua_pushinteger(L, static_cast<uint32_t>(e->type()) >> 16);
    return 2;
  } else if (strcmp(key, "category") == 0) {
    lua_pushstring(L, "element");
    lua_pushinteger(L, e->type() & 0xffff);
    return 2;
  }

  io::event_info const* info = io::events::instance().get_event_info(e->type());
  if (info) {
    bool found = false;
    for (const mapping::entry* current_entry = info->get_mapping();
         !current_entry->is_null(); ++current_entry) {
      char const* entry_name(current_entry->get_name_v2());
      if (!entry_name || *entry_name == 0)
        continue;
      if (strcmp(key, "element") == 0)
        found = true;
      else if (!found && strcmp(entry_name, key) == 0) {
        found = true;
        continue;
      }
      if (found) {
        lua_pushstring(L, entry_name);
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            lua_pushboolean(L, current_entry->get_bool(*e));
            break;
          case mapping::source::DOUBLE:
            lua_pushnumber(L, current_entry->get_double(*e));
            break;
          case mapping::source::INT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                int val(current_entry->get_int(*e));
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                int val(current_entry->get_int(*e));
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_int(*e));
            }
            break;
          case mapping::source::SHORT:
            lua_pushinteger(L, current_entry->get_short(*e));
            break;
          case mapping::source::STRING:
            if (current_entry->get_attribute() ==
                mapping::entry::invalid_on_zero) {
              std::string val{current_entry->get_string(*e)};
              if (val.empty())
                lua_pushnil(L);
              else
                lua_pushstring(L, val.c_str());
            } else
              lua_pushstring(L, current_entry->get_string(*e).c_str());
            break;
          case mapping::source::TIME:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                time_t val = current_entry->get_time(*e);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                time_t val = current_entry->get_time(*e);
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_time(*e));
            }
            break;
          case mapping::source::UINT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                uint32_t val = current_entry->get_uint(*e);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                uint32_t val = current_entry->get_uint(*e);
                if (val == static_cast<uint32_t>(-1))
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_uint(*e));
            }
            break;
          default:  // Error in one of the mappings.
            lua_pushnil(L);
        }
        return 2;
      }
    }
    lua_pushnil(L);
    return 1;
  } else
    throw exceptions::msg() << "unable to parse object of type " << e->type()
                            << " ; it does not look like a BBDO event";
}

/**
 * @brief This function is the implementation of indexation on a broker event.
 * It is useful to be able to get for example the name of a host, we enter
 * something like host.name
 *
 * @param L The Lua interpreter to access its stack.
 *
 * @return 1 (the number of element returned by this function which is just
 * a value).
 */
static int l_broker_event_index(lua_State* L) {
  std::shared_ptr<io::data> e{*static_cast<std::shared_ptr<io::data>*>(
      luaL_checkudata(L, 1, "broker_event"))};
  const char* key = luaL_checkstring(L, 2);

  if (strcmp(key, "_type") == 0) {
    lua_pushinteger(L, e->type());
    return 1;
  } else if (strcmp(key, "category") == 0) {
    lua_pushinteger(L, (static_cast<uint32_t>(e->type()) >> 16));
    return 1;
  } else if (strcmp(key, "element") == 0) {
    lua_pushinteger(L, e->type() & 0xffff);
    return 1;
  }

  io::event_info const* info = io::events::instance().get_event_info(e->type());
  if (info) {
    for (const mapping::entry* current_entry = info->get_mapping();
         !current_entry->is_null(); ++current_entry) {
      char const* entry_name(current_entry->get_name_v2());
      if (entry_name && strcmp(entry_name, key) == 0) {
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            lua_pushboolean(L, current_entry->get_bool(*e));
            break;
          case mapping::source::DOUBLE:
            lua_pushnumber(L, current_entry->get_double(*e));
            break;
          case mapping::source::INT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                int val(current_entry->get_int(*e));
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                int val(current_entry->get_int(*e));
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_int(*e));
            }
            break;
          case mapping::source::SHORT:
            lua_pushinteger(L, current_entry->get_short(*e));
            break;
          case mapping::source::STRING:
            if (current_entry->get_attribute() ==
                mapping::entry::invalid_on_zero) {
              std::string val{current_entry->get_string(*e)};
              if (val.empty())
                lua_pushnil(L);
              else
                lua_pushstring(L, val.c_str());
            } else
              lua_pushstring(L, current_entry->get_string(*e).c_str());
            break;
          case mapping::source::TIME:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                time_t val = current_entry->get_time(*e);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                time_t val = current_entry->get_time(*e);
                if (val == -1)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_time(*e));
            }
            break;
          case mapping::source::UINT:
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero: {
                uint32_t val = current_entry->get_uint(*e);
                if (val == 0)
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              case mapping::entry::invalid_on_minus_one: {
                uint32_t val = current_entry->get_uint(*e);
                if (val == static_cast<uint32_t>(-1))
                  lua_pushnil(L);
                else
                  lua_pushinteger(L, val);
              } break;
              default:
                lua_pushinteger(L, current_entry->get_uint(*e));
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
                            << " to lua userdata: mapping does not exist";
  return 0;
}

/**
 *  Register the broker_event element in the Lua interpreter.
 *
 *  @param The Lua interpreter as a lua_State*
 */
void broker_event::broker_event_reg(lua_State* L) {
  luaL_Reg s_broker_event_regs[] = {{"__gc", l_broker_event_destructor},
                                    {"__index", l_broker_event_index},
                                    {"__next", l_broker_event_next},
                                    {"__pairs", l_broker_event_pairs},
                                    {nullptr, nullptr}};

  const char* name = "broker_event";
  luaL_newmetatable(L, name);

  // Register the C functions into the metatable we just created.
#ifdef LUA51
  luaL_register(L, NULL, s_broker_event_regs);
#else
  luaL_setfuncs(L, s_broker_event_regs, 0);
#endif

  lua_setglobal(L, name);
}
