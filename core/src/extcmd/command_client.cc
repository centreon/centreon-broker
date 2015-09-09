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
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"

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
 *  @param[in,out] listener       Command listener.
 */
command_client::command_client(
                  int native_socket,
                  command_listener* listener)
  : _listener(listener), _socket_native(native_socket) {}

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
  size_t delimiter(_buffer.find_first_of('\n'));
  while (delimiter == std::string::npos) {
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
    delimiter = _buffer.find_first_of('\n');
    if ((deadline == (time_t)-1) || (time(NULL) < deadline))
      QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    else
      break ;
  }

  // External command received.
  if (delimiter != std::string::npos) {
    std::string cmd(_buffer.substr(0, delimiter));
    _buffer.erase(0, delimiter + 1);

    // Process command.
    command_result res;
    try {
      // Process command immediately if it queries
      // another command status.
      static char const* execute_cmd("EXECUTE;");
      static char const* status_cmd("STATUS;");
      if (cmd.substr(0, strlen(status_cmd)) == status_cmd) {
        unsigned int cmd_id(strtoul(
                              cmd.substr(strlen(status_cmd)).c_str(),
                              NULL,
                              0));
        res = _listener->command_status(io::data::broker_id, cmd_id);
      }
      // Store command in result listener and let
      // it be processed by multiplexing engine.
      else if (cmd.substr(0, strlen(execute_cmd)) == execute_cmd) {
        misc::shared_ptr<command_request>
          req(new command_request);
        req->parse(cmd.substr(strlen(execute_cmd)));
        d = req;
        _listener->write(req);
        res = _listener->command_status(io::data::broker_id, req->id);
      }
      else
        throw (exceptions::msg() << "invalid command format: expected "
               << "either STATUS;<CMDID> or "
               << "EXECUTE;<BROKERID>;<ENDPOINTNAME>;<CMD>[;ARG1[;ARG2...]]");
    }
    catch (std::exception const& e) {
      // At this point, command request was not written to the command
      // listener, so it not necessary to write command result either.
      res.id = 0;
      res.code = -1;
      res.msg = e.what();
    }

    // Write result string to client.
    std::string result_str;
    {
      std::ostringstream oss;
      oss << std::dec << res.id << " " << std::hex << std::showbase
          << res.code << " " << res.msg.toStdString() << "\n";
      result_str = oss.str();
    }
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
unsigned int command_client::write(
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
