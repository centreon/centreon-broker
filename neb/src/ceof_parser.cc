/*
** Copyright 2009-2013,2015 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/ceof_parser.hh"

using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[in] str  The string to parse.
 */
ceof_parser::ceof_parser(std::string const& str)
  : _string(str) {

}

/**
 *  Destructor.
 */
ceof_parser::~ceof_parser() throw() {

}

/**
 *  Skip comments and whitespaces.
 *
 *  @param[in,out] actual  The actual index.
 */
static void skip(size_t& actual, std::string const& string) {
  actual = string.find_first_not_of(" \t\n", actual);
  if (actual != std::string::npos)
    return ;
  while (string[actual] == '#') {
    ++actual;
    skip(actual, string);
  }
}

/**
 *  @brief Parse the string given to the parser.
 *
 *  This tokenize the document and returns an iterator to iterate over it.
 *
 *  @return  An iterator to iterate over the document.
 */
ceof_iterator ceof_parser::parse() {
  // Clear the tokens.
  _tokens.clear();

  // We tokenize the string using a simple state machine.
  // Declare the state machine.
  enum {
    waiting_for_define,
    waiting_for_object_name,
    waiting_for_object_opening,
    in_object_waiting_for_key,
    in_object_waiting_for_value
  } state = waiting_for_define;

  // Parse the string.
  size_t actual = 0;
  skip(actual, _string);
  int parent_token = -1;
  while (actual != std::string::npos) {
    // Get the token.
    size_t end_of_token = _string.find_first_of(" \t\n", actual);
    std::string substr = _string.substr(actual, end_of_token - actual);

    switch (state) {
    case waiting_for_define:
      if (substr != "define")
        throw (exceptions::msg() << "expected 'define' at position " << actual);
      state = waiting_for_object_name;
      break;
    case waiting_for_object_name:
      _tokens.push_back(
        ceof_token(ceof_token::object, substr, _tokens.size(), parent_token));
      parent_token = _tokens.size();
      state = waiting_for_object_opening;
      break;
    case waiting_for_object_opening:
      if (substr != "{")
        throw (exceptions::msg() << "expected '{' at position " << actual);
      state = in_object_waiting_for_key;
      break;
    case in_object_waiting_for_key:
      if (substr == "}") {
        parent_token = _tokens[parent_token].get_parent_token();
        state = waiting_for_define;
      }
      else {
        _tokens.push_back(
          ceof_token(ceof_token::key, substr, _tokens.size(), parent_token));
        state = in_object_waiting_for_value;
      }
      break;
    case in_object_waiting_for_value:
      if (substr == "}")
        throw (exceptions::msg()
               << "expected value instead of '{' at position " << actual);
      _tokens.push_back(
        ceof_token(ceof_token::value, substr, _tokens.size(), parent_token));
      state = in_object_waiting_for_key;
    }
    // Skip to the next token.
    skip(actual, _string);
  }

  return (ceof_iterator(_tokens.begin(), _tokens.end()));
}
