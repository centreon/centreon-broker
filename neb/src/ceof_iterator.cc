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
ceof_iterator::ceof_iterator() {

}

/**
 *  Constructor.
 *
 *  @param[in] tokens        A vector of tokens.
 *  @param[in] token_number  The number of tokens.
 */
ceof_iterator::ceof_iterator(
                 std::vector<ceof_token>::const_iterator const& begin,
                 std::vector<ceof_token>::const_iterator const& end)
  : _token_it(begin),
    _token_end(end) {
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
ceof_iterator::ceof_iterator(ceof_iterator const& other) {
  _token_it = other._token_it;
  _token_end = other._token_end;
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
    _token_it = other._token_it;
    _token_end = other._token_end;
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
  return (_token_it == other._token_it && _token_end == other._token_end);
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
  unsigned int parent_token = _token_it->get_parent_token();
  for (++_token_it;
       _token_it != _token_end
         && _token_it->get_parent_token() != parent_token;
       ++_token_it);
}

/**
 *  Get the type of the current token.
 *
 *  @return  The type of the current token.
 */
ceof_token::token_type ceof_iterator::get_type() const throw() {
  return (_token_it->get_type());
}

/**
 *  Get the value of the current token.
 *
 *  @return  The value of the current token.
 */
std::string const& ceof_iterator::get_value() const throw() {
  return (_token_it->get_value());
}

/**
 *  Does this iterator has children?
 *
 *  @return  True if this iterator has children.
 */
bool ceof_iterator::has_children() const throw() {
  unsigned int token_number = _token_it->get_token_number();
  std::vector<ceof_token>::const_iterator it = _token_it;
  ++it;
  return (it != _token_end
            && it->get_parent_token() == token_number);
}

/**
 *  Enter the children.
 *
 *  @return  An iterator to the children.
 */
ceof_iterator ceof_iterator::enter_children() const throw() {
  return (has_children() ?
            ceof_iterator(_token_it + 1, _token_end)
              : ceof_iterator());
}

/**
 *  Is this an end iterator?
 *
 *  @return  True if this an end iterator.
 */
bool ceof_iterator::end() const throw() {
  return (_token_it == _token_end);
}
