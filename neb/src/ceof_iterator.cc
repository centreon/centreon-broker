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

#include "com/centreon/broker/neb/ceof_parser.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
ceof_iterator::ceof_iterator()
  : _tokens(NULL),
    _token_number(0),
    _index(0) {

}

/**
 *  Constructor.
 *
 *  @param[in] tokens        A vector of tokens.
 *  @param[in] token_number  The number of tokens.
 */
ceof_iterator::ceof_iterator(const ceof_token* tokens, size_t token_number)
  : _tokens(tokens),
    _token_number(token_number),
    _index(0) {
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
ceof_iterator::ceof_iterator(ceof_iterator const& other) {
  _tokens = other._tokens;
  _token_number = other._token_number;
  _index = other._index;
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           A reference to this object.
 */
ceof_iterator& ceof_iterator::operator=(ceof_iterator const& other) {
  if (this != &other) {
    _tokens = other._tokens;
    _token_number = other._token_number;
    _index = other._index;
  }
  return (*this);
}

/**
 *  Destructor.
 */
ceof_iterator::~ceof_iterator() throw() {

}

/**
 *  Comparison operator.
 *
 *  @param[in] other  The object to compare with.
 *
 *  @return           True if both objects are equal.
 */
bool ceof_iterator::operator==(ceof_iterator const& other) const throw() {
  return (_tokens == other._tokens
            && _token_number
            && other._token_number && _index == other._index);
}

/**
 *  Comparison unequality.
 *
 *  @param[in] other  The object to compare with.
 *
 *  @return           True if the object are not equal.
 */
bool ceof_iterator::operator!=(ceof_iterator const& other) const throw() {
  return (!operator ==(other));
}

/**
 *  @brief Go forward one token.
 *
 *  Skip children tokens.
 *
 *  @return  Reference to this object.
 */
ceof_iterator& ceof_iterator::operator++() throw() {
  unsigned int parent_token = _tokens[_index].get_parent_token();
  for (++_index;
       _index < _token_number
         && _tokens[_index].get_parent_token() != parent_token;
       ++_index);
}

/**
 *  Get the type of the current token.
 *
 *  @return  The type of the current token.
 */
ceof_token::token_type ceof_iterator::get_type() const throw() {
  return (_tokens[_index].get_token_type());
}

/**
 *  Get the value of the current token.
 *
 *  @return  The value of the current token.
 */
std::string const& ceof_iterator::get_value() const throw() {
  return (_tokens[_index].get_value());
}

/**
 *  Does this iterator has children?
 *
 *  @return  True if this iterator has children.
 */
bool ceof_iterator::has_children() const throw() {
  size_t next_index = _index + 1;
  return (next_index < _token_number
            && _tokens[next_index].get_parent_token() == _index);
}

/**
 *  Enter the children.
 *
 *  @return  An iterator to the children.
 */
ceof_iterator ceof_iterator::enter_children() const throw() {
  return (has_children() ?
            ceof_iterator(_tokens + _index + 1, _token_number - _index - 1)
              : ceof_iterator());
}

/**
 *  Is this an end iterator?
 *
 *  @return  True if this an end iterator.
 */
bool ceof_iterator::end() const throw() {
  return (!_tokens || _token_number == _index);
}
