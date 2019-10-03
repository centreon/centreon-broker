/*
** Copyright 2015 Centreon
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

#ifndef CCB_MISC_TOKENIZER_HH
#define CCB_MISC_TOKENIZER_HH

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
template <typename T>
/**
 *  Get a value from a stringstream.
 *
 *  @param[in] ss  The stringstream.
 *
 *  @return        The value.
 */
T from_string_stream(std::stringstream& ss) {
  T ret;
  ss >> ret;
  return (ret);
}

template <>
std::string from_string_stream(std::stringstream& ss);

/**
 *  @class tokenizer tokenizer.hh "com/centreon/broker/misc/tokenizer.hh"
 *  @brief Simple tokenizer.
 *
 *  Tokenize like a tokenizer should.
 */
class tokenizer {
 public:
  tokenizer(std::string const& line, char separator = ';');
  ~tokenizer();

  /**
   *  Get the next token.
   *
   *  @param[in] optional  Is the token optional.
   */
  template <typename T>
  T get_next_token(bool optional = false) {
    char* position = ::strchr(_index, _separator);

    std::string arg;

    if (position == NULL)
      position = _line + ::strlen(_line);

    arg = std::string(_index, position - _index);

    if (arg.empty() && !optional)
      throw(exceptions::msg() << "expected non optional argument " << _pos
                              << " empty or not found");

    std::stringstream ss;
    ss << arg;
    T ret = from_string_stream<T>(ss);
    if (ss.fail())
      throw(exceptions::msg() << "can't convert '" << ss.str()
                              << "' to expected type for pos " << _pos);

    _index = *position ? position + 1 : position;
    ++_pos;

    return (ret);
  }

 private:
  char* _line;
  char _separator;
  uint32_t _pos;
  char* _index;

  tokenizer();
  tokenizer(tokenizer const& other);
  tokenizer& operator=(tokenizer const& other);
};
}  // namespace misc

CCB_END()

#endif  // !CCB_MISC_TOKENIZER_HH
