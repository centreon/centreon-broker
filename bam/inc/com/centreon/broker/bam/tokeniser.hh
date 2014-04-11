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

#ifndef CCB_BAM_BOOL_TOKENISER_HH
#  define CCB_BAM_BOOL_TOKENISER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam{
  class tokeniser{

  public:

    tokeniser(std::string const & text);
    bool         char_is(char expected_char);
    bool         token_is( char const * pStr);
    std::string  get_token() ;
    void         assert_char(char);
    void         drop_char();
    void         drop_token();
    void         drop_ws();
    void         eat_ws();
  private:
    typedef std::pair<std::string::iterator,std::string::iterator> token_limits;
    token_limits          _get_token_limits();
    std::string           _text;
    std::string::iterator _currPos;
    std::string           _view_curr_token();
    std::string           _eat_curr_token();
  };
}

CCB_END()

#endif
