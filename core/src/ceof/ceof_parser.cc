/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/ceof/ceof_parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::ceof;

/**
 *  Constructor.
 *
 *  @param[in] str  The string to parse.
 */
ceof_parser::ceof_parser(std::string const& str) : _string(str) {}

/**
 *  Destructor.
 */
ceof_parser::~ceof_parser() throw() {}

/**
 *  Skip comments and whitespaces.
 *
 *  @param[in,out] actual  The actual index.
 */
static void skip(size_t& actual,
                 std::string const& string,
                 const char* characters) {
  actual = string.find_first_not_of(characters, actual);
  if (actual == std::string::npos)
    return;
  if (string[actual] == '#') {
    actual = string.find_first_of('\n', actual);
    if (actual == std::string::npos)
      return;
    ++actual;
    skip(actual, string, characters);
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
  skip(actual, _string, " \t\n");
  int parent_token = -1;
  while (actual != std::string::npos) {
    // Get the token.
    size_t end_of_token = _string.find_first_of(
        state == in_object_waiting_for_value ? "\n" : " \t\n", actual);
    if (end_of_token == std::string::npos)
      end_of_token = _string.size();
    std::string substr = _string.substr(actual, end_of_token - actual);

    switch (state) {
      case waiting_for_define:
        if (substr != "define")
          throw(exceptions::msg()
                << "expected 'define' at position " << actual);
        state = waiting_for_object_name;
        break;
      case waiting_for_object_name:
        parent_token = _tokens.size();
        _tokens.push_back(
            ceof_token(ceof_token::object, substr, parent_token, -1));
        state = waiting_for_object_opening;
        break;
      case waiting_for_object_opening:
        if (substr != "{")
          throw(exceptions::msg() << "expected '{' at position " << actual);
        state = in_object_waiting_for_key;
        break;
      case in_object_waiting_for_key:
        if (substr == "}")
          state = waiting_for_define;
        else {
          _tokens.push_back(ceof_token(ceof_token::key, substr, _tokens.size(),
                                       parent_token));
          state = in_object_waiting_for_value;
        }
        break;
      case in_object_waiting_for_value:
        if (substr == "}")
          throw(exceptions::msg()
                << "expected value instead of '{' at position " << actual);
        size_t trimmed(substr.find_last_not_of(" \t"));
        substr =
            substr.substr(0, (trimmed == std::string::npos) ? std::string::npos
                                                            : trimmed + 1);
        _tokens.push_back(ceof_token(ceof_token::value, substr, _tokens.size(),
                                     parent_token));
        state = in_object_waiting_for_key;
    }
    // Skip to the next token.
    actual = end_of_token;
    skip(actual, _string,
         state == in_object_waiting_for_value ? " \t" : " \t\n");
  }

  return (ceof_iterator(_tokens.begin(), _tokens.end()));
}
