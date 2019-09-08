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

#include "com/centreon/broker/extcmd/plaintext_command_parser.hh"
#include <cstdlib>
#include <cstring>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
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
 *  @param[in,out] listener       Command listener.
 */
plaintext_command_parser::plaintext_command_parser(command_listener& listener)
    : command_parser(listener) {}

/**
 *  Destructor.
 */
plaintext_command_parser::~plaintext_command_parser() {}

/**
 *  Parse the string given in argument.
 *
 *  @param[in] buffer     The buffer to parse.
 *  @param[out] res       The command result.
 *  @param[out] request   The parsed command request. Can be null for none.
 *
 * @return  The number of characters parsed succesfully, 0 for none.
 */
unsigned int plaintext_command_parser::parse(
    std::string const& buffer,
    command_result& res,
    std::shared_ptr<command_request>& request) {
  request.reset();
  size_t delimiter(buffer.find_first_of('\n'));
  if (delimiter != std::string::npos) {
    std::string cmd(buffer.substr(0, delimiter));
    // Process command.
    res = command_result();
    try {
      // Process command immediately if it queries
      // another command status.
      static char const* execute_cmd("EXECUTE;");
      static char const* status_cmd("STATUS;");
      if (cmd.substr(0, ::strlen(status_cmd)) == status_cmd)
        res = _listener.command_status(cmd.substr(::strlen(status_cmd)));

      // Store command in result listener and let
      // it be processed by multiplexing engine.
      else if (cmd.substr(0, ::strlen(execute_cmd)) == execute_cmd) {
        request.reset(new command_request);
        request->parse(cmd.substr(::strlen(execute_cmd)));
        logging::debug(logging::high)
            << "command: sending request " << request->uuid << " ('"
            << request->cmd << "') to endpoint '" << request->endp
            << "' of Centreon Broker instance " << request->destination_id;
        _listener.write(request);
        res = _listener.command_status(request->uuid);
      } else
        throw(exceptions::msg()
              << "invalid command format: expected "
              << "either STATUS;<CMDID> or "
              << "EXECUTE;<BROKERID>;<ENDPOINTNAME>;<CMD>[;ARG1[;ARG2...]]");
    } catch (std::exception const& e) {
      // At this point, command request was not written to the command
      // listener, so it not necessary to write command result either.
      res.uuid = "";
      res.code = -1;
      res.msg = e.what();
    }
    return (delimiter);
  }
  return (0);
}

/**
 *  Write a command result into a string.
 *
 *  @param[in] res  The command result.
 *
 *  @return         The string.
 */
std::string plaintext_command_parser::write(command_result const& res) {
  std::ostringstream oss;
  oss << res.uuid << " " << std::hex << std::showbase << res.code << " "
      << res.msg << "\n";
  return oss.str();
}
