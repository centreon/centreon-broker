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

#ifndef CCB_SIMU_BROKER_SOCKET_HH
#  define CCB_SIMU_BROKER_SOCKET_HH

#  include "com/centreon/broker/namespace.hh"

extern "C" {
#  include "lua.h"
#  include "lauxlib.h"
#  include "lualib.h"
}

CCB_BEGIN()

namespace               simu {
  /**
   *  @class broker_socket broker_socket.hh "com/centreon/broker/simu/broker_socket.hh"
   *  @brief Class providing TCP socket to the lua interpreter
   *
   *  This class provides a binding to Lua to access TCP sockets.
   */
  class                 broker_socket {
   public:
    static void         broker_socket_reg(lua_State* L);
  };
}

CCB_END()

#endif // !CCB_SIMU_BROKER_SOCKET_HH
