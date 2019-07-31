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

#ifndef CCB_LUA_BROKER_LOG_HH
#  define CCB_LUA_BROKER_LOG_HH

#  include "com/centreon/broker/lua/luabinding.hh"

CCB_BEGIN()

namespace               lua {
  /**
   *  @class broker_log broker_log.hh "com/centreon/broker/lua/broker_log.hh"
   *  @brief Class managing the Lua connector logs
   *
   *  This class manages the logs written from the Lua script.
   */
  class                 broker_log {
   public:
    static void         broker_log_reg(lua_State* L);

    void                set_parameters(
                          int level,
                          std::string const& filename);
    int                 get_level() const;
    std::string const&  get_file() const;

   private:
                        broker_log();

    // Data exchanged with the Lua interpreter
    std::string         _file;
    int                 _level;
  };
}

CCB_END()

#endif // !CCB_LUA_BROKER_LOG_HH
