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

#ifndef CCB_BAM_BOOL_EXP_PARSER_HH
#  define CCB_BAM_BOOL_EXP_PARSER_HH

#  include <algorithm>
#  include <iostream>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/bam/bool_binary_operator.hh"
#  include "com/centreon/defines.hh"

CCB_BEGIN()

namespace        bam {


  //  Declaration
  class tokeniser;



  /**
   *  @class bool_exp_parser bool_exp_parser.hh "com/centreon/broker/bam/bool_exp_parser.hh"
   *  @brief A boolean expression parser for interpreting BAM expressions
   *         consisting of a boolean-type syntax.
   */
  class bool_exp_parser{
  public:

    bool_exp_parser(std::string const& exp_text);
    bool_exp_parser(bool_exp_parser const&);
    ~bool_exp_parser();
    bool_exp_parser& operator=( bool_exp_parser const&);


  private:
    bool_value::ptr _root;
    std::string::const_iterator _curr;
    std::string::const_iterator _end;
    std::string _text;

    // internal class for calculating tokens

    void                      _assert_char(char);
    void                      _drop_char();
    bool_value::ptr           _make_boolean_exp(tokeniser &data);
    bool_binary_operator::ptr _make_op(tokeniser &data);
    bool_value::ptr           _make_term(tokeniser &data);
    bool_value::ptr           _make_host_service_state(tokeniser& data);

    e_service_state           _token_2_service_state(std::string const& token);
    bool                      _token_2_condition ( std::string const& token);

  };

}

CCB_END()

#endif // !CCB_BAM_BOOL_EXP_PARSER_HH
