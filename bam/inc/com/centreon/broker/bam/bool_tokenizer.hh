/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
