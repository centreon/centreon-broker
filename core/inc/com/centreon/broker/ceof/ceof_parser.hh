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

#ifndef CCB_CEOF_CEOF_PARSER_HH
#define CCB_CEOF_CEOF_PARSER_HH

#include <string>
#include <vector>
#include "com/centreon/broker/ceof/ceof_iterator.hh"
#include "com/centreon/broker/ceof/ceof_token.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_parser ceof_parser.hh "com/centreon/broker/ceof/ceof_parser.hh"
 *  @brief Centreon Engine Object File parser.
 */
class ceof_parser {
 public:
  ceof_parser(std::string const& str);
  ~ceof_parser() throw();

  ceof_iterator parse();

 private:
  std::string _string;

  std::vector<ceof_token> _tokens;

  ceof_parser(ceof_parser const& other);
  ceof_parser& operator=(ceof_parser const& other);
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_PARSER_HH
