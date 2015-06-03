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

#include "com/centreon/broker/neb/ceof_token.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
ceof_token::ceof_token() {

}

/**
 *  Constructor.
 *
 *  @param[in] type          Type of the token.
 *  @param[in] val           Value of the token.
 *  @param[in] token_number  The number of the token.
 *  @param[in] parent        The index of the parent token.
 */
ceof_token::ceof_token(
              token_type type,
              std::string const& val,
              unsigned int token_number,
              int parent)
  : _type(type),
    _value(val),
    _token_number(token_number),
    _parent_token(parent) {

}

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
ceof_token::~ceof_token() throw() {

}

/**
 *  Get the type of this token.
 *
 *  @return  The type of this token.
 */
ceof_token::token_type  ceof_token::get_type() const throw() {
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
unsigned int ceof_token::get_token_number() const throw() {
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
