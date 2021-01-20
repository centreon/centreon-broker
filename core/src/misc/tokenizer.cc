/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/misc/tokenizer.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::misc;

/**
 *  Tokenizer constructor.
 *
 *  @param[in] line       The line to tokenize.
 *  @param[in] separator  The seperator.
 */
tokenizer::tokenizer(std::string const& line, char separator /*= ';'*/)
    : _separator(separator), _pos(0), _index(nullptr) {
  _line = ::strdup(line.c_str());
  if (_line == nullptr)
    throw msg_fmt("can't allocate line for tokenizer");
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
