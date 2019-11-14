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

#include "com/centreon/broker/lua/broker_utils.hh"
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <json11.hpp>
#include <sstream>
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/parser.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

static void broker_json_encode(lua_State* L, std::ostringstream& oss);

/**
 *  The json_encode function for Lua tables
 *
 *  @param L The Lua interpreter
 *  @param oss The output stream
 */
static void broker_json_encode_table(lua_State* L, std::ostringstream& oss) {
  bool array(false);
  /* We must parse the table from the first key */
  lua_pushnil(L); /* this tells lua_next to start from the first key */
  if (lua_next(L, -2)) {
#if LUA53
    if (lua_isinteger(L, -2)) {
#else
    if (lua_isnumber(L, -2)) {
#endif
      int index(lua_tointeger(L, -2));
      if (index == 1) {
        array = true;
        oss << '[';
        broker_json_encode(L, oss);
        lua_pop(L, 1);
        while (lua_next(L, -2)) {
#if LUA53
          if (lua_isinteger(L, -2)) {
#else
          if (lua_isnumber(L, -2)) {
#endif
            oss << ',';
            broker_json_encode(L, oss);
          }
          lua_pop(L, 1);
        }
        oss << ']';
      }
    }
  } else {
    /* There are no key, the table is empty */
    oss << "[]";
    return;
  }

  if (!array) {
    oss << "{\"" << lua_tostring(L, -2) << "\":";
    broker_json_encode(L, oss);
    lua_pop(L, 1);
    while (lua_next(L, -2)) {
      oss << ",\"" << lua_tostring(L, -2) << "\":";
      broker_json_encode(L, oss);
      lua_pop(L, 1);
    }
    oss << '}';
  }
}

/**
 *  The json_encode function for Lua objects others than tables
 *
 *  @param L The Lua interpreter
 *  @param oss The output stream
 */
static void broker_json_encode(lua_State* L, std::ostringstream& oss) {
  switch (lua_type(L, -1)) {
    case LUA_TNUMBER:
      oss << lua_tostring(L, -1);
      break;
    case LUA_TSTRING: {
      /* If the string contains '"', we must escape it */
      char const* content(lua_tostring(L, -1));
      size_t pos(strcspn(content, "\\\"\t\r\n"));
      if (content[pos] != 0) {
        std::string str(content);
        char replacement[3] = "\\\\";
        do {
          switch (str[pos]) {
            case '\\':
              replacement[1] = '\\';
              break;
            case '"':
              replacement[1] = '"';
              break;
            case '\t':
              replacement[1] = 't';
              break;
            case '\r':
              replacement[1] = 'r';
              break;
            case '\n':
              replacement[1] = 'n';
              break;
          }
          str.replace(pos, 1, replacement);
          pos += 2;
        } while ((pos = str.find_first_of("\\\"\t\r\n", pos)) !=
                 std::string::npos);
        oss << '"' << str << '"';
      } else
        oss << '"' << content << '"';
    } break;
    case LUA_TBOOLEAN:
      oss << (lua_toboolean(L, -1) ? "true" : "false");
      break;
    case LUA_TTABLE:
      broker_json_encode_table(L, oss);
      break;
    default:
      luaL_error(L, "json_encode: type not implemented");
  }
}

/**
 *  The Lua json_encode function (the real one)
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
static int l_broker_json_encode(lua_State* L) {
  std::ostringstream oss;
  broker_json_encode(L, oss);
  lua_pushstring(L, oss.str().c_str());
  return 1;
}

static void broker_json_decode(lua_State* L, json11::Json const& it);

/**
 *  The Lua json_decode function for arrays.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode_array(lua_State* L, json11::Json const& it) {
  int size(it.array_items().size());
  auto cit(it.array_items().begin());
  lua_createtable(L, size, 0);
  for (int i(1); i <= size; ++i, ++cit) {
    broker_json_decode(L, *cit);
    lua_rawseti(L, -2, i);
  }
}

/**
 *  The Lua json_decode function for objects.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode_object(lua_State* L, json11::Json const& it) {
  int size(it.object_items().size());
  lua_createtable(L, 0, size);
  for (auto cit(it.object_items().begin()); cit != it.object_items().end();
       ++cit) {
    lua_pushstring(L, cit->first.c_str());
    broker_json_decode(L, cit->second);
    lua_settable(L, -3);
  }
}

/**
 *  The Lua json_decode function for anything else than array and object.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode(lua_State* L, json11::Json const& it) {
  switch (it.type()) {
    case json11::Json::STRING: {
      std::string str(it.string_value());
      size_t pos(str.find_first_of("\\"));
      while (pos != std::string::npos) {
        switch (str[pos + 1]) {
          case '\\':
            str.replace(pos, 2, "\\");
            break;
          case '"':
            str.replace(pos, 2, "\"");
            break;
          case 't':
            str.replace(pos, 2, "\t");
            break;
          case 'r':
            str.replace(pos, 2, "\r");
            break;
          case 'n':
            str.replace(pos, 2, "\n");
            break;
        }
        ++pos;
        pos = str.find_first_of("\\", pos);
      }
      lua_pushstring(L, str.c_str());
    } break;
    case json11::Json::NUMBER: {
      double value(it.number_value());
      int intvalue(it.int_value());
      if (value == intvalue)
        lua_pushinteger(L, intvalue);
      else
        lua_pushnumber(L, value);
    } break;
    case json11::Json::BOOL:
      lua_pushboolean(L, it.bool_value() ? 1 : 0);
      break;
    case json11::Json::ARRAY:
      broker_json_decode_array(L, it);
      break;
    case json11::Json::OBJECT:
      broker_json_decode_object(L, it);
      break;
    case json11::Json::NUL:
      break;
    default:
      luaL_error(L, "Unrecognized type in json content");
  }
}

/**
 *  The Lua json_decode function (the real one)
 *
 *  @param L The Lua interpreter
 */
static int l_broker_json_decode(lua_State* L) {
  char const* content(luaL_checkstring(L, -1));
  std::string err;

  json11::Json const& it{json11::Json::parse(content, err)};
  broker_json_decode(L, it);
  return 1;
}

/**
 *  The Lua parse_perfdata function
 *
 * @param L The Lua interpreter
 *
 * @return 1
 */
static int l_broker_parse_perfdata(lua_State* L) {
  char const* perf_data(lua_tostring(L, 1));
  int full(lua_toboolean(L, 2));
  storage::parser p;
  std::list<storage::perfdata> pds;
  try {
    p.parse_perfdata(perf_data, pds);
  } catch (storage::exceptions::perfdata const& e) {
    lua_pushnil(L);
    lua_pushstring(L, e.what());
    return 2;
  }
  lua_createtable(L, 0, pds.size());
  for (std::list<storage::perfdata>::const_iterator it(pds.begin()),
       end(pds.end());
       it != end; ++it) {
    storage::perfdata const& pd(*it);
    lua_pushstring(L, pd.name().c_str());
    if (full) {
      lua_createtable(L, 0, 3);
      lua_pushnumber(L, pd.value());
      lua_setfield(L, -2, "value");
      lua_pushstring(L, pd.unit().c_str());
      lua_setfield(L, -2, "uom");
      lua_pushnumber(L, pd.min());
      lua_setfield(L, -2, "min");
      lua_pushnumber(L, pd.max());
      lua_setfield(L, -2, "max");
      lua_pushnumber(L, pd.warning());
      lua_setfield(L, -2, "warning_high");
      lua_pushnumber(L, pd.warning_low());
      lua_setfield(L, -2, "warning_low");
      lua_pushboolean(L, pd.warning_mode());
      lua_setfield(L, -2, "warning_mode");

      lua_pushnumber(L, pd.critical());
      lua_setfield(L, -2, "critical_high");
      lua_pushnumber(L, pd.critical_low());
      lua_setfield(L, -2, "critical_low");
      lua_pushboolean(L, pd.critical_mode());
      lua_setfield(L, -2, "critical_mode");
      lua_settable(L, -3);
    } else {
      lua_pushnumber(L, pd.value());
      lua_settable(L, -3);
    }
  }
  return 1;
}

/**
 *  The Lua url_encode function
 *
 * @param L The Lua interpreter
 *
 * @return 1
 */
static int l_broker_url_encode(lua_State* L) {
  char const* str = lua_tostring(L, -1);

  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (char const* cc = str; *cc; ++cc) {
    char c = *cc;
    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }

  lua_pushstring(L, escaped.str().c_str());
  return 1;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_utils::broker_utils_reg(lua_State* L) {
  luaL_Reg s_broker_regs[] = {{"json_encode", l_broker_json_encode},
                              {"json_decode", l_broker_json_decode},
                              {"parse_perfdata", l_broker_parse_perfdata},
                              {"url_encode", l_broker_url_encode},
                              {nullptr, nullptr}};

#ifdef LUA51
  luaL_register(L, "broker", s_broker_regs);
#else
  luaL_newlibtable(L, s_broker_regs);
  luaL_setfuncs(L, s_broker_regs, 0);
  lua_setglobal(L, "broker");
#endif
}
