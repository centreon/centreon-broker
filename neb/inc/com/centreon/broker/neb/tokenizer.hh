/*
** Copyright 2015 Merethis
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

#ifndef CCB_NEB_TOKENIZER_HH
#  define CCB_NEB_TOKENIZER_HH

#  include <string>
#  include <cstring>
#  include <sstream>
#  include <cstdlib>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/exceptions/msg.hh"
#  include "com/centreon/broker/logging/logging.hh"

CCB_BEGIN()

namespace        neb {
  /**
   *  @class tokenizer tokenizer.hh "com/centreon/broker/neb/tokenizer.hh"
   *  @brief Simple tokenizer.
   *
   *  Tokenize like a tokenizer should.
   */
  class          tokenizer {
  public:
                 tokenizer(std::string const& line, char separator = ';');
                 ~tokenizer();

    void         begin();
    /**
     *  Get the next token.
     *
     *  @param[in] optional  Is the token optional.
     */
    template <typename T>
    T            get_next_token(bool optional = false) {
      char* position = ::strchr(_index, _separator);

      std::string arg;

      if (position == NULL)
        position = _line + ::strlen(_line);

      arg = std::string(_index, position - _index);

      if (arg.empty() && !optional)
        throw (exceptions::msg()
               << "expected non optional argument "
               << _pos << " empty or not found");

      std::stringstream ss;
      ss << arg;
      T ret;
      ss >> ret;
      if (ss.fail())
        throw (exceptions::msg()
               << "can't convert '" << ss.str()
               << "' to expected type for pos " << _pos);

      _index = *position ? position + 1 : position;
      ++_pos;

      return (ret);
    }

    void         end();

  private:
    char*        _line;
    char         _separator;
    unsigned int
                 _pos;
    char*        _index;

                 tokenizer();
                 tokenizer(tokenizer const& other);
    tokenizer&
                 operator=(tokenizer const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_TOKENIZER_HH
