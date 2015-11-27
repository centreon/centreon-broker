/*
** Copyright 2015 Centreon
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
#include <QCoreApplication>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_client.hh"
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_parser.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in]     native_socket  Client socket.
 *  @param[in,out] parser         Command parser.
 */
command_client::command_client(
                  int native_socket,
                  command_parser& parser)
  : _parser(parser), _socket_native(native_socket) {}

/**
 *  Destructor.
 */
command_client::~command_client() {
  if (_socket_native >= 0)
    _initialize_socket();
  if (_socket.get())
    _socket->close();
}

/**
 *  Read from command client.
 *
 *  @param[out] d         Read event.
 *  @param[in]  deadline  Deadline in time.
 *
 *  @return Respect io::stream's read() return value.
 */
bool command_client::read(
                       misc::shared_ptr<io::data>& d,
                       time_t deadline) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Read commands from socket.
  d.clear();
  command_result res;
  misc::shared_ptr<command_request> req;
  size_t parsed = 0;
  while ((parsed = _parser.parse(_buffer, res, req)) == 0) {
    if (_socket->waitForReadyRead(0)) {
      char buffer[1000];
      int rb(_socket->read(buffer, sizeof(buffer)));
      if (rb == 0)
        throw (io::exceptions::shutdown(true, true)
               << "command: client disconnected");
      else if (rb < 0)
        throw (exceptions::msg() << "command: error on client socket: "
               << _socket->errorString());
      _buffer.append(buffer, rb);
    }
    if ((deadline == (time_t)-1) || (time(NULL) < deadline))
      QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    else
      break ;
  }

  // External command parsed.
  if (parsed != 0) {
    d = req;
    // Erase the parsed data.
    _buffer.erase(0, parsed + 1);

    // Write result string to client.
    std::string result_str = _parser.write(res);
    int pos(0), remaining(result_str.size());
    while (remaining > 0) {
      int wb(_socket->write(result_str.data() + pos, remaining));
      if (wb < 0)
        throw (exceptions::msg()
               << "could not write command result to client: "
               << _socket->errorString());
      pos += wb;
      remaining -= wb;
    }

    return (true);
  }
  // No data so we timed out.
  else
    return (false);
}

/**
 *  Write to command client.
 *
 *  @param[in] d  Unused.
 *
 *  @return This method will throw.
 */
int command_client::write(
                      misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (io::exceptions::shutdown(false, true)
         << "command: cannot write event to command client");
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Initialize socket.
 */
void command_client::_initialize_socket() {
  _socket.reset(new QLocalSocket);
  _socket->setSocketDescriptor(_socket_native);
  _socket_native = -1;
  return ;
}
