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

#ifndef CCB_CEOF_CEOF_TOKEN_HH
#define CCB_CEOF_CEOF_TOKEN_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_token ceof_token.hh "com/centreon/broker/ceof/ceof_token.hh"
 *  @brief Centreon Engine Object File token.
 *
 *  As produced by the parser.
 */
class ceof_token {
 public:
  enum token_type { object, key, value };

  ceof_token();
  ceof_token(token_type type,
             std::string const& val,
             int token_number,
             int parent);
  ceof_token(ceof_token const& other);
  ceof_token& operator=(ceof_token const& other);
  ~ceof_token() throw();

  token_type get_type() const throw();
  std::string const& get_value() const throw();
  int get_token_number() const throw();
  int get_parent_token() const throw();

 private:
  token_type _type;
  std::string _value;
  int _token_number;
  int _parent_token;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_TOKEN_HH
