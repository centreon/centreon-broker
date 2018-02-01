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

#include <QTcpSocket>
#include <QHostAddress>
#include <sstream>
#include "com/centreon/broker/luageneric/broker_socket.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::luageneric;

/**
 *  The Lua broker_socket constructor
 *
 *  @param L The Lua interpreter
 *
 *  @return 1
 */
int l_broker_socket_constructor(lua_State * L) {
  QTcpSocket** udata = static_cast<QTcpSocket**>(
    lua_newuserdata(L, sizeof(QTcpSocket*)));
  *udata = new QTcpSocket;

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
  delete *static_cast<QTcpSocket**>(
           luaL_checkudata(L, 1, "lua_broker_tcp_socket"));
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
  QTcpSocket* socket(
                *static_cast<QTcpSocket**>(
                  luaL_checkudata(L, 1, "lua_broker_tcp_socket")));
  char const* addr(luaL_checkstring(L, 2));
  int port(static_cast<int>(luaL_checknumber(L, 3)));
  socket->connectToHost(addr, port);
  if (!socket->waitForConnected()) {
    std::ostringstream ss;
    ss << "broker_socket::connect: Couldn't connect to "
       << addr << ":" << port
       << ": " << socket->errorString().toStdString();
    luaL_error(L, ss.str().c_str());
  }
  return 0;
}

/**
 *  The Lua broker_socket write method
 *
 *  @param L The Lua interpreter
 *
 *  @return 0
 */
static int l_broker_socket_write(lua_State* L) {
  QTcpSocket* socket(
                *static_cast<QTcpSocket**>(
                  luaL_checkudata(L, 1, "lua_broker_tcp_socket")));
  size_t len;
  char const* content(lua_tolstring(L, 2, &len));
  if (socket->write(content, len) != len) {
    std::ostringstream ss;
    ss << "broker_socket::write: Couldn't write to "
       << socket->peerAddress().toString().toStdString()
       << ":" << socket->peerPort()
       << ": " << socket->errorString().toStdString();
    luaL_error(L, ss.str().c_str());
  }

  while (socket->bytesToWrite()) {
    if (!socket->waitForBytesWritten()) {
      std::ostringstream ss;
      ss << "broker_socket::write: Couldn't send data to "
         << socket->peerAddress().toString().toStdString()
         << ":" << socket->peerPort()
         << ": " << socket->errorString().toStdString();
      luaL_error(L, ss.str().c_str());
    }
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
  QTcpSocket* socket(
                *static_cast<QTcpSocket**>(
                  luaL_checkudata(L, 1, "lua_broker_tcp_socket")));
  QString answer;

  if (!socket->waitForReadyRead()) {
    std::ostringstream ss;
    ss << "broker_socket::read: Couldn't read data from "
      << socket->peerAddress().toString().toStdString()
      << ":" << socket->peerPort()
      << ": " << socket->errorString().toStdString();
    luaL_error(L, ss.str().c_str());
  }
  answer.append(socket->readAll());
  lua_pushstring(L, answer.toStdString().c_str());
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
  QTcpSocket* socket(
                *static_cast<QTcpSocket**>(
                  luaL_checkudata(L, 1, "lua_broker_tcp_socket")));

  socket->close();
  return 0;
}

/**
 *  Load the Lua interpreter with the standard libraries
 *  and the broker lua sdk.
 *
 *  @return The Lua interpreter as a lua_State*
 */
void broker_socket::broker_socket_reg(lua_State* L) {

  luaL_Reg s_broker_socket_regs[] = {
    { "new", l_broker_socket_constructor },
    { "__gc", l_broker_socket_destructor },
    { "connect", l_broker_socket_connect },
    { "write", l_broker_socket_write },
    { "read", l_broker_socket_read },
    { "close", l_broker_socket_close },
    { NULL, NULL }
  };

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
