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

using namespace com::centreon::broker::ceof;

/**
 *  Default constructor.
 */
ceof_iterator::ceof_iterator() {}

/**
 *  Constructor.
 *
 *  @param[in] tokens        A vector of tokens.
 *  @param[in] token_number  The number of tokens.
 */
ceof_iterator::ceof_iterator(
    std::vector<ceof_token>::const_iterator const& begin,
    std::vector<ceof_token>::const_iterator const& end)
    : _token_it(begin), _token_end(end) {}

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
ceof_iterator::~ceof_iterator() throw() {}

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
  return (!operator==(other));
}

/**
 *  @brief Go forward one token.
 *
 *  Skip children tokens.
 *
 *  @return  Reference to this object.
 */
ceof_iterator& ceof_iterator::operator++() throw() {
  int parent_token = _token_it->get_parent_token();
  for (++_token_it; (_token_it != _token_end) &&
                    (_token_it->get_parent_token() != parent_token);
       ++_token_it)
    ;
  return (*this);
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
  int token_number = _token_it->get_token_number();
  std::vector<ceof_token>::const_iterator it = _token_it;
  ++it;
  return ((it != _token_end) && (it->get_parent_token() == token_number));
}

/**
 *  Enter the children.
 *
 *  @return  An iterator to the children.
 */
ceof_iterator ceof_iterator::enter_children() const throw() {
  return (has_children() ? ceof_iterator(_token_it + 1, _token_end)
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
