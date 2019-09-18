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

#include "com/centreon/broker/lua/broker_socket.hh"
#include <asio.hpp>
#include <sstream>

#if ASIO_VERSION < 101200
namespace asio {
typedef io_service io_context;
}
#endif

using namespace asio;
using namespace com::centreon::broker;
using namespace com::centreon::broker::lua;

static io_context ctx;
enum {
  unconnected,
  hostLookup,
  connecting,
  connected,
  closing,
} socket_state{unconnected};

/**
 *  The Lua broker_socket constructor
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
int l_broker_socket_constructor(lua_State* L) {
  ip::tcp::socket** udata{static_cast<ip::tcp::socket**>(
      lua_newuserdata(L, sizeof(ip::tcp::socket*)))};
  *udata = new ip::tcp::socket{ctx};

  socket_state = unconnected;

  luaL_getmetatable(L, "lua_broker_tcp_socket");
  lua_setmetatable(L, -2);

  return 1;
}

/**
 *  The Lua broker_socket destructor
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_socket_destructor(lua_State* L) {
  delete *static_cast<ip::tcp::socket**>(
      luaL_checkudata(L, 1, "lua_broker_tcp_socket"));

  socket_state = unconnected;
  return 0;
}

/**
 *  The Lua broker_socket connect method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_socket_connect(lua_State* L) {
  ip::tcp::socket* socket{*static_cast<ip::tcp::socket**>(
      luaL_checkudata(L, 1, "lua_broker_tcp_socket"))};
  char const* addr{luaL_checkstring(L, 2)};
  int port{static_cast<int>(luaL_checknumber(L, 3))};

  socket_state = hostLookup;
  ip::tcp::resolver resolver{ctx};
  ip::tcp::resolver::query query{addr, std::to_string(port)};

  try {
    ip::tcp::resolver::iterator it{resolver.resolve(query)};
    ip::tcp::resolver::iterator end;
    socket_state = connecting;

    std::error_code err{std::make_error_code(std::errc::host_unreachable)};

    // it can resolve to multiple addresses like ipv4 and ipv6
    // we need to try all to find the first available socket
    while (err && it != end) {
      socket->connect(*it, err);

      if (err)
        socket->close();

      ++it;
    }

    if (err) {
      socket_state = unconnected;
      std::ostringstream ss;
      ss << "broker_socket::connect: Couldn't connect to " << addr << ":"
         << port << ": " << err.message();
      luaL_error(L, ss.str().c_str());
    } else {
      socket_state = connected;
    }
  } catch (std::system_error const& se) {
    socket_state = unconnected;
    std::ostringstream ss;
    ss << "broker_socket::connect: Couldn't connect to " << addr << ":" << port
       << ": " << se.what();
    luaL_error(L, ss.str().c_str());
  }
  return 0;
}

/**
 *  The Lua broker_socket state method
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
static int l_broker_socket_state(lua_State* L) {
  char const* ans[] = {
      "unconnected", "hostLookup", "connecting", "connected", "closing",
  };
  lua_pushstring(L, ans[socket_state]);
  return 1;
}

/**
 *  The Lua broker_socket write method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_socket_write(lua_State* L) {
  ip::tcp::socket* socket{*static_cast<ip::tcp::socket**>(
      luaL_checkudata(L, 1, "lua_broker_tcp_socket"))};
  size_t len;
  char const* content(lua_tolstring(L, 2, &len));
  std::error_code err;

  asio::write(*socket, asio::buffer(content, len), asio::transfer_all(), err);

  if (err) {
    std::ostringstream ss;
    ss << "broker_socket::write: Couldn't write to "
       << socket->remote_endpoint().address().to_string() << ":"
       << socket->remote_endpoint().port() << ": " << err.message();
    luaL_error(L, ss.str().c_str());
  }

  return 0;
}

/**
 *  The Lua broker_socket read method
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
static int l_broker_socket_read(lua_State* L) {
  ip::tcp::socket* socket{*static_cast<ip::tcp::socket**>(
      luaL_checkudata(L, 1, "lua_broker_tcp_socket"))};
  std::error_code err;

  char* buff = new char[1024];

  size_t len = socket->read_some(asio::buffer(buff, 1024), err);

  if (err && err != asio::error::eof) {
    std::ostringstream ss;
    ss << "broker_socket::read: Couldn't read data from "
       << socket->remote_endpoint().address().to_string() << ":"
       << socket->remote_endpoint().port() << ": " << err.message();
    luaL_error(L, ss.str().c_str());
  } else if (!err) {
    lua_pushlstring(L, buff, len);
  }

  delete[] buff;
  return 1;
}

/**
 *  The Lua broker_socket close method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_socket_close(lua_State* L) {
  ip::tcp::socket* socket{*static_cast<ip::tcp::socket**>(
      luaL_checkudata(L, 1, "lua_broker_tcp_socket"))};

  socket_state = closing;
  socket->close();
  socket_state = unconnected;
  return 0;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_socket::broker_socket_reg(lua_State* L) {
  luaL_Reg s_broker_socket_regs[] = {{"new", l_broker_socket_constructor},
                                     {"__gc", l_broker_socket_destructor},
                                     {"connect", l_broker_socket_connect},
                                     {"get_state", l_broker_socket_state},
                                     {"write", l_broker_socket_write},
                                     {"read", l_broker_socket_read},
                                     {"close", l_broker_socket_close},
                                     {nullptr, nullptr}};

  // Create a metatable. It is not exposed to Lua. It is not
  // exposed to Lua. The "lua_broker" label is used by Lua
  // internally to identify things.
  luaL_newmetatable(L, "lua_broker_tcp_socket");

  // Register the C functions into the metatable we just created.
#ifdef LUA51
  luaL_register(L, NULL, s_broker_socket_regs);
#else
  luaL_setfuncs(L, s_broker_socket_regs, 0);
#endif

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -2
  // 2  =>  metatable "lua_broker"    => -1
  lua_pushvalue(L, -1);

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -3
  // 2  =>  metatable "lua_broker"    => -2
  // 3  =>  metatable "lua_broker"    => -1

  // Set the __index field of the metatable to point to itself
  lua_setfield(L, -1, "__index");

  // The Lua stack at this point looks like:
  // 1  =>  userdata                  => -2
  // 2  =>  metatable "lua_broker"    => -1

  // And now, we use setglobal to store userdata as the variable "broker".
  lua_setglobal(L, "broker_tcp_socket");
}
