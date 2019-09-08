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

#include "com/centreon/broker/ceof/ceof_token.hh"

using namespace com::centreon::broker::ceof;

/**
 *  Default constructor.
 */
ceof_token::ceof_token() {}

/**
 *  Constructor.
 *
 *  @param[in] type          Type of the token.
 *  @param[in] val           Value of the token.
 *  @param[in] token_number  The number of the token.
 *  @param[in] parent        The index of the parent token.
 */
ceof_token::ceof_token(token_type type,
                       std::string const& val,
                       int token_number,
                       int parent)
    : _type(type),
      _value(val),
      _token_number(token_number),
      _parent_token(parent) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ceof_token::ceof_token(ceof_token const& other) {
  _type = other._type;
  _value = other._value;
  _token_number = other._token_number;
  _parent_token = other._parent_token;
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return           Reference to this object.
 */
ceof_token& ceof_token::operator=(ceof_token const& other) {
  if (this != &other) {
    _type = other._type;
    _value = other._value;
    _token_number = other._token_number;
    _parent_token = other._parent_token;
  }
  return (*this);
}

/**
 *  Destructor.
 */
ceof_token::~ceof_token() throw() {}

/**
 *  Get the type of this token.
 *
 *  @return  The type of this token.
 */
ceof_token::token_type ceof_token::get_type() const throw() {
  return (_type);
}

/**
 *  Get the value of this token.
 *
 *  @return  The value of this token.
 */
std::string const& ceof_token::get_value() const throw() {
  return (_value);
}

/**
 *  Get the token number.
 *
 *  @return  Get the token number.
 */
int ceof_token::get_token_number() const throw() {
  return (_token_number);
}

/**
 *  Get the parent token number.
 *
 *  @return  Parent token number.
 */
int ceof_token::get_parent_token() const throw() {
  return (_parent_token);
}
