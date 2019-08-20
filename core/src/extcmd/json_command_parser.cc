/*
** Copyright 2015-2016 Centreon
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
#include <QString>
#include <sstream>
#include <json11.hpp>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/extcmd/json_command_parser.hh"
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
json_command_parser::json_command_parser(command_listener& listener)
  : command_parser(listener) {}

/**
 *  Destructor.
 */
json_command_parser::~json_command_parser() {}

/**
 *  Parse the string given in argument.
 *
 *  @param[in] buffer     The buffer to parse.
 *  @param[out] res       The command result.
 *  @param[out] request   The parsed command request. Can be null for none.
 *
 * @return  The number of characters parsed succesfully, 0 for none.
 */
unsigned int json_command_parser::parse(
               std::string const& buffer,
               command_result& res,
               std::shared_ptr<command_request>& request) {
  res = command_result();
  request.reset();

  // Try to find a valid json snippet.
  size_t parsed = 0;
  size_t level = 0;
  try {
    for (parsed = 0; parsed < buffer.size(); ++parsed) {
      if (buffer[parsed] == '{') {
        ++level;
        break ;
      }
      else if (buffer[parsed] != ' ' && buffer[parsed] != '\n')
        throw (exceptions::msg() << "expected '{'");
    }
    // Didn't find opening '{'.
    if (level == 0)
      return (0);
    for (++parsed; parsed < buffer.size(); ++parsed) {
      if (buffer[parsed] == '{')
        ++level;
      else if (buffer[parsed] == '}')
        --level;
      if (level == 0)
        break ;
    }
    // Didn't find closing '}'
    if (level != 0)
      return (0);

    // Found a (hopefully) valid json snippet. Try to parse it.
    std::string err;
    json11::Json js{json11::Json::parse(buffer.substr(0, parsed + 1), err) };
    if (js.is_null() || !js.is_object()) {
      throw (exceptions::msg() << "cannot parse json stream'");
    }

    json11::Json const& command_type{js["command_type"]};

    if (!command_type.is_string()) {
      throw (exceptions::msg() << "couldn't find 'command_type'");
    }

    if (command_type.string_value() == "status") {
      json11::Json command_id{js["command_id"]};
      if (!command_id.is_string())
        throw (exceptions::msg() << "couldn't find 'command_id'");
      _listener.command_status(QString::fromStdString(command_id.string_value()));
    } else if (command_type.string_value() == "execute") {
      json11::Json command{js["command"]};
      json11::Json broker_id{js["broker_id"]};
      json11::Json endpoint{js["endpoint"]};

      request.reset(new command_request);
      if (!command.is_string())
        throw (exceptions::msg() << "couldn't find 'commands'");
      request->cmd = QString::fromStdString(command.string_value());
      if (!broker_id.is_string())
        throw (exceptions::msg() << "couldn't find 'broker_id'");
      request->destination_id = std::stoul(broker_id.string_value());
      if (!endpoint.is_string())
        throw (exceptions::msg() << "couldn't find 'endpoint'");
      request->endp = QString::fromStdString(endpoint.string_value());

      json11::Json with_partial_result{js["endpoint"]["with_partial_result"]};
      if (with_partial_result.is_bool())
        request->with_partial_result = with_partial_result.bool_value();

      logging::debug(logging::high)
        << "command: sending request " << request->uuid << " ('" << request->cmd
        << "') to endpoint '" << request->endp
        << "' of Centreon Broker instance " << request->destination_id
        << " with partial result "
        << (request->with_partial_result ? "enabled" : "disabled");
      _listener.write(request);
      res = _listener.command_status(request->uuid);
    }
    else {
      throw (exceptions::msg()
             << "invalid command type: expected 'execute' or 'status' ");
    }
  } catch (std::exception const& e) {
    // At this point, command request was not written to the command
    // listener, so it not necessary to write command result either.
    res.uuid = QString();
    res.code = -1;
    res.msg = QString("\"") + e.what() + "\"";
  }
  return (parsed);
}

/**
 *  Write a command result into a string.
 *
 *  @param[in] res  The command result.
 *
 *  @return         The string.
 */
std::string json_command_parser::write(command_result const& res) {
  json11::Json writer = json11:: Json::object {
    {"command_id", res.uuid.toStdString()},
    {"command_code", res.code},
    {"command_output", res.msg.toStdString()},
  };

  return (writer.dump());
}
