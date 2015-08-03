/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
 *  Define which events to process.
 *
 *  @param[in] in   True to process input.
 *  @param[in] out  True to process output.
 */
void command_client::process(bool in, bool out) {
  (void)in;
  (void)out;
  return ;
}

/**
 *  Read from command client.
 *
 *  @param[out] d         Read event.
 */
void command_client::read(misc::shared_ptr<io::data>& d) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  while (true) {
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
      // if ((deadline == (time_t)-1) || (time(NULL) < deadline))
      //   QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
      // else
      //   break ;
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
          res = _listener->command_status(cmd_id);
        }
        // Store command in result listener and let
        // it be processed by multiplexing engine.
        else if (cmd.substr(0, strlen(execute_cmd)) == execute_cmd) {
          misc::shared_ptr<command_request>
            req(new command_request);
          req->parse(cmd.substr(strlen(execute_cmd)));
          d = req;
          _listener->write(req);
          res = _listener->command_status(req->id);
        }
        else
          throw (exceptions::msg() << "invalid command format: expected"
                 << " either STATUS;<CMDID> or "
                 << "EXECUTE;<ENDPOINTNAME>;<CMD>[;ARG1[;ARG2...]]");
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
    }
  }
  return ;
}

/**
 *  Write to command client.
 *
 *  @param[in] d  Unused.
 *
 *  @return Always return 1.
 */
unsigned int command_client::write(
                               misc::shared_ptr<io::data> const& d) {
  (void)d;
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
