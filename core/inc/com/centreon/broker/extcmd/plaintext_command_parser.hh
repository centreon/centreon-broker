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

#ifndef CCB_EXTCMD_PLAINTEXT_COMMAND_PARSER_HH
#define CCB_EXTCMD_PLAINTEXT_COMMAND_PARSER_HH

#include "com/centreon/broker/extcmd/command_parser.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace extcmd {
/**
 *  @class plaintext_command_parser plaintext_command_parser.hh
 * "com/centreon/broker/extcmd/plaintext_command_parser.hh"
 *  @brief Plaintext command parser.
 */
class plaintext_command_parser : public command_parser {
 public:
  plaintext_command_parser(command_listener& listener);
  virtual ~plaintext_command_parser();

  unsigned int parse(std::string const& buffer,
                     command_result& res,
                     std::shared_ptr<command_request>& request);

  std::string write(command_result const& res);

 private:
  plaintext_command_parser(plaintext_command_parser const&);
  plaintext_command_parser& operator=(plaintext_command_parser const&);
};
}  // namespace extcmd

CCB_END()

#endif  // !CCB_EXTCMD_PLAINTEXT_COMMAND_PARSER_HH
