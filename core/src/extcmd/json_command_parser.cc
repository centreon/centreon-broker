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
#include <QString>
#include <sstream>
#include "com/centreon/broker/json/json_writer.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/extcmd/json_command_parser.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**
 *  Find a value or throw a exception.
 *
 *  @param[in] val  The key of the param to find.
 *  @param[in] it   A valid iterator.
 *
 *  @return  The value.
 */
std::string find_or_except(
              std::string const& val,
              json::json_iterator const& it) {
  json::json_iterator found = it.find_child(val).enter_children();
  if (found.is_null())
    throw (exceptions::msg() << "couldn't find '" << val << "'");
  return (found.get_string());
}

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
               misc::shared_ptr<command_request>& request) {
  res = command_result();
  request.clear();

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
    _parser.parse(buffer.substr(0, parsed + 1));
    json::json_iterator it = _parser.begin();
    std::string command_type = find_or_except("command_type", it);
    if (command_type == "status") {
      res = _listener.command_status(
                        QString::fromStdString(find_or_except("command_id", it)));
    }
    else if (command_type == "execute") {
      request = misc::make_shared(new command_request);
      request->cmd = QString::fromStdString(find_or_except("command", it));
      request->destination_id =
        QString::fromStdString(find_or_except("broker_id", it)).toUInt();
      request->endp = QString::fromStdString(find_or_except("endpoint", it));
      request->with_partial_result
        = it.find_child("with_partial_result").get_bool();
      logging::debug(logging::high)
        << "command: sending request " << request->uuid << " ('" << request->cmd
        << "') to endpoint '" << request->endp
        << "' of Centreon Broker instance " << request->destination_id;
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
    res.msg = e.what();
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
  json::json_writer writer;
  writer.open_object();
  writer.add_key("command_id");
  writer.add_string(res.uuid.toStdString());
  writer.add_key("command_code");
  writer.add_number(res.code);
  writer.add_key("command_output");
  writer.add_raw(res.msg.toStdString());
  writer.close_object();
  return (writer.get_string());
}
