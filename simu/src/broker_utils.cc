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

#include <cstdlib>
#include <cstring>
#include <sstream>
#include "com/centreon/broker/json/json_parser.hh"
#include "com/centreon/broker/simu/broker_utils.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::json;
using namespace com::centreon::broker::simu;

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
  }
  else {
    /* There are no key, the table is empty */
    oss << "[]";
    return ;
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
    case LUA_TSTRING:
      {
        /* If the string contains '"', we must escape it */
        char const* content(lua_tostring(L, -1));
        size_t pos(strcspn(content, "\"\t\r\n"));
        if (content[pos] != 0) {
          std::string str(content);
          char replacement[3] = "\\\\";
          do {
            switch (str[pos]) {
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
          } while ((pos = str.find_first_of("\"\t\r\n", pos))
                    != std::string::npos);
          oss << '"' << str << '"';
        }
        else
          oss << '"' << content << '"';
      }
      break;
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

static void broker_json_decode(lua_State* L, json::json_iterator& it);

/**
 *  The Lua json_decode function for arrays.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode_array(lua_State* L, json::json_iterator& it) {
  int size(it.children());
  json::json_iterator cit(it.enter_children());
  lua_createtable(L, size, 0);
  for (int i(1); i <= size; ++i, ++cit) {
    broker_json_decode(L, cit);
    lua_rawseti(L, -2, i);
  }
}

/**
 *  The Lua json_decode function for objects.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode_object(lua_State* L, json::json_iterator& it) {
  int size(it.children());
  lua_createtable(L, 0, size);
  for (json::json_iterator cit(it.enter_children());
       !cit.end();
       ++cit) {
    broker_json_decode(L, cit);
    json::json_iterator ccit(cit.enter_children());
    broker_json_decode(L, ccit);
    lua_settable(L, -3);
  }
}

/**
 *  The Lua json_decode function for anything else than array and object.
 *
 *  @param L The Lua interpreter
 *  @param it The current json_iterator
 */
static void broker_json_decode(lua_State* L, json::json_iterator& it) {
  switch (it.get_type()) {
    case json_iterator::string:
      {
        std::string str(it.get_string());
        size_t pos(str.find_first_of("\\"));
        while (pos != std::string::npos) {
          switch (str[pos + 1]) {
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
      }
      break;
    case json_iterator::number:
      {
        double value(atof(it.get_string().c_str()));
        int intvalue(atoi(it.get_string().c_str()));
        if (value == intvalue)
          lua_pushinteger(L, intvalue);
        else
          lua_pushnumber(L, value);
      }
      break;
    case json_iterator::boolean:
      lua_pushboolean(L, it.get_bool() ? 1 : 0);
      break;
    case json_iterator::array:
      broker_json_decode_array(L, it);
      break;
    case json_iterator::object:
      broker_json_decode_object(L, it);
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
  json::json_parser parser;
  parser.parse(content);
  json::json_iterator it(parser.begin());
  broker_json_decode(L, it);
  return 1;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_utils::broker_utils_reg(lua_State* L) {

  luaL_Reg s_broker_regs[] = {
    { "json_encode", l_broker_json_encode },
    { "json_decode", l_broker_json_decode },
    { NULL, NULL }
  };

#ifdef LUA51
  luaL_register(L, "broker", s_broker_regs);
#else
  luaL_newlibtable(L, s_broker_regs);
  luaL_setfuncs(L, s_broker_regs, 0);
  lua_setglobal(L, "broker");
#endif
}
