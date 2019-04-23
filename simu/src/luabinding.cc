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

#include <QVariant>
#include <fstream>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/simu/broker_log.hh"
#include "com/centreon/broker/simu/broker_utils.hh"
#include "com/centreon/broker/simu/luabinding.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::simu;

/**
 *  Constructor.
 *
 *  @param[in] lua_script the json parameters file
 *  @param[in] conf_params A hash table with user parameters
 */
luabinding::luabinding(
              std::string const& lua_script,
              QMap<QString, QVariant> const& conf_params)
  : _lua_script(lua_script),
    _total(0) {
  size_t pos(lua_script.find_last_of('/'));
  std::string path(lua_script.substr(0, pos));
  _L = _load_interpreter();
  _update_lua_path(path);

  logging::debug(logging::medium)
    << "simu: initializing the Lua virtual machine";

  _load_script();
  _init_script(conf_params);
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
 *  Reads the Lua script, checks its syntax and checks if
 *   - init()
 *   - read()
 *  functions exist in the Lua script. The two first ones are
 *  mandatory whereas the third one is optional.
 */
void luabinding::_load_script() {
  // script loading
  if (luaL_loadfile(_L, _lua_script.c_str()) != 0) {
    char const* error_msg(lua_tostring(_L, -1));
    throw exceptions::msg()
      << "simu: '" << _lua_script << "' could not be loaded: "
      << error_msg;
  }

  // Script compilation
  if (lua_pcall(_L, 0, 0, 0) != 0) {
    throw exceptions::msg()
      << "simu: '" << _lua_script << "' could not be compiled";
  }

  // Checking for init() availability: this function is mandatory
  lua_getglobal(_L, "init");
  if (!lua_isfunction(_L, lua_gettop(_L)))
   throw exceptions::msg()
     << "simu: '" << _lua_script << "' init() global function is missing";

  // Checking for read() availability: this function is mandatory
  lua_getglobal(_L, "read");
  if (!lua_isfunction(_L, lua_gettop(_L)))
   throw exceptions::msg()
     << "simu: '" << _lua_script
     << "' read() global function is missing";
}

/**
 *  Executes the init() function given in the Lua script
 *  with the content of conf_params as parameter.
 *
 *  @param conf_params A hashtable of data providing various
 *  informations needed by the script to work.
 *
 */
void luabinding::_init_script(QMap<QString, QVariant> const& conf_params) {
  lua_getglobal(_L, "init");
  lua_newtable(_L);
  for (QMap<QString, QVariant>::const_iterator
         it(conf_params.begin()),
         end(conf_params.end());
       it != end;
       ++it) {
    bool ok;
    switch (it->userType())
    {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
          lua_pushstring(_L, it.key().toStdString().c_str());
          lua_pushinteger(_L, it->toInt(&ok));
          lua_rawset(_L, -3);
          break;
        case QMetaType::Double:
        case QMetaType::Float:
          lua_pushstring(_L, it.key().toStdString().c_str());
          lua_pushnumber(_L, it->toDouble(&ok));
          lua_rawset(_L, -3);
          break;
        case QMetaType::QString:
          lua_pushstring(_L, it.key().toStdString().c_str());
          lua_pushstring(_L, it->toString().toStdString().c_str());
          lua_rawset(_L, -3);
          break;
    }
  }
  if (lua_pcall(_L, 1, 0, 0) != 0)
    throw exceptions::msg()
      << "simu: error running function `init'"
      << lua_tostring(_L, -1);
}

/**
 *  The read method called by the stream::read method.
 *
 *  @param d The event to read.
 *
 *  @return The number of events written.
 */
bool luabinding::read(std::shared_ptr<io::data>& data) {
  bool retval(false);
  logging::debug(logging::medium) << "simu: luabinding::read call";

  // Total to acknowledge incremented
  ++_total;

  // Let's get the function to call
  lua_getglobal(_L, "read");

  if (lua_pcall(_L, 0, 1, 0) != 0)
    throw exceptions::msg()
      << "simu: error running function `read' "
      << lua_tostring(_L, -1);

  if (lua_istable(_L, -1))
    retval = _parse_event(data);
  else if (lua_isnil(_L, -1))
    lua_pop(_L, -1);
  else {
    throw exceptions::msg()
      << "simu: `read' must return a table or a nil value ("
      << lua_type(_L, -1) << " type return)";
  }

  return retval;
}

/**
 *  Given an event d, this method converts it to a Lua table.
 *  The result is stored on the Lua interpreter stack.
 *
 *  @param d The event to convert.
 *  @return true if an object is correctly filled, false otherwise.
 */
bool luabinding::_parse_event(std::shared_ptr<io::data>& d) {
  bool retval(true);
  d.reset();
  lua_pushnil(_L);  // push nil, so lua_next removes it from stack and puts (k, v) on stack
  QMap<QString, QVariant> map;
  while (lua_next(_L, -2) != 0) { // -2, because we have table at -1
    if (lua_isstring(_L, -2)) { // only store stuff with string keys
      char const* key(lua_tostring(_L, -2));
      if (lua_isboolean(_L, -1))
        map[key] = QVariant(lua_toboolean(_L, -1));
#if LUA53
      else if (lua_isinteger(_L, -1))
        map[key] = QVariant(lua_tointeger(_L, -1));
#endif
      else if (lua_isnumber(_L, -1))
        map[key] = QVariant(lua_tonumber(_L, -1));
      else if (lua_isstring(_L, -1))
        map[key] = QVariant(lua_tostring(_L, -1));
      else
        throw exceptions::msg() << "simu: item with key " << key
          << " is not supported for a broker event";
    }
    lua_pop(_L, 1); // remove value, keep key for lua_next
  }

  lua_pop(_L, 1); // pop table
  io::event_info const*
    info(io::events::instance().get_event_info(map["_type"].toUInt()));
  if (info) {
    // Create event
    std::unique_ptr<io::data> t(info->get_operations().constructor());
    if (t.get()) {
      // Browse all mapping to unserialize the object.
      for (mapping::entry const* current_entry(info->get_mapping());
           !current_entry->is_null();
           ++current_entry) {
        if (map.contains(current_entry->get_name_v2())) {
          // Skip entries that should not be serialized.
          switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            current_entry->set_bool(*t, map[current_entry->get_name_v2()].toBool());
            break ;
          case mapping::source::DOUBLE:
            current_entry->set_double(*t, map[current_entry->get_name_v2()].toDouble());
            break ;
          case mapping::source::INT:
            current_entry->set_int(*t, map[current_entry->get_name_v2()].toInt());
            break ;
          case mapping::source::SHORT:
            current_entry->set_short(
                             *t,
                             static_cast<short>(
                               map[current_entry->get_name_v2()].toInt()));
            break ;
          case mapping::source::STRING:
            current_entry->set_string(*t, map[current_entry->get_name_v2()].toString());
            break ;
          case mapping::source::TIME:
            current_entry->set_time(*t, map[current_entry->get_name_v2()].toULongLong());
            break ;
          case mapping::source::UINT:
            current_entry->set_uint(*t, map[current_entry->get_name_v2()].toUInt());
            break ;
          default:
            throw (exceptions::msg() << "simu: invalid mapping for "
                   << "object of type '" << info->get_name() << "': "
                   << current_entry->get_type()
                   << " is not a known type ID");
          }
        }
      }
      d.reset(t.release());
    }
    else
      throw exceptions::msg() << "simu: cannot create object of ID "
                          << map["_type"].toInt() << " whereas it has been registered";
  }
  else {
    logging::info(logging::high)
      << "simu: cannot unserialize event of ID " << map["_type"].toInt()
      << ": event was not registered and will therefore be ignored";
    retval = false;
  }
  return retval;
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

  // Registers the broker_utils object
  broker_utils::broker_utils_reg(L);

  return L;
}
