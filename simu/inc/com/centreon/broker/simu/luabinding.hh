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

#ifndef CCB_SIMU_SIMUBINDING_HH
#define CCB_SIMU_SIMUBINDING_HH

#include <map>
#include <memory>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/misc/variant.hh"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

CCB_BEGIN()

namespace simu {
/**
 *  @class luabinding luabinding.hh "com/centreon/broker/simu/luabinding.hh"
 *  @brief Class managing exchange with the the lua interpreter.
 *
 *  Expose an api to simplify exchanges with the Lua interpreter.
 */
class luabinding {
 public:
  luabinding(std::string const& lua_script,
             std::map<std::string, misc::variant> const& conf_params);
  ~luabinding();
  bool read(std::shared_ptr<io::data>& d);

 private:
  luabinding(luabinding const& other);
  luabinding& operator=(luabinding const& other);
  lua_State* _load_interpreter();
  void _load_script();
  void _init_script(std::map<std::string, misc::variant> const& conf_params);
  void _update_lua_path(std::string const& path);
  bool _parse_event(std::shared_ptr<io::data>& d);

  // Event conversion to Lua table.
  void _parse_entries(io::data const& d);

  // The Lua state machine.
  lua_State* _L;

  // True if there is a filter() function in the Lua script.
  bool _filter;

  // The Lua script name.
  std::string const& _lua_script;

  // Count on events
  int _total;
};
}  // namespace simu

CCB_END()

#endif  // !CCB_SIMU_SIMU_HH
