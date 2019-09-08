/*
** Copyright 2015,2017 Centreon
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

#include "com/centreon/broker/extcmd/command_client.hh"
#include <cstdlib>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_parser.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace asio;
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
command_client::command_client(asio::local::stream_protocol::socket& socket,
                               command_parser& parser)
    : _parser(parser) {
  _socket.reset(&socket);
}

/**
 *  Destructor.
 */
command_client::~command_client() {
  if (_socket)
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
bool command_client::read(std::shared_ptr<io::data>& d, time_t deadline) {
  std::error_code err;

  // Check that socket exist.
  if (!_socket)
    throw exceptions::shutdown() << "socket does not exist";

  // Read commands from socket.
  d.reset();
  command_result res;
  std::shared_ptr<command_request> req;
  size_t parsed = 0;
  while ((parsed = _parser.parse(_buffer, res, req)) == 0) {
    asio::streambuf b;

    size_t len = asio::read(*_socket, b, asio::transfer_all(), err);
    if (err)
      throw exceptions::shutdown() << "command: client disconnected";

    std::string s((std::istreambuf_iterator<char>(&b)),
                  std::istreambuf_iterator<char>());
    _buffer.append(s.c_str(), len);

    if ((deadline == (time_t)-1) || (time(nullptr) < deadline))
      ;
    else
      break;
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
      size_t wb =
          asio::write(*_socket, buffer(result_str), asio::transfer_all(), err);
      if (err)
        throw(exceptions::msg()
              << "could not write command result to client: " << err.message());
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
int command_client::write(std::shared_ptr<io::data> const& d) {
  (void)d;
  throw(exceptions::shutdown()
        << "command: cannot write event to command client");
  return (1);
}
