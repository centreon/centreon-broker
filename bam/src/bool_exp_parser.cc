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

#include "com/centreon/broker/bam/bool_exp_parser.hh"
#include "com/centreon/broker/bam/bool_and.hh"
#include "com/centreon/broker/bam/bool_xor.hh"
#include "com/centreon/broker/bam/bool_or.hh"
#include "com/centreon/broker/bam/bool_not.hh"
#include "com/centreon/broker/bam/bool_service.hh"
#include "com/centreon/broker/bam/parse_exception.hh"
#include "com/centreon/defines.hh"
#include "com/centreon/tools/create_map.hh"
#include "com/centreon/broker/bam/tokeniser.hh"
#include "com/centreon/broker/bam/bool_exp_parser.hh"

#include <iostream>
#include <sstream>
#include <locale>
#include <algorithm>
#include <functional>
#include <locale>
#include <memory>
#include <map>

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;
using namespace com::centreon::tools;

////////////////////SPECIFICATION///////////////////////
//        .......CONCRETE GRAMMAR : an example.......

//({Server Cmd}{not}{OK}{AND}{Server-titi Cpu}{is}{WARNING})
//   {OR} {NOT}{Server2 Cmd2}{is}{CRITICAL}


//     .....BNF : the abstract grammar ..............
/*
  ws = {' '|tab}
  sym = <letter>, {<letter>|<digit>}

  be = '{',("is"|"not"),'}'
  not= '{',"NOT",'}'
  result = '{',<ws>,("WARNING"|"OK"|"UNKNOWN"|"CRITICAL"),<ws>,'}'
  host_service = '{',<ws>,<sym>,<ws>,<sym>,'}'
  host_service_state = <host_service>,<ws>,<be>,<ws>,<result>
  op = '{',("AND"|"OR"|"XOR"),'}'
  term = <host_service_state>
  |      '(',<bool_exp>,')'
  |      <not>,<term>
  bool_exp =<ws>,<term>,{<ws,<op>,<ws>,<term>}
*/
///////////////////////////////////////////////////////



/**
 *  Constructor
 *
 */
bool_exp_parser::bool_exp_parser(std::string const&  exp_text){
  tokeniser data( exp_text);
  _root = _make_boolean_exp( data );
}

/**
 * Copy constructor
 *
 */
bool_exp_parser::bool_exp_parser( bool_exp_parser const & other)
  : _root( other._root){}

/**
 * Destructor
 *
 */
bool_exp_parser::~bool_exp_parser(){}

/**
 * Assignment Operator
 *
 */
bool_exp_parser& bool_exp_parser::operator=( bool_exp_parser const& other){
  if (this != &other){
    _root = other._root;
  }
  return *this;
}

/**
 *  eat boolean expression
 *
 *  @brief Consumes all terms at a certain level of scope
 *         and returns a bool_value
 *
 *  @param1  The tokeniser that holds the tokens for the terms.
 *  @return A bool_value that abstracts all the boolean exp.
 */
bool_value::ptr bool_exp_parser::_make_boolean_exp
                               ( tokeniser& data ){

  bool_value::ptr result = _make_term( data );
  bool_binary_operator::ptr op     = _make_op( data );
  // We read until we have consumed all terms AT a given level
  while ( !op.isNull()  ){

    bool_value::ptr right_term = _make_term( data );
    op->set_left( result );
    op->set_right( right_term);
    result = op;
    op =  _make_op( data );
  }

  return result;
}

/*
 *  Make operator
 *
 *  @brief   This method consumes an operator token
 *
 *  @param1  The tokeniser that holds the operator token.
 *  @return  A binary operator object
 */
bool_binary_operator::ptr bool_exp_parser::_make_op(tokeniser& data){

  // Note: it is perfectly legal to return an empty operator
  //       this denotes the end of an embedded boolean expression
  bool_binary_operator::ptr result;

  if( data.char_is('{') ){
    std::string sym = data.get_token();
    std::transform ( sym.begin(), sym.end(), sym.begin(), toupper);

    if ( !sym.empty()  ){
      if ( sym == "AND")
        result =  new bool_and();
      if ( sym == "OR" )
        result =  new bool_or();
      if ( sym == "XOR" )
        result =  new bool_xor();
      else
        throw parse_exception() << "bam: illegal operator = "
                                << sym <<" in boolean parse";
    }
  }
  return result;
}






/*
 *  Eat term
 *
 *  @brief   This method consumes a term expression
 *  @return  A bool value object representing the consumed token
 */
bool_value::ptr bool_exp_parser::_make_term(tokeniser& data){

  bool_value::ptr result;

  if ( data.char_is('{') ){
    if ( data.token_is("NOT") ){
      //NOT operator
      data.drop_token();
      result = new bool_not( _make_term(data)  );
    }
    else{
      //Host_service construct
      result = _make_host_service_state( data);
    }
  }
  else if ( data.char_is('(') ) {
    data.drop_char();
    result = _make_boolean_exp( data );
    data.assert_char(')');
    data.drop_char();
  }
  else{
    throw parse_exception() << " illegal";
  }

  data.drop_ws();
}

  /*  _eat_ws();
  switch( data.hasScopepeek_char() ){
  case: '(':
    {

            _eat_char();
      _eat_ws();
      result = _eat_boolean_exp();
      eat_bracket.ensure();
      _eat_ws();
      if ( _eat_char() != ')' )
        throw parser_exception();
    }
  case: '{':
    {
      const string sym = _view_curr_token();
      if ( sym =="NOT" ){
        _eat_token();
        result = new bool_not( _eat_term() );
      }
      else{
        result = _eat_host_service_state();
      }
    }
  }
  _eat_ws();
*/




/*
 *  Make host service state
 *
 *  @brief  turns a token into a boolean node
 *  @return Return bool value
 */
bool_value::ptr bool_exp_parser::_make_host_service_state(tokeniser& data){
  //  FORMAT example   {HOST SERV}{is}{UNKNOWN}

  // get host serv
  std::stringstream ss (  data.get_token().c_str()  );
  short host;
  short service;
  ss >> host;
  if( !ss) throw parse_exception();
  ss >> service;
  if( !ss) throw parse_exception();

  // condition whether state desired OR not
  bool is_expected =  _token_2_condition( data.get_token()  );

  // get referred state
  e_service_state state = _token_2_service_state( data.get_token() );

  bool_service::ptr ret( new bool_service(host, service) );
  ret->set_value_if_state_match( is_expected );
  ret->set_expected_state( state );
  return ret;
}




e_service_state  bool_exp_parser::_token_2_service_state(std::string const& token){

 //uppercase the token
  std::string serv_state ( token );
  std::transform ( serv_state.begin(), serv_state.end(), serv_state.begin(), toupper);

  typedef std::map<std::string, e_service_state> string2state;

  static string2state token_2_state =
    create_map <std::string, e_service_state>
    ("OK"      , E_STATE_OK  )
    ("WARNING" , E_STATE_WARNING  )
    ("CRITICAL", E_STATE_CRITICAL  )
    ("UNKNOWN" , E_STATE_UNKNOWN  );

  string2state::iterator it =  token_2_state.find(serv_state);
  if ( it == token_2_state.end() )
    throw parse_exception() << "bam: illegal state in boolean expression:"
                            << serv_state;
  return it->second;
}


bool  bool_exp_parser::_token_2_condition ( std::string const& token){
 // get condition "is" OR "not"
  std::string be (token);
  std::transform ( be.begin(), be.end(), be.begin(), toupper);

  if ( be == "IS")
    return true;
  else if ( be == "NOT")
    return false;
  throw parse_exception() << "bam: illegal condition in boolean expression:"
                          << be ;

}



/*

bool_not


*/

