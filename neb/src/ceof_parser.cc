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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/ceof_parser.hh"

using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[in] str  The string to parse.
 */
ceof_parser::ceof_parser(std::string const& str)
  : _string(str) {

}

/**
 *  Destructor.
 */
ceof_parser::~ceof_parser() throw() {

}

static void skip(
              std::string::const_iterator& it,
              std::string::const_iterator const& end) {
  for (;
       it != end && (*it == ' ' || *it == '\t' || *it == '\n');
       ++it);
}

/**
 *  Parse the string given to the parser.
 *
 *  @return  An iterator to iterate over the document.
 */
ceof_iterator ceof_parser::parse() {
  // Clear the tokens.
  _tokens.clear();

  // Declare the state machine.
  enum {
    waiting_for_object,
    in_object
  } state = waiting_for_object;

  // Parse the string.
  std::string::const_iterator actual = _string.begin();
  std::string::const_iterator end = _string.end();

  while (actual != end) {
    // Skip.
    skip(actual, end);


  }

  return (_tokens.empty()
            ? ceof_iterator()
              : ceof_iterator(&_tokens[0], _tokens.size()));
}
