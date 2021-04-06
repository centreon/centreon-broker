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

#ifndef CCB_EXTCMD_COMMAND_PARSER_HH
#define CCB_EXTCMD_COMMAND_PARSER_HH

#include <string>
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/io/factory.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace extcmd {
/**
 *  @class command_parser command_parser.hh
 * "com/centreon/broker/extcmd/command_parser.hh"
 *  @brief Command parser interface.
 */
class command_parser {
 public:
  command_parser(command_listener& listener);
  virtual ~command_parser();

  /**
   *  Parse the string given in argument.
   *
   *  @param[in] buffer     The buffer to parse.
   *  @param[out] res       The command result.
   *  @param[out] request   The parsed command request. Can be null for none.
   *
   * @return  The number of characters parsed succesfully, 0 for none.
   */
  virtual uint32_t parse(std::string const& buffer,
                         command_result& res,
                         std::shared_ptr<command_request>& request) = 0;
  /**
   *  Write a command result into a string.
   *
   *  @param[in] res  The command result.
   *
   *  @return         The string.
   */
  virtual std::string write(command_result const& res) = 0;

 protected:
  command_parser(command_parser const&);
  command_parser& operator=(command_parser const&);

  command_listener& _listener;
};
}  // namespace extcmd

CCB_END()

#endif  // !CCB_EXTCMD_COMMAND_PARSER_HH
