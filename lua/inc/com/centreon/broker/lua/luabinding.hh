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
#define CCB_LUA_LUABINDING_HH

#include <map>

#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/variant.hh"

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

CCB_BEGIN()

namespace lua {
/**
 *  @class luabinding luabinding.hh
 * "com/centreon/broker/luabinding/luabinding.hh"
 *  @brief Class managing exchange with the the lua interpreter.
 *
 *  Here is the class used to send broker data to the Lua interpreter. We are
 *  compatible with Lua 5.1, 5.2, 5.3 and 5.4.
 *
 *  This class constructor needs three parameters that are:
 *  * the script to load (just its file name).
 *  * the configuration parameters given as a map.
 *  * a macro_cache object (used to store the cache...).
 *
 *  At the construction, the script is read. We check also that it contains:
 *  * a global function init(conf) : this one is mandatory. conf is a Lua table
 *    containing the configuration set in the web configuration of broker.
 *  * a global function write(d) : this one is mandatory. if the v1 api is used,
 *    d is a Lua table that reprensents a transformed Broker event. In case of
 *    v2 api, d is a Lua userdata containing directly the Broker event. The api
 *    allows the user to use this event as if it was a Lua table. To choose what
 *    api to use, the script has to contain the definition of a variable named
 *    broker_version_api, that should contain 1 or 2. The write function returns
 *    true to allow broker to acknowledge all the last received events and false
 *    otherwise.
 *  * a global filter(c, v) : this one is not mandatory. This function improves
 *    Broker performance particularly in the case of the v1 api, because
 *    transforming an event into Lua table is expensive for the CPU. Using it
 *    may be dangerous, events retained by the filter are not automatically
 *    acknowledged because broker can only acknowledge events from the beginning
 *    of its queue and the acknowledged block must not contain hole, they must
 *    be contiguous. So filtered events are acknowledged only when not filtered
 *    events among them are also acknowledged.
 *    If the beginning of the queue is as follows (N not filtered, F for
 *    filtered), The first one is given to the write function but often not
 *    acknowledged immediatly. Then, the three next events are not given to
 *    write() but cannot be acknowledged since the first one is still waiting.
 *    And so only when the first one will be acknowledged, those ones will:
 *
 *                        NFFFNFFNNN.....
 *
 *  * a global flush() : this one is not mandatory but may be should. The good
 *    practice is to have a write function that keeps a queue of received
 *    events. When this queue reaches the a max size, it is sent to a peer using
 *    the flush() function. If this flush() function successes, it returns true.
 *
 *    And in case of retention, broker does not call anymore the write()
 *    function, but directly calls flush(). If present, this will clear the
 *    queue and events are acknowledged so it will be possible again to call the
 *    write() function.
 *
 */
class luabinding {
  // The Lua state machine.
  lua_State* _L;

  // True if there is a filter() function in the Lua script.
  bool _filter;

  // True if there is a flush() function in the Lua script.
  bool _flush;

  // The cache.
  macro_cache& _cache;

  // Count on events
  int32_t _total;

  // Api version among (1, 2)
  uint32_t _broker_api_version;

  lua_State* _load_interpreter();
  void _load_script(const std::string& lua_script);
  void _init_script(std::map<std::string, misc::variant> const& conf_params);
  void _update_lua_path(std::string const& path);

 public:
  luabinding(std::string const& lua_script,
             std::map<std::string, misc::variant> const& conf_params,
             macro_cache& cache);
  luabinding(luabinding const&) = delete;
  luabinding& operator=(luabinding const&) = delete;
  ~luabinding();
  bool has_filter() const noexcept;
  int32_t write(std::shared_ptr<io::data> const& data) noexcept;
  bool has_flush() const noexcept;
  int32_t flush() noexcept;
};

// Event conversion to Lua table.
void push_event_as_table(lua_State* L, io::data const& d);

}  // namespace lua

CCB_END()

#endif  // !CCB_LUA_LUA_HH
