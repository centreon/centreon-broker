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
 *  @param[in] type    Type of the token.
 *  @param[in] val     Value of the token.
 *  @param[in] parent  The index of the parent token.
 */
ceof_token::ceof_token(
              token_type type,
              std::string const& val,
              unsigned int parent)
  : _type(type),
    _value(val),
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
ceof_token::token_type  ceof_token::get_token_type() const throw() {
  return (_type);
}

/**
 *  Set the type of this token.
 *
 *  @param[in] type  The type of this token.
 */
void ceof_token::set_token_type(token_type type) throw() {
  _type = type;
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
 *  Set the value of this token.
 *
 *  @param[in] val  The value of this token.
 */
void ceof_token::set_value(std::string const& val) {
  _value = val;
}

/**
 *  Get the parent token number.
 *
 *  @return  Parent token number.
 */
unsigned int ceof_token::get_parent_token() const throw() {
  return (_parent_token);
}

/**
 *  Set the parent token number.
 *
 *  @param[in] parent  The parent token number.
 */
void ceof_token::set_parent_token(unsigned int parent) throw() {
  _parent_token = parent;
}
