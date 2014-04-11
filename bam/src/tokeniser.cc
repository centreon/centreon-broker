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

#include "com/centreon/broker/bam/tokeniser.hh"
#include "com/centreon/broker/bam/parse_exception.hh"
#include <algorithm>

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;
//using namespace com::centreon::tools;


/**
 *  @function trim
 *
 *  @brief returns a trimmed string with NO whitespace before OR after
 *         ..ie..  "   Hello world  "  -> "Hello World"
 */
std::string trim(const std::string& trim_me){
  using namespace std;

  const char * ws=" \t\n\r";
  int from = trim_me.find_first_not_of(ws);
  from = ( from == string::npos ? trim_me.size() : from);

  int to   = trim_me.find_last_not_of (ws, from);
  to = (to==string::npos ? trim_me.size() : to);

  return trim_me.substr( from,to - from);
}

/*
 *
 *@brief  This class returns tokens from a text of string
 */

tokeniser::tokeniser(std::string const & text)
    : _text( text), _currPos(_text.begin() )  {}

  /*
   * current char is
   * @param[in] expected_char This is the expected char value
   * @return    Whether the expected_char is the current char or not
   */
  bool tokeniser::char_is(char expected_char){
    eat_ws();
    return _currPos!=_text.end() ? *_currPos == expected_char : false;
  }

  /*
   * get token  limits
   * @return    Returns a pair having values (first, last] meaning that the first points to the first char
   *            and the last points to the char following the last char.. according to STL semantics
   * @exception In the case of an illegal token, an exception is thrown
   */
  tokeniser::token_limits tokeniser::_get_token_limits(){
    //precondition : we have a token start
    eat_ws();
    if (!char_is('{') )
      throw parse_exception() << "Bam: bool expression parser cannot find beginning of a valid token";

    std::string::iterator from = _currPos + 1;
    std::string::iterator to =  std::find( from, _text.end(), '}');

    if ( to == _text.end() )
      throw parse_exception() << "Bam: bool expression parser cannot find end of a valid token";

    return std::make_pair( from, to );
  }

  /*
   * current token value is
   * @param[in] pStr  This is the expected current token
   * @return    Whether the pStr is the current token or not
   */
  bool tokeniser::token_is( char const * pStr){

    token_limits tl = _get_token_limits();
    return trim( std::string( tl.first, tl.second ) ) == std::string( pStr);
  }

  /*
   * get token   Gets the current token and moves to the next.
   * @return    returns the current token
   */
  std::string tokeniser::get_token() {
    token_limits tl = _get_token_limits();
    _currPos = tl.first + 1; // move currPos to after the '}' char
    return trim( std::string( tl.first, tl.second ) );
  }
  void tokeniser::assert_char(char){};
  void tokeniser::drop_char(){}
  void tokeniser::drop_token(){}
  void tokeniser::drop_ws(){}
  void tokeniser::eat_ws(){

    //    while ( _text.
    /* while ( _curr != _end && isspace(**_curr)  ){
       _curr++;
       }*/
  }
  std::string tokeniser::_view_curr_token(){/*
                                   _eat_ws();
                                   if( _eat_char() != '{' )
                                   throw parser_exception();
                                   _eat_ws();
                                   std::string::iterator begin_token = _curr;
                                   std::string::iterator end_token = _text.find( _curr, '}');
                                   if ( end_token == string::npos)
                                   throw parser_exception();
                                   _curr = end_token;
                                   _eat_char();
                                   _eat_ws;
                                   return std::string sym ( begin_token, end_token);*/
  }

  std::string tokeniser::_eat_curr_token(){/*

                                //this ensure that '{' and '}' are consumed
                                scope_eater eat_this('{','}', this );

                                std::string::iterator begin_token = _curr;
                                std::string::iterator end_token = _text.find( _curr, '}');

                                //  if ( end_token == std::string::npos)
                                //  throw parse_exception();

                                eat_this.

                                return std::string ( begin_token, end_token);*/
  }

