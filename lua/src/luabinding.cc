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

#include <cassert>
#include <fstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/lua/broker_cache.hh"
#include "com/centreon/broker/lua/broker_log.hh"
#include "com/centreon/broker/lua/broker_socket.hh"
#include "com/centreon/broker/lua/broker_utils.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

/**
 *  Constructor.
 *
 *  @param[in] lua_script the json parameters file
 *  @param[in] conf_params A hash table with user parameters
 *  @param[in] cache the persistent cache.
 */
luabinding::luabinding(std::string const& lua_script,
                       std::map<std::string, misc::variant> const& conf_params,
                       macro_cache& cache)
    : _L{nullptr},
      _filter{false},
      _flush{false},
      _lua_script(lua_script),
      _cache(cache),
      _total{0} {
  size_t pos(lua_script.find_last_of('/'));
  std::string path(lua_script.substr(0, pos));
  _L = _load_interpreter();
  _update_lua_path(path);

  logging::debug(logging::medium)
      << "lua: initializing the Lua virtual machine";

  try {
    _load_script();
    _init_script(conf_params);
  }
  catch (std::exception const& e) {
    lua_close(_L);
    throw;
  }
}

/**
 *  Destructor
 */
luabinding::~luabinding() {
  if (_L)
    lua_close(_L);
}

/**
 *  This function updates the Lua variables package.path and package.cpath so
 *  that a script in that directory can be called without difficulty.
 *
 * @param path A directory to add to the Lua paths
 */
void luabinding::_update_lua_path(std::string const& path) {
  /* Working on path: lua scripts */
  lua_getglobal(_L, "package");
  lua_getfield(_L, -1, "path");
  std::string current_path(lua_tostring(_L, -1));
  current_path.append(";");
  current_path.append(path);
  current_path.append("/?.lua");
  lua_pop(_L, 1);
  lua_pushstring(_L, current_path.c_str());
  lua_setfield(_L, -2, "path");

  /* Working on cpath: so libraries */
  lua_getfield(_L, -1, "cpath");
  current_path = lua_tostring(_L, -1);
  current_path.append(";");
  current_path.append(path);
  current_path.append("/lib/?.so");
  lua_pop(_L, 1);
  lua_pushstring(_L, current_path.c_str());
  lua_setfield(_L, -2, "cpath");
  lua_pop(_L, 1);
}

/**
 *  Returns true if a filter was configured in the Lua script.
 */
bool luabinding::has_filter() const noexcept { return _filter; }

/**
 *  Returns true if a flush was configured in the Lua script.
 */
bool luabinding::has_flush() const noexcept { return _flush; }

/**
 *  Reads the Lua script, checks its syntax and checks if
 *   - init()
 *   - write()
 *   - filter()
 *  functions exist in the Lua script. The two first ones are
 *  mandatory whereas the third one is optional.
 */
void luabinding::_load_script() {
  // script loading
  if (luaL_loadfile(_L, _lua_script.c_str()) != 0) {
    char const* error_msg(lua_tostring(_L, -1));
    throw exceptions::msg() << "lua: '" << _lua_script
                            << "' could not be loaded: " << error_msg;
  }

  // Script compilation
  if (lua_pcall(_L, 0, 0, 0) != 0) {
    throw exceptions::msg() << "lua: '" << _lua_script
                            << "' could not be compiled";
  }

  // Checking for init() availability: this function is mandatory
  lua_getglobal(_L, "init");
  if (!lua_isfunction(_L, lua_gettop(_L)))
    throw exceptions::msg() << "lua: '" << _lua_script
                            << "' init() global function is missing";

  // Checking for write() availability: this function is mandatory
  lua_getglobal(_L, "write");
  if (!lua_isfunction(_L, lua_gettop(_L)))
    throw exceptions::msg() << "lua: '" << _lua_script
                            << "' write() global function is missing";

  // Checking for filter() availability: this function is optional
  lua_getglobal(_L, "filter");
  if (!lua_isfunction(_L, lua_gettop(_L))) {
    logging::debug(logging::medium)
        << "lua: filter() global function is missing, "
        << "the write() function will be called for each event";
    _filter = false;
  } else
    _filter = true;

  // Checking for flush() availability: this function is optional
  lua_getglobal(_L, "flush");
  if (!lua_isfunction(_L, lua_gettop(_L))) {
    logging::debug(logging::medium)
        << "lua: flush() global function is not defined";
    _flush = false;
  } else
    _flush = true;
}

/**
 *  Executes the init() function given in the Lua script
 *  with the content of conf_params as parameter.
 *
 *  @param conf_params A hashtable of data providing various
 *  informations needed by the script to work.
 *
 */
void luabinding::_init_script(
    std::map<std::string, misc::variant> const& conf_params) {
  lua_getglobal(_L, "init");
  lua_newtable(_L);
  for (std::map<std::string, misc::variant>::const_iterator
           it(conf_params.begin()),
       end(conf_params.end());
       it != end;
       ++it) {
    switch (it->second.user_type()) {
      case misc::variant::type_int:
      case misc::variant::type_uint:
        lua_pushstring(_L, it->first.c_str());
        lua_pushinteger(_L, it->second.as_int());
        lua_rawset(_L, -3);
        break;
      case misc::variant::type_long:
      case misc::variant::type_ulong:
        lua_pushstring(_L, it->first.c_str());
        lua_pushinteger(_L, it->second.as_long());
        lua_rawset(_L, -3);
        break;
      case misc::variant::type_double:
        lua_pushstring(_L, it->first.c_str());
        lua_pushnumber(_L, it->second.as_double());
        lua_rawset(_L, -3);
        break;
      case misc::variant::type_string:
        lua_pushstring(_L, it->first.c_str());
        lua_pushstring(_L, it->second.as_string().c_str());
        lua_rawset(_L, -3);
        break;
      default:
        /* Should not arrive */
        assert(1 == 0);
    }
  }
  if (lua_pcall(_L, 1, 0, 0) != 0)
    throw exceptions::msg() << "lua: error running function `init'"
                            << lua_tostring(_L, -1);
}

/**
 *  The write method called by the stream::write method.
 *
 *  @param data The event to write.
 *
 *  @return The number of events written.
 */
int luabinding::write(std::shared_ptr<io::data> const& data) noexcept {
  int retval = 0;
  logging::debug(logging::medium) << "lua: luabinding::write call";

  // Give data to cache.
  _cache.write(data);

  // Process event.
  uint32_t type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));

  bool execute_write = true;

  // Total to acknowledge incremented
  ++_total;

  if (has_filter()) {
    // Let's get the function to call
    lua_getglobal(_L, "filter");
    lua_pushinteger(_L, cat);
    lua_pushinteger(_L, elem);

    if (lua_pcall(_L, 2, 1, 0) != 0) {
      logging::error(logging::high)
          << "lua: error while running function `filter()': "
          << lua_tostring(_L, -1);
      return 0;
    }

    if (!lua_isboolean(_L, -1)) {
      logging::error(logging::high) << "lua: `filter' must return a boolean";
      return 0;
    }

    execute_write = lua_toboolean(_L, -1);
    logging::debug(logging::medium) << "lua: `filter' returned "
                                    << (execute_write ? "true" : "false");
    lua_pop(_L, -1);
  }

  if (!execute_write)
    return 0;

  // Let's get the function to call
  lua_getglobal(_L, "write");

  // Let's build the table from the event as argument to write()
  lua_newtable(_L);
  lua_pushstring(_L, "_type");
  lua_pushinteger(_L, type);
  lua_rawset(_L, -3);

  lua_pushstring(_L, "category");
  lua_pushinteger(_L, cat);
  lua_rawset(_L, -3);

  lua_pushstring(_L, "element");
  lua_pushinteger(_L, elem);
  lua_rawset(_L, -3);

  io::data const& d(*data);
  _parse_entries(d);

  if (lua_pcall(_L, 1, 1, 0) != 0) {
    logging::error(logging::high) << "lua: error running function `write'"
                                  << lua_tostring(_L, -1);
    return 0;
  }

  if (!lua_isboolean(_L, -1)) {
    logging::error(logging::high) << "lua: `write' must return a boolean";
    return 0;
  }
  int acknowledge = lua_toboolean(_L, -1);
  lua_pop(_L, -1);

  // We have to acknowledge rejected events by the filter. It is only possible
  // when an acknowledgement is sent by the write function.
  if (acknowledge) {
    retval = _total;
    _total = 0;
  }
  return retval;
}

/**
 *  Given an event d, this method converts it to a Lua table.
 *  The result is stored on the Lua interpreter stack.
 *
 *  @param d The event to convert.
 */
void luabinding::_parse_entries(io::data const& d) {
  io::event_info const* info(io::events::instance().get_event_info(d.type()));
  if (info) {
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null();
         ++current_entry) {
      char const* entry_name(current_entry->get_name_v2());
      if (entry_name && entry_name[0]) {
        lua_pushstring(_L, entry_name);
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
        lua_rawset(_L, -3);
      }
    }
  } else
    throw exceptions::msg() << "cannot bind object of type " << d.type()
                            << " to database query: mapping does not exist";
}

/**
 *  Load the Lua interpreter with classical and custom libraries.
 *
 * @return The Lua interpreter
 */
lua_State* luabinding::_load_interpreter() {
  lua_State* L = luaL_newstate();

  // Read common lua libraries
  luaL_openlibs(L);

  // Registers the broker_log object
  broker_log::broker_log_reg(L);

  // Registers the broker socket object
  broker_socket::broker_socket_reg(L);

  // Registers the broker utils
  broker_utils::broker_utils_reg(L);

  // Registers the broker cache
  broker_cache::broker_cache_reg(L, _cache);

  return L;
}

int32_t luabinding::flush() noexcept {
  if (!_flush)
    return 0;
  // Let's get the function to call
  lua_getglobal(_L, "flush");
  if (lua_pcall(_L, 0, 1, 0) != 0) {
    logging::error(logging::high) << "lua: error running function `flush'"
                                  << lua_tostring(_L, -1);
    return 0;
  }
  if (!lua_isboolean(_L, -1)) {
    logging::error(logging::high) << "lua: `flush' must return a boolean";
    return 0;
  }
  bool acknowledge = lua_toboolean(_L, -1);
  lua_pop(_L, -1);

  int32_t retval = 0;
  if (acknowledge) {
    retval = _total;
    _total = 0;
  }
  return retval;
}
