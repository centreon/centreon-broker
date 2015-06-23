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

#include "com/centreon/broker/misc/tokenizer.hh"

using namespace com::centreon::broker::misc;

/**
 *  Tokenizer constructor.
 *
 *  @param[in] line       The line to tokenize.
 *  @param[in] separator  The seperator.
 */
tokenizer::tokenizer(std::string const& line, char separator /*= ';'*/)
  : _separator(separator),
    _pos(0),
    _index(0) {
  _line = ::strdup(line.c_str());
  if (_line == NULL)
    throw (exceptions::msg() << "can't allocate line for tokenizer");
  _index = _line;
}

/**
 *  Destructor.
 */
tokenizer::~tokenizer() {
  ::free(_line);
}

template <>
/**
 *  Get a string from a stringstream.
 *
 *  @param[in] ss  The stringstream.
 *
 *  @return        The string.
 */
std::string com::centreon::broker::misc::from_string_stream(
              std::stringstream& ss) {
  return (ss.str());
}
