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
               misc::shared_ptr<command_request>& request) {

  return (0);
}

/**
 *  Write a command result into a string.
 *
 *  @param[in] res  The command result.
 *
 *  @return         The string.
 */
std::string json_command_parser::write(command_result const& res) {
  std::ostringstream oss;
  return (oss.str());
}
