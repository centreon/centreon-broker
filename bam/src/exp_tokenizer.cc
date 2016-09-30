/*
** Copyright 2016 Centreon
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

#include <cctype>
#include "com/centreon/broker/bam/exp_tokenizer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 */
exp_tokenizer::exp_tokenizer(std::string const& text)
  : _current(0), _next(0), _text(text) {
  _size = _text.size();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
exp_tokenizer::exp_tokenizer(exp_tokenizer const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
exp_tokenizer::~exp_tokenizer() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
exp_tokenizer& exp_tokenizer::operator=(exp_tokenizer const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Get next token.
 *
 *  @return Next token. Empty is end of expression is reached.
 */
std::string exp_tokenizer::next() {
  // Move to next token.
  _current = _next;
  _skip_ws();

  // Check if there are more tokens to read.
  std::string retval;
  if (_current < _size) {
    switch (_text[_current]) {
      // Token start/end, parenthisis, semicolon, ... no problem.
      case '{':
      case '}':
      case '(':
      case ')':
      case ':':
      case ',':
        retval.push_back(_text[_current]);
        _next = _current + 1;
        break ;
      // This seems like a classical token, extract it.
      default:
        retval = _extract_token();
    }
  }
  return (retval);
}

/**
 *  Extract next token from text.
 *
 *  @return Next token.
 */
std::string exp_tokenizer::_extract_token()
{
  std::string retval;
  _next = _current;
  while ((_next < _size)
         && !isspace(_text[_next])
         && !_is_special_char(_text[_next])) {
    switch (_text[_next]) {
      case '\'':
      case '"':
        {
          char quote(_text[_next]);
          bool process_metachars(quote == '"');
          bool quote_matched(false);
          while ((++_next < _size)) {
            if (_text[_next] == quote) {
              quote_matched = true;
              break ;
            }
            if (process_metachars && (_text[_next] == '\\')) {
              ++_next;
              if (_next < _size) {
                retval.push_back(_text[_next]);
              }
            }
            else {
              retval.push_back(_text[_next]);
            }
          }
          if (!quote_matched)
            throw (exceptions::msg() << "unterminated "
                   << (process_metachars ? "double" : "single")
                   << " quote in the following expression: "
                   << _text);
        }
        break ;
      case '\\':
        if (++_next < _size)
          retval.push_back(_text[_next]);
        break ;
      default:
        retval.push_back(_text[_next]);
    }
    ++_next;
  }
  return (retval);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void exp_tokenizer::_internal_copy(exp_tokenizer const& other) {
  _current = other._current;
  _next = other._next;
  _size = other._size;
  _text = other._text;
  return ;
}

/**
 *  Check if character is a special character for the lexer.
 *
 *  @return True if the character is special.
 */
bool exp_tokenizer::_is_special_char(char c) {
  switch (c) {
    case '{':
    case '}':
    case '(':
    case ')':
    case ':':
    case ',':
      return (true);
  }
  return (false);
}

/**
 *  Skip whitespaces.
 */
void exp_tokenizer::_skip_ws() {
  while (isspace(_text[_current])) {
    ++_current;
  }
  return ;
}
