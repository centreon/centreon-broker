/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_BOOL_TOKENIZER_HH
#  define CCB_BAM_BOOL_TOKENIZER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           bam {
  /**
   *  @class bool_tokenizer bool_tokenizer.hh "com/centreon/broker/bam/bool_tokenizer.hh"
   *  @brief Tokenize boolean expression.
   *
   *  Split a boolean expression in tokens.
   */
  class             bool_tokenizer {
  public:
                    bool_tokenizer(std::string const& text);
                    bool_tokenizer(bool_tokenizer const& other);
                    ~bool_tokenizer();
    bool_tokenizer& operator=(bool_tokenizer const& other);
    bool            char_is(char expected_char);
    void            drop_char();
    void            drop_token();
    void            drop_ws();
    std::string     get_token();
    bool            token_is(char const* pstr);

  private:
    typedef std::pair<std::string::iterator,std::string::iterator> token_limits;

    token_limits    _get_token_limits();
    void            _internal_copy(bool_tokenizer const& other);

    std::string::iterator
                    _pos;
    std::string     _text;
  };
}

CCB_END()

#endif // !CCB_BOOL_TOKENIZER_HH
