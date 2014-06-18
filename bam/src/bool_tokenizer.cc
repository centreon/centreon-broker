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

#include <algorithm>
#include "com/centreon/broker/bam/bool_tokenizer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Returns a trimmed string with NO whitespace before OR after
 *    ..ie..  "   Hello world  "  -> "Hello World".
 *
 *  @param[in] trim_me String to trim.
 *
 *  @return Trimmed string.
 */
static std::string trim(std::string const& trim_me) {
  char const* ws(" \t\n\r");
  std::size_t from(trim_me.find_first_not_of(ws));
  std::size_t to(trim_me.find_last_not_of(ws));

  return (((from == std::string::npos)
           || (to == std::string::npos))
          ? std::string()
          : trim_me.substr(from, to - from + 1));
}

/**
 *  Constructor.
 */
bool_tokenizer::bool_tokenizer(std::string const& text)
  : _text(text) {
  _pos = _text.begin();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
bool_tokenizer::bool_tokenizer(bool_tokenizer const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
bool_tokenizer::~bool_tokenizer() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
bool_tokenizer& bool_tokenizer::operator=(bool_tokenizer const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Check what current char is.
 *
 *  @param[in] expected_char This is the expected char value.
 *
 *  @return    Whether the expected_char is the current char or not.
 */
bool bool_tokenizer::char_is(char expected_char) {
  drop_ws();
  return ((_pos != _text.end()) ? (*_pos == expected_char) : false);
}

/**
 *  Drop next character.
 */
void bool_tokenizer::drop_char() {
  if (_pos != _text.end())
    ++_pos;
  return ;
}

/**
 *  Drop next token.
 */
void bool_tokenizer::drop_token() {
  get_token();
  return ;
}

/**
 *  Drop whitespaces.
 */
void bool_tokenizer::drop_ws() {
  std::string::iterator end(_text.end());
  while ((_pos != end) && isspace(*_pos))
    ++_pos;
  return ;
}

/**
 *  Gets the current token and moves to the next.
 *
 *  @return Returns the current token.
 */
std::string bool_tokenizer::get_token() {
  token_limits tl(_get_token_limits());
  _pos = tl.second + 1; // Move _pos to after the '}' char.
  return (trim(std::string(tl.first, tl.second)));
}

/**
 *  Check current token.
 *
 *  @param[in] pstr  This is the expected current token.
 *
 *  @return    Whether the string argument is the current token or not.
 */
bool bool_tokenizer::token_is(char const* pstr) {
  token_limits tl(_get_token_limits());
  return (trim(std::string(tl.first, tl.second)) == std::string(pstr));
}

/**
 *  Get token limits.
 *
 *  @return  Returns a pair having values (first, last) meaning that the
 *           first points to the first char and the last points to the
 *           char following the last char, according to STL semantics.
 */
bool_tokenizer::token_limits bool_tokenizer::_get_token_limits() {
  // Precondition : we have a token start.
  drop_ws();
  if (!char_is('{'))
    throw (exceptions::msg() << "cannot find beginning of a valid token");

  std::string::iterator from(_pos + 1);
  std::string::iterator to(std::find(from, _text.end(), '}'));
  if (to == _text.end())
    throw (exceptions::msg() << "cannot find end of token");

  return (std::make_pair(from, to));
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void bool_tokenizer::_internal_copy(bool_tokenizer const& other) {
  _pos = other._pos;
  _text = other._text;
  return ;
}
