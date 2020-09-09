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

#include "com/centreon/broker/bam/exp_tokenizer.hh"

#include <cctype>

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
 *  @return Next token. Empty if end of expression is reached.
 */
std::string exp_tokenizer::next() {
  // If queue is not empty, tokens are already available.
  std::string retval;
  if (!_queue.empty()) {
    retval = _queue.front();
    _queue.pop();
  }
  // Extract new token.
  else {
    // Move to next token.
    _current = _next;
    _skip_ws();
    _next = _current;

    // Check if there are more tokens to read.
    if (_current < _size) {
      // Special char. Most of them are single-charaters token.
      if (_is_special_char()) {
        if ((_current + 1 < _size)
            // Double character exceptions: ==, !=, >=, <=.
            && ((((_text[_current] == '=') || (_text[_current] == '!') ||
                  (_text[_current] == '<') || (_text[_current] == '>')) &&
                 (_text[_current + 1] == '='))
                // Double character exceptions: ||, &&.
                || (((_text[_current] == '|') || (_text[_current] == '&')) &&
                    (_text[_current] == _text[_current + 1])))) {
          retval.push_back(_text[_current]);
          retval.push_back(_text[_current + 1]);
          _next = _current + 2;
        } else {
          retval.push_back(_text[_current]);
          _next = _current + 1;
        }
      }
      // This seems like a classical token, extract it.
      else {
        retval = _extract_token();
      }
    }
  }
  return (retval);
}

/**
 *  Extract next token from text.
 *
 *  @return Next token.
 */
std::string exp_tokenizer::_extract_token() {
  // Return value.
  std::string retval;

  /*
  ** Well, the whole brace parsing is inherited from the old
  ** "{Host Service} {IS} {OK}"-style syntax which SUCKS. The following
  ** code should not exist at all or maybe should be placed within the
  ** parser. However the parser being already complex enough, this
  ** seemed like a good place.
  */

  // Brace-delimited token, this is special.
  if ((_current < _size) && (_text[_current] == '{')) {
    // Extract token.
    ++_current;
    retval = _extract_until(&exp_tokenizer::_is_space_or_right_brace);

    // If it's an operator or a constant, leave it as it is.
    // Otherwise this express the STATUS of a host/service.
    if ((retval != "IS") && (retval != "NOT") && (retval != "AND") &&
        (retval != "XOR") && (retval != "OR") && (retval != "OK") &&
        (retval != "WARNING") && (retval != "CRITICAL") &&
        (retval != "UNKNOWN") && (retval != "UP") && (retval != "DOWN") &&
        (retval != "UNREACHABLE")) {
      // Host name is retrieved first.
      _queue.push("(");
      _queue.push(retval);
      _current = _next;
      _skip_ws();
      _next = _current;

      // Extract service if possible.
      if ((_current < _size) && (_text[_current] != '}')) {
        retval = _extract_until(&exp_tokenizer::_is_right_brace);
        _queue.push(",");
        _queue.push(retval);
        retval = "SERVICESTATUS";
      }
      // No service, wish to retrieve host status.
      else {
        retval = "HOSTSTATUS";
      }
      _queue.push(")");
    }

    // Assert that ending brace was found.
    if ((_next < _size) && (_text[_next] == '}')) {
      ++_next;
    } else
      throw(exceptions::msg() << "opening brace at position " << _current
                              << " has no ending brace ");
  }
  // Extract classical token.
  else {
    retval = _extract_until(&exp_tokenizer::_is_delimiter);
  }

  return (retval);
}

/**
 *  Extract a token until a predicate.
 *
 *  @param[in] predicate  Predicate method.
 *
 *  @return Extracted token.
 */
std::string exp_tokenizer::_extract_until(bool (exp_tokenizer::*predicate)()) {
  std::string retval;
  _next = _current;
  while ((_next < _size) && !(this->*predicate)()) {
    switch (_text[_next]) {
      case '\'':
      case '"': {
        char quote(_text[_next]);
        bool process_metachars(quote == '"');
        bool quote_matched(false);
        while ((++_next < _size)) {
          if (_text[_next] == quote) {
            quote_matched = true;
            break;
          }
          if (process_metachars && (_text[_next] == '\\')) {
            ++_next;
            if (_next < _size) {
              retval.push_back(_text[_next]);
            }
          } else {
            retval.push_back(_text[_next]);
          }
        }
        if (!quote_matched)
          throw(exceptions::msg()
                << "unterminated " << (process_metachars ? "double" : "single")
                << " quote in the following expression: " << _text);
      } break;
      case '\\':
        if (++_next < _size)
          retval.push_back(_text[_next]);
        break;
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
  _queue = other._queue;
  _size = other._size;
  _text = other._text;
  return;
}

/**
 *  Check if the next character is a right brace.
 *
 *  @return True if the next character is a right brace.
 */
bool exp_tokenizer::_is_right_brace() {
  return (_text[_next] == '}');
}

/**
 *  Check if the next character is a right brace or a space.
 *
 *  @return True if the next character is a right brace or a space.
 */
bool exp_tokenizer::_is_space_or_right_brace() {
  return (isspace(_text[_next]) || _is_right_brace());
}

/**
 *  Check if the next character is a special character for the lexer.
 *
 *  @return True if the next character is special.
 */
bool exp_tokenizer::_is_special_char() {
  switch (_text[_next]) {
    case '(':
    case ')':
    case ',':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
    case '!':
    case '>':
    case '<':
    case '|':
    case '&':
      return (true);
  }
  return (false);
}

/**
 *  Check if the next character is a delimiter.
 *
 *  @return True if the next character is a delimiter.
 */
bool exp_tokenizer::_is_delimiter() {
  return (isspace(_text[_next]) || _is_special_char());
}

/**
 *  Skip whitespaces.
 */
void exp_tokenizer::_skip_ws() {
  while (isspace(_text[_current])) {
    ++_current;
  }
  return;
}
